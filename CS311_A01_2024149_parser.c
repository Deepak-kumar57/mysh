#include <stdio.h>
#include <string.h>
#include "CS311_A01_2024149_parser.h"

void parse_input(char *line, Command *cmd)
{
    // Zero out the struct to prevent undefined behavior from stale data
    memset(cmd, 0, sizeof(Command));

    // Tokenize the input string by spaces, tabs, and newlines
    char *token = strtok(line, " \t\n");

    while (token != NULL && cmd->argc < MAX_ARGS - 1)
    {
        if (strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " \t\n"); // Grab the next token as the filename
            if (token)
                cmd->input_file = token;
        }
        else if (strcmp(token, ">") == 0)
        {
            token = strtok(NULL, " \t\n");
            if (token)
                cmd->output_file = token;
        }
        else if (strcmp(token, ">>") == 0)
        {
            cmd->append_output = 1;
            token = strtok(NULL, " \t\n");
            if (token)
                cmd->output_file = token;
        }
        else if (strcmp(token, "&") == 0)
        {
            cmd->background = 1;
        }
        else
        {
            // Standard argument
            cmd->argv[cmd->argc++] = token;
        }
        // Fetch the next token
        token = strtok(NULL, " \t\n");
    }

    // exec() family functions require the argument array to be NULL-terminated
    cmd->argv[cmd->argc] = NULL;
}


int parse_line(char *line, Command cmds[MAX_COMMANDS])
{
    char *segments[MAX_COMMANDS];
    char seg_end[MAX_COMMANDS]; // Character that ended each segment: '&', '|' or '\0'
    int seg_count = 0;
    char *seg_start = line;

    for (char *p = line;; p++)
    {
        if (*p == '&' || *p == '|' || *p == '\0')
        {
            int at_end = (*p == '\0');
            if (seg_count < MAX_COMMANDS)
            {
                seg_end[seg_count] = *p;
                segments[seg_count++] = seg_start;
            }
            if (!at_end)
                *p = '\0'; 
            if (at_end)
                break;
            seg_start = p + 1;
        }
    }

    int num_cmds = 0;
    int prev_pipe = 0;
    for (int i = 0; i < seg_count && num_cmds < MAX_COMMANDS; i++)
    {
        parse_input(segments[i], &cmds[num_cmds]);

        // A pipe needs a command on both sides, e.g. "| wc" or "ls |" is invalid
        if ((seg_end[i] == '|' || prev_pipe) && (cmds[num_cmds].argc == 0 || (seg_end[i] == '|' && i == seg_count - 1)))
        {
            fprintf(stderr, "syntax error: '|' must have a command on both sides\n");
            return 0;
        }
        prev_pipe = (seg_end[i] == '|');

        if (cmds[num_cmds].argc > 0)
        {
            // Segment terminated by '&' runs in background, by '|' pipes into the next one
            cmds[num_cmds].background = (seg_end[i] == '&') ? 1 : 0;
            cmds[num_cmds].pipe_next = (seg_end[i] == '|') ? 1 : 0;
            num_cmds++;
        }
    }

    return num_cmds;
}