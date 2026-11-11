#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "CS311_A01_2024149_process_list.h"

ProcessNode *process_list_head = NULL;

void add_process(pid_t pid, const char *cmd_name)
{
    ProcessNode *new_node = (ProcessNode *)malloc(sizeof(ProcessNode));
    if (!new_node)
    {
        perror("malloc failed"); // Always check system call/library returns
        return;
    }
    new_node->pid = pid;
    new_node->command_name = strdup(cmd_name);
    new_node->next = process_list_head;
    process_list_head = new_node;
}

void remove_process(pid_t pid)
{
    ProcessNode *current = process_list_head;
    ProcessNode *prev = NULL;

    while (current != NULL)
    {
        if (current->pid == pid)
        {
            if (prev == NULL)
            {
                process_list_head = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            free(current->command_name); // Prevent memory leaks
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void print_processes()
{
    ProcessNode *current = process_list_head;
    printf("PID\tCOMMAND\n");
    while (current != NULL)
    {
        printf("%d\t%s\n", current->pid, current->command_name);
        current = current->next;
    }
}

void cleanup_processes()
{
    ProcessNode *current = process_list_head;
    while (current != NULL)
    {
        ProcessNode *next = current->next;
        free(current->command_name);
        free(current);
        current = next;
    }
    process_list_head = NULL;
}