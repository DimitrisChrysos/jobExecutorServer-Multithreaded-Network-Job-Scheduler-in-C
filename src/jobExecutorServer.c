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
#include "../include/ServerCommands.h"
#include <semaphore.h>

// stores the server info
ServerInfo *info;

// worker threads function
void* worker_threads(void* arg) {

    // keep the worker threads open
    while(info->open) {

        // if buffer empty, wait for jobs to be added
        if (info->myqueue->size == 0) {
            
            // get the lock
            pthread_mutex_lock(info->mutex_worker);
            
            // wait
            pthread_cond_wait(info->cond_worker, info->mutex_worker);
        
            // release the lock
            pthread_mutex_unlock(info->mutex_worker);
        }
        
        // check for server closure
        if (info->open == 0)
            break;
        
        // try executing a job
        execute_job();
    }
}

// controller threads function
void* controller_thread(void* myArgs) {

    // get the lock
    pthread_mutex_lock(info->mutex_controller);

    // if buffer full, wait for jobs to be removed
    if (info->myqueue->size == info->bufferSize)
        pthread_cond_wait(info->cond_controller, info->mutex_controller);

    // prepare and call the commands function, to decide the action of the server
    call_commands(myArgs);

    // release the lock
    pthread_mutex_unlock(info->mutex_controller);
}

// the main function of the server, creates controller threads and is responsible to manage the client-server connections
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
    listen(server_socket, 100);
    
    // create the detach attribute for the detached threads creations later inside the while loop
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // keep the server open
    while (info->open) {

        // accept a new connection from the Commander
        int commander_socket = accept(server_socket, NULL, NULL);
        
        // read if exit
        int exit;
        read(commander_socket, &exit, sizeof(int));

        // handle the Commander-Server communication using detached threads
        pthread_t temp_thread;
        ControllerArgs* myArgs = (ControllerArgs*)malloc(sizeof(ControllerArgs));
        myArgs->commander_socket = commander_socket;
        pthread_create(&temp_thread, &attr, &controller_thread, (void*)myArgs);

        // if exit, break the loop
        if (exit == 1) {
            pthread_attr_destroy(&attr);
            break;
        }
    }
}

int main(int argc, char *argv[]) {

    // create a Queue for the jobs
    Queue* myqueue = createQueue();

    // init the ServerInfo struct (including cond vars and mutexes) and set the global pointer
    ServerInfo myServerInfo = {myqueue, 1, 0, 1, atoi(argv[2]), atoi(argv[3]), NULL, NULL};
    info = &myServerInfo;
    info->mutex_worker = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(info->mutex_worker, NULL);   // init worker mutex
    info->cond_worker = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    pthread_cond_init(info->cond_worker, NULL); // init worker cond var
    info->mutex_controller = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(info->mutex_controller, NULL);   // init commander mutex
    info->cond_controller = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    pthread_cond_init(info->cond_controller, NULL); // init commander cond var
    info->mutex_queue = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(info->mutex_queue, NULL);   // init queue mutex
    info->mutex_concurrency = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(info->mutex_concurrency, NULL);   // init concurrency mutex

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
    pthread_cond_destroy(info->cond_worker);
    free(info->mutex_worker);
    free(info->cond_worker);
    pthread_mutex_destroy(info->mutex_controller);
    pthread_cond_destroy(info->cond_controller);
    free(info->mutex_controller);
    free(info->cond_controller);
    pthread_mutex_destroy(info->mutex_queue);
    free(info->mutex_queue);
    pthread_mutex_destroy(info->mutex_concurrency);
    free(info->mutex_concurrency);

    // wait for all the threads to finish
    pthread_exit(0);
}