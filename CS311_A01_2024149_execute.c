#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "CS311_A01_2024149_execute.h"
#include "CS311_A01_2024149_builtins.h"
#include "CS311_A01_2024149_process_list.h"

void reap_zombies()
{
    int status;
    pid_t pid;
    // WNOHANG ensures waitpid doesn't block if no children have exited
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        remove_process(pid);
    }
}

int execute_command(Command *cmd)
{
    if (cmd->argc == 0)
    {
        return 1; 
    }

    // --- Built-in Commands Routing ---
    int builtin_status = execute_builtin(cmd);
    if (builtin_status == 1)
        return 1; 
    if (builtin_status == -1)
        return 0; 

    // --- External Commands ---
    pid_t pid = fork(); //

    if (pid < 0)
    {
        perror("fork failed"); 
        return 1;
    }

    if (pid == 0)
    {
        // Child Process: Setup I/O Redirection
        setup_redirection(cmd);

        // Execute the program
        if (execvp(cmd->argv[0], cmd->argv) == -1)
        {
            perror("command execution failed");
        }
        exit(EXIT_FAILURE); // Only reached if exec fails
    }
    else
    {
        // Parent Process[cite: 1]
        add_process(pid, cmd->argv[0]);

        if (cmd->background)
        {
            // Do not wait for background processes
            printf("[Background] PID %d started: %s\n", pid, cmd->argv[0]);
        }
        else
        {
            // Wait for foreground process to finish
            int status;
            waitpid(pid, &status, 0);
            remove_process(pid);      // Remove from tracking once it finishes
        }
    }

    return 1;
}

// Applies '<', '>' and '>>' redirection in the child process (called after fork)
void setup_redirection(Command *cmd)
{
        if (cmd->input_file)
        {
            int fd_in = open(cmd->input_file, O_RDONLY);
            if (fd_in < 0)
            {
                perror("failed to open input file");
                exit(EXIT_FAILURE);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (cmd->output_file)
        {
            int flags = O_WRONLY | O_CREAT;
            flags |= cmd->append_output ? O_APPEND : O_TRUNC; 

            // 0644 gives read/write permissions to owner, read to others
            int fd_out = open(cmd->output_file, flags, 0644);
            if (fd_out < 0)
            {
                perror("failed to open output file");
                exit(EXIT_FAILURE);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
}

// Executes n commands connected by pipes: cmds[0] | cmds[1] | ... | cmds[n-1]
void execute_pipeline(Command *cmds, int n)
{
    pid_t pids[MAX_COMMANDS];
    int started = 0;
    int prev_read = -1; // Read end of the pipe coming from the previous command

    for (int i = 0; i < n; i++)
    {
        int fd[2] = {-1, -1};
        // Every command except the last one writes into a new pipe
        if (i < n - 1 && pipe(fd) < 0)
        {
            perror("pipe failed");
            break;
        }

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
            if (fd[0] != -1)
            {
                close(fd[0]);
                close(fd[1]);
            }
            break;
        }

        if (pid == 0)
        {
            // Child: read from the previous pipe, write into the next pipe
            if (prev_read != -1)
            {
                dup2(prev_read, STDIN_FILENO);
                close(prev_read);
            }
            if (i < n - 1)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
            }
            // '<' / '>' on a command still apply (and override the pipe)
            setup_redirection(&cmds[i]);

            // Built-ins like pwd or ps can also be used inside a pipe
            if (execute_builtin(&cmds[i]) != 0)
            {
                fflush(stdout);
                exit(EXIT_SUCCESS);
            }

            execvp(cmds[i].argv[0], cmds[i].argv);
            perror("command execution failed");
            exit(EXIT_FAILURE);
        }

        // Parent: close the pipe ends it no longer needs, otherwise the
        // reading command never sees EOF
        if (prev_read != -1)
            close(prev_read);
        prev_read = -1;
        if (i < n - 1)
        {
            close(fd[1]);
            prev_read = fd[0];
        }

        pids[started++] = pid;
        add_process(pid, cmds[i].argv[0]);
    }

    if (prev_read != -1)
        close(prev_read);

    if (cmds[n - 1].background)
    {
        // '&' after the last command runs the whole pipeline in background
        for (int i = 0; i < started; i++)
            printf("[Background] PID %d started: %s\n", pids[i], cmds[i].argv[0]);
        return;
    }

    // Wait for every command in the pipeline to finish
    for (int i = 0; i < started; i++)
    {
        int status;
        waitpid(pids[i], &status, 0);
        remove_process(pids[i]);
    }
}
