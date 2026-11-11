#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include <sys/types.h>

// Node for our linked list tracking active processes
typedef struct ProcessNode {
    pid_t pid;
    char *command_name;
    struct ProcessNode *next;
} ProcessNode;

// Global head of the process list
extern ProcessNode *process_list_head;

// Function prototypes
void add_process(pid_t pid, const char *cmd_name);
void remove_process(pid_t pid);
void print_processes();
void cleanup_processes();

#endif