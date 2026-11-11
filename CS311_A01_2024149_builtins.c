#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include "CS311_A01_2024149_builtins.h"
#include "CS311_A01_2024149_process_list.h"

int execute_builtin(Command *cmd)
{
    if (cmd->argc == 0)
        return 0;

    if (strcmp(cmd->argv[0], "cd") == 0)
    {
        char *path = cmd->argv[1];
        if (path == NULL)
        {
            path = getenv("HOME"); // Default to home directory
            if (path == NULL)
            {
                fprintf(stderr, "cd: HOME not set\n");
                return 1;
            }
        }
        if (chdir(path) != 0)
        {
            perror("cd failed");
        }
        return 1;
    }

    if (strcmp(cmd->argv[0], "pwd") == 0)
    {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s\n", cwd);
        }
        else
        {
            perror("pwd failed");
        }
        return 1;
    }

    if (strcmp(cmd->argv[0], "ps") == 0)
    {
        print_processes();
        return 1;
    }

    if (strcmp(cmd->argv[0], "kill") == 0)
    {
        if (cmd->argv[1] == NULL)
        {
            fprintf(stderr, "kill: expected PID argument\n");
        }
        else
        {
            char *endptr;
            // Use strtol for strict validation instead of atoi
            long target_pid = strtol(cmd->argv[1], &endptr, 10);

            // Prevent killing PID <= 0 (which kills process groups) or invalid strings
            if (*endptr != '\0' || target_pid <= 0)
            {
                fprintf(stderr, "kill: invalid PID format\n");
                return 1;
            }

            if (kill((pid_t)target_pid, SIGTERM) == 0)
            {
                remove_process((pid_t)target_pid);
                printf("Process %ld killed.\n", target_pid);
            }
            else
            {
                perror("kill failed");
            }
        }
        return 1;
    }

    if (strcmp(cmd->argv[0], "exit") == 0)
    {
        return -1; // Special return value to signal shell exit
    }

    return 0; // Not a built-in
}