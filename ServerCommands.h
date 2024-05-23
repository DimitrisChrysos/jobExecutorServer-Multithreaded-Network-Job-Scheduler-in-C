#include "queue.h"
#include <semaphore.h>

// struct to save info for the server (global access through pointer)
typedef struct server_info ServerInfo;
typedef struct server_info {
    Queue* myqueue;
    int concurrency;
    int open;
    int bufferSize;
    int threadPoolSize;
} ServerInfo;

extern ServerInfo *info;

// matches the command given to it's corresponding function
char* commands(char** tokenized, char* unix_command);

// // the signal_handler after SIGCHILD signal - executes the processes in queue
// void exec_commands_in_queue(int sig);

// function for the issueJob command
Doublet* issueJob(char* job);

// function for the stop jobID command
char* stop_job(char** tokenized);

// function for the poll command
char* poll(char** tokenized);