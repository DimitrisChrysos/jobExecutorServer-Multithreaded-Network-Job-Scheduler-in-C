#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include "ServerCommands.h"
#include <semaphore.h>

ServerInfo *info;

typedef struct controller_args ControllerArgs;
typedef struct controller_args {
    int commander_socket;
} ControllerArgs;

void* controller_thread(void* myArgs) {
    // int commander_socket = *((int *)com_socket);
    ControllerArgs* insideArgs = (ControllerArgs*)myArgs;
    int commander_socket = insideArgs->commander_socket;

    // read the number of words of the job
    int total_words;
    read(commander_socket, &total_words, sizeof(int));
    // printf("SERVER: total_words = %d\n", total_words);

    // read the total_len of the job
    int total_len;
    read(commander_socket, &total_len, sizeof(int));
    // printf("SERVER: total_len = %d\n", total_len);

    // read the job from the Commander and print it
    char *commander_message = (char*)calloc(total_len, sizeof(char));
    read(commander_socket, commander_message, total_len*sizeof(char));
    printf("SERVER: %s\n\n", commander_message);

    // save the commander_message as full_job
    char* full_job = (char*)calloc(total_len, sizeof(char));
    strcpy(full_job, commander_message);

    // tokenize the string
    char** tokenized = (char **)malloc(total_words * sizeof(char*));   
    for (int i = 0 ; i < total_words ; i++) {
        tokenized[i] = malloc(total_len * sizeof(char));
    } 
    char* tok = strtok(commander_message, " ");
    int count = 0;
    while (tok != NULL) {
        if (count == total_words) {
            break;
        }
        strcpy(tokenized[count], tok);
        tok = strtok(NULL, " ");
        count++;
    }

    // // print the tokens
    // for (int i = 0 ; i < total_words ; i++) {
    //     printf("tok[%d] = %s\n", i, tokenized[i]);
    // }

    // call the commands function to execute the command
    char* returned_message = commands(tokenized, full_job, commander_socket);

    // send the length of the message to be sent afterwards
    int len = strlen(returned_message) + 1;
    send(commander_socket, &len, sizeof(int), 0);

    // send a message back to the Commander
    send(commander_socket, returned_message, sizeof(char)*(strlen(returned_message) + 1), 0);

    // free the commander_message, full_job, the returned_message and the com_socket
    free(commander_message);
    free(full_job);
    free(returned_message);
    free(insideArgs);

    // free the memory of "tokenized"
    for (int i = 0; i < total_words; i++) {
        if (tokenized[i] != NULL) {
            free(tokenized[i]);
        }
    }

    // // close the Commander socket
    // close(commander_socket);
}

void jobExecutorServer(int argc, char *argv[]) {

    // name the arguments
    int port = atoi(argv[1]);   // the port of the Server

    // create the server socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // set the socket
    struct sockaddr_in addr;
    int options = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &options, sizeof(options));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
    if (bind(server_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        printf("binding the socket failed...\n");
    }

    // Prepare to accept connections
    listen(server_socket, 5);

    // save the Controller threads to join them after while 
    int controller_count = 0;
    int arr_size = 10;
    pthread_t* thread_arr = (pthread_t*)malloc(arr_size*sizeof(pthread_t));

    // keep the server open
    while (info->open) {

        // accept a new connection from the Commander
        int commander_socket = accept(server_socket, NULL, NULL);
        
        // read if exit
        int exit;
        read(commander_socket, &exit, sizeof(int));

        // handle the Commander-Server communication
        pthread_t temp_thread;
        ControllerArgs* myArgs = (ControllerArgs*)malloc(sizeof(ControllerArgs));
        myArgs->commander_socket = commander_socket;
        pthread_create(&temp_thread, NULL, &controller_thread, (void*)myArgs);

        // save the thread to join it later
        // if thread array is filled, double it's size
        if (controller_count == arr_size) {
            arr_size += arr_size;
            thread_arr = (pthread_t*)realloc(thread_arr, arr_size*sizeof(pthread_t));
        }
        thread_arr[controller_count] = temp_thread;
        controller_count++;

        // if exit, wait for all the threads to join
        // when the last thread joins, info->open is going to close and we will exit the loop
        if (exit) {
            for (int i = 0 ; i < controller_count ; i++) {
                pthread_join(thread_arr[i], NULL);
            }
        }
    }

    // free the memory allocated for the thread array
    free(thread_arr);
}

int main(int argc, char *argv[]) {

    // create a Queue for the jobs
    Queue* myqueue = createQueue();

    // init the ServerInfo struct and set the global pointer
    ServerInfo myServerInfo = {myqueue, 1, 0, 1, atoi(argv[2]), atoi(argv[3]), NULL, NULL};
    info = &myServerInfo;
    pthread_mutex_init(info->mutex_worker, NULL);   // init worker mutex
    pthread_cond_init(info->cond_worker, NULL); // init cond var mutex

    // create the worker threads
    pthread_t work_thr[info->threadPoolSize];
    for (int i = 0 ; i < info->threadPoolSize ; i++) {
        pthread_create(&(work_thr[i]), NULL, &worker_threads, NULL);
    }

    // call the jobExecutorServer function
    jobExecutorServer(argc, argv);

    // join the worker threads
    for (int i = 0 ; i < info->threadPoolSize ; i++) {
        pthread_join(work_thr[i], NULL);
    }

    // delete every triplet of the queue
    if (info->myqueue->size > 0) {  // free the memory from the waiting queue
        int qSize = info->myqueue->size;
        Node* temp_node = info->myqueue->first_node;
        for (int i = 0 ; i < qSize ; i++) {
            Triplet* tempTriplet = temp_node->value;
            delete_triplet(tempTriplet);
            temp_node = temp_node->child;
        }
    }

    // delete the Queue
    deleteQueue(info->myqueue);

    // destroy the mutex and the cond var of the server info
    pthread_mutex_destroy(info->mutex_worker);
    pthread_cond_destroy(&info->cond_worker);
}