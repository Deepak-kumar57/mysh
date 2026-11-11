#include <stdio.h>
#include <stdlib.h>
#include "linenoise.h"
#include "CS311_A01_2024149_parser.h"
#include "CS311_A01_2024149_execute.h"
#include "CS311_A01_2024149_process_list.h"

int main(int argc, char **argv)
{
    char *line;
    Command cmds[MAX_COMMANDS];
    int num_cmds;
    int status = 1;

    // Display a cool prompt of your choosing
    const char *prompt = "\033[1;32mdeepsshell>\033[0m ";

    while (status)
    {
        line = linenoise(prompt);
        if (line == NULL)
        {
            break; // Terminate on EOF (Ctrl+D)
        }

        reap_zombies();

        if (line[0] != '\0')
        {
            linenoiseHistoryAdd(line); // Add to history for arrow key navigation
            num_cmds = parse_line(line, cmds);
            for (int i = 0; i < num_cmds && status; i++)
            {
                if (cmds[i].pipe_next)
                {
                    // Collect the chain of commands joined by '|'
                    int start = i;
                    while (i < num_cmds - 1 && cmds[i].pipe_next)
                        i++;
                    execute_pipeline(&cmds[start], i - start + 1);
                }
                else
                {
                    status = execute_command(&cmds[i]);
                }
            }
        }

        free(line); 
    }

    cleanup_processes(); 
    return 0;
}