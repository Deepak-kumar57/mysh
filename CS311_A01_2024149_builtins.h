#ifndef BUILTINS_H
#define BUILTINS_H

#include "CS311_A01_2024149_parser.h"

// Returns 1 if the command was a built-in and executed successfully.
// Returns -1 if the shell should exit.
// Returns 0 if the command is not a built-in.
int execute_builtin(Command *cmd);

#endif