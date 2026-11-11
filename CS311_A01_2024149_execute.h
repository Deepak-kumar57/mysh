#ifndef EXECUTE_H
#define EXECUTE_H

#include "CS311_A01_2024149_parser.h"

// Executes a parsed command. Returns 1 if the shell should continue, 0 if it should exit.
int execute_command(Command *cmd);

// Applies '<', '>' and '>>' redirection (used inside a child process)
void setup_redirection(Command *cmd);

// Executes n commands connected with pipes (cmd1 | cmd2 | ...)
void execute_pipeline(Command *cmds, int n);

// Cleans up finished background processes to prevent zombies
void reap_zombies();

#endif