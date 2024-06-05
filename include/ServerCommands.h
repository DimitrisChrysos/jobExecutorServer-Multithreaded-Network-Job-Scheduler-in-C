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
    pthread_mutex_t* mutex_queue;
} ServerInfo;

extern ServerInfo *info;

// matches the command given to it's corresponding function
char* commands(char** tokenized, char* unix_command, int commander_socket);

// function for the issueJob command 
// returns a pointer to the triplet of the job added to the queue
Triplet* issueJob(char* job, int commander_socket);

// function for the stop jobID command
char* stop_job(char** tokenized);

// function for the poll command
char* poll(char** tokenized);

// function for the exit command
char* exit_server();

// helper function for the worker_threads to execute a job
void execute_job();

// struct for the arguments passed to the controller thread and then to the call_commands() function
typedef struct controller_args ControllerArgs;
typedef struct controller_args {
    int commander_socket;
} ControllerArgs;

// helper function for the controller_threads to call the commands function
void call_commands(void* myArgs);