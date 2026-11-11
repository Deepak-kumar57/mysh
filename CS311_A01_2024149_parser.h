#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 64
#define MAX_COMMANDS 16

// Struct to hold the parsed command components
typedef struct
{
    char *argv[MAX_ARGS]; // Array of arguments for exec()
    char *input_file;     // Target file for '<'
    char *output_file;    // Target file for '>' or '>>'
    int append_output;    // Flag (1 if '>>', 0 if '>')
    int background;       // Flag (1 if '&' is present)
    int argc;             // Argument count
    int pipe_next;        // Flag (1 if output is piped '|' into the next command)
} Command;

void parse_input(char *line, Command *cmd);
int parse_line(char *line, Command cmds[MAX_COMMANDS]);

#endif