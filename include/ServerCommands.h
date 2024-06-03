#include "queue.h"
#include <semaphore.h>

// struct to save info for the server (global access through pointer)
typedef struct server_info ServerInfo;
typedef struct server_info {
    Queue* myqueue;
    int concurrency;
    int active_processes;
    int open;
    int bufferSize;
    int threadPoolSize;
    pthread_mutex_t* mutex_worker;
    pthread_cond_t* cond_worker;
    pthread_mutex_t* mutex_controller;
    pthread_cond_t* cond_controller;
} ServerInfo;

extern ServerInfo *info;

// matches the command given to it's corresponding function
char* commands(char** tokenized, char* unix_command, int commander_socket);

// function that satisfies the worker threads operations 
void* worker_threads(void* arg);

// function for the issueJob command
Triplet* issueJob(char* job, int commander_socket);

// function for the stop jobID command
char* stop_job(char** tokenized);

// function for the poll command
char* poll(char** tokenized);

// function for the exit command
char* exit_server();