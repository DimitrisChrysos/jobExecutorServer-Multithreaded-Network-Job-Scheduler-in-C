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

Triplet* issueJob(char* job, int commander_socket);

char* commands(char** tokenized, char* unix_command, int commander_socket) {
    if (strcmp(tokenized[0], "issueJob" ) == 0) {
        
        Triplet* myTriplet = issueJob(unix_command, commander_socket);
        char* message = format_triplet(myTriplet);
        return message;
    }
    else if (strcmp(tokenized[0], "setConcurrency" ) == 0) {

        info->concurrency = atoi(tokenized[1]);
        // exec_commands_in_queue(-1);
        char* message = (char*)malloc((strlen("CONCURRENCY SET AT Ν") + 20)*sizeof(char));
        sprintf(message, "CONCURRENCY SET AT %d", info->concurrency);
        return message;
    }
    else if (strcmp(tokenized[0], "stop" ) == 0) {
        
        char* message = stop_job(tokenized);
        return message; 
    }
    else if (strcmp(tokenized[0], "poll" ) == 0) {
        
        char* message = poll(tokenized);
        return message; 
    }
    else if (strcmp(tokenized[0], "exit" ) == 0) {
        
        char* message = exit_server();

        // send signal to worker_threads to unpause and join (when no issueJobs)
        pthread_cond_broadcast(info->cond_worker);
        return message;
    }
}

void* worker_threads(void* arg) {
    pthread_mutex_lock(info->mutex_worker);

    while(info->open) {

        // wait for a signal, from an issueJob commander (or from exit)
        pthread_cond_wait(info->cond_worker, info->mutex_worker);

        if (info->open == 0)
            break;
        
        // check if we can execute a job
        if (info->active_processes < info->concurrency && info->myqueue->size != 0) {

            // get the first process "job" of the queue to be executed
            Triplet* mytriplet = info->myqueue->first_node->value;
            int len = strlen(mytriplet->job);
            char job[len];
            strcpy(job, mytriplet->job);

            // find the "amount" of words in the "job" to be executed
            int amount = 0;
            int total_len = strlen(job) + 1;
            for (int i = 0 ; i < total_len ; i++) {
                if (job[i] == ' ') {
                    amount++;
                } 
            }
            amount++;

            // tokenize the string "job" using "amount + 1" for the tokenized array to end with NULL
            char** tokenized = (char **)malloc((amount+1)*sizeof(char*));
            for (int i = 0 ; i < amount ; i++) {
                tokenized[i] = (char*)malloc((total_len+1) * sizeof(char));
            }
            tokenized[amount] = NULL;
            char* tok = strtok(job, " ");
            int count = 0;
            while (tok != NULL) {
                if (count == amount) {
                    break;
                }
                strcpy(tokenized[count], tok);
                tok = strtok(NULL, " ");
                count++;
            }
            tokenized[amount] = NULL;

            // remove the front process from the queue and add one to the active_processes
            Triplet* removed_triplet = dequeue(info->myqueue);
            info->active_processes++;

            // execute the process using fork() and execvp(),
            // wait for it to end and send its output to the client
            pid_t pid = fork();
            if (pid == 0) { // child process
                
                // create the file name and the file
                pid_t child_pid = getpid();
                char file_name[100];
                sprintf(file_name, "%d.output", child_pid);
                int file = open(file_name, O_WRONLY | O_CREAT, 0777);

                // redirect standard output to the file we created
                dup2(file, STDOUT_FILENO);

                // close the file
                close(file);

                // execute the wanted process
                execvp(tokenized[0], tokenized);
            }
            else if (pid > 0) { // parent process

                // wait for the child to finish
                pid_t child_pid = wait(NULL);
                info->active_processes--;

                // open the file created from the child, find its size and read it
                char file_name[100];
                sprintf(file_name, "%d.output", child_pid);
                FILE* file = fopen(file_name, "r");
                fseek(file, 0, SEEK_END);   // find the size of the file
                long total_chars = ftell(file);
                fseek(file, 0, SEEK_SET);   // get back to the starting position, of the file
                char* buffer = (char*)malloc(sizeof(char)*(total_chars + 1));
                fread(buffer, sizeof(char), total_chars, file); // read the file

                // format the message to send to Commander (client)
                char* new_buffer = (char*)malloc(sizeof(char)*(total_chars + 100));
                sprintf(new_buffer, "\n-----jobID output start------\n\n%s\
                \n-----jobID output end------\n", buffer);

                // send the len of the message and the message, to the Commander (client)
                int client_socket = mytriplet->commander_socket;
                int len = strlen(new_buffer) + 1;
                send(client_socket, &len, sizeof(int), 0);
                send(client_socket, new_buffer, sizeof(char)*(strlen(new_buffer) + 1), 0);

                // free memory, close client socket and remove the file created by the child
                free(buffer);
                free(new_buffer);
                close(client_socket);
                remove(file_name);
            }

            // free the memory of "tokenized"
            for (int i = 0; i < amount + 1; i++) {
                if (tokenized[i] != NULL) {
                    free(tokenized[i]);
                }
            }
        }
    }
    
    pthread_mutex_unlock(info->mutex_worker);
}

Triplet* issueJob(char* job, int commander_socket) {

    // format the triplet variables
    static int counter = 0;
    counter++;
    char jobID[100];
    sprintf(jobID, "job_%d", counter);

    // create a job Triplet for the queue
    Triplet* mytriplet = init_triplet(jobID, job, commander_socket);

    // add the job to the queue only if the buffer is not full
    if (info->myqueue->size < info->bufferSize) {
        enqueue(info->myqueue, mytriplet);
        // printf("queue_size = %d || bufSize = %d\n", info->myqueue->size, info->bufferSize);
        pthread_cond_broadcast(info->cond_worker);
    }

    // TODO: need to make it check, if it can run and then run

    return mytriplet;
}

char* stop_job(char** tokenized) {

    // get jobID and allocate a right amount of memory for the message to be returned
    char* jobID = tokenized[1];
    char* buffer = (char*)malloc(sizeof(char)*(strlen(jobID) + 20));

    // Check if the process with jobID is waiting
    int waiting = 0;
    int qSize = info->myqueue->size;
    Triplet* tempTriplet;
    Node* temp_node = info->myqueue->first_node;
    for (int i = 0 ; i < qSize ; i++) {
        tempTriplet = temp_node->value;
        if (strcmp(tempTriplet->jobID, jobID) == 0) {
            waiting = 1;
            break;
        }
        temp_node = temp_node->child;
    }

    // if it's waiting, remove it from myqueue
    // at the end, found or not, return the appropriate message
    if (waiting) {
        if (temp_node->parent != NULL) {
            if (temp_node->child == NULL) {
                info->myqueue->last_node = temp_node->parent;
            }
            else {
                temp_node->child->parent = temp_node->parent;
            }
            temp_node->parent->child = temp_node->child;
            info->myqueue->size--;
            delete_triplet(tempTriplet);
            free(temp_node);
        }
        else if (temp_node->child == NULL) {
            tempTriplet = dequeue(info->myqueue);
            delete_triplet(tempTriplet);
        }
        else {
            info->myqueue->first_node = temp_node->child;
            temp_node->child->parent = NULL;
            info->myqueue->size--;
            delete_triplet(tempTriplet);
            free(temp_node);
        }
        sprintf(buffer, "JOB <%s> REMOVED", jobID);
        return buffer;
    }
    else {
        sprintf(buffer, "JOB <%s> NOTFOUND", jobID);
        return buffer;
    }

}

char* poll(char** tokenized) {
    
    // find total size of the formatted triplets 
    // and save the pointer for each triplets in an array
    Queue* myqueue = info->myqueue;
    Triplet* tempTriplet;
    int total_size = 0;
    int qSize = myqueue->size;
    char* TripletPointerArray[qSize];
    Node* temp_node = myqueue->first_node;
    for (int i = 0 ; i < qSize ; i++) {
        tempTriplet = temp_node->value;
        int tempSZ = (strlen(tempTriplet->job) + strlen(tempTriplet->jobID) + 10)*2;
        total_size += tempSZ;
        TripletPointerArray[i] = format_triplet_basic(tempTriplet);
        temp_node = temp_node->child;
    }

    // create the message (string), consisting of all the triplets to be returned
    char* buffer = (char*)malloc(total_size + (total_size / 2));
    buffer[0] = '\0';
    strcat(buffer, "\n");
    for (int i = 0 ; i < qSize ; i ++) {
        strcat(buffer, TripletPointerArray[i]);
        free(TripletPointerArray[i]);   // free the memory for each formatted_triplet
        if (i == qSize - 1) // to skip "\n" at the end of the string
            continue;
        strcat(buffer, "\n");
    }

    // return the message
    return buffer;
}


char* exit_server() {

    // close the server
    info->open = 0;

    // send a message to all the Commanders (clients), that the server terminated
    // without executing their command and close the client_socket
    char client_msg[] = "SERVER TERMINATED BEFORE EXECUTION";
    char* send_msg = (char*)malloc(sizeof(char)*(strlen(client_msg) + 1));
    strcpy(send_msg, client_msg);
    Node* tempNode = info->myqueue->first_node;
    Triplet* tempTriplet;
    for (int i = 0 ; i < info->myqueue->size ; i++) {
        tempTriplet = tempNode->value;
        int client_socket = tempTriplet->commander_socket;
        int mes_len = strlen(client_msg) + 1;
        send(client_socket, &mes_len, sizeof(int), 0);    // send message length
        send(client_socket, send_msg, sizeof(char)*(strlen(send_msg) + 1), 0);  // send message
        close(client_socket);
        tempNode = tempNode->child;
    }
    free(send_msg);

    // return the message
    char buf[] = "SERVER TERMINATED";
    char* message = (char*)malloc(sizeof(char)*(strlen(buf) + 1));
    strcpy(message, buf);
    return message;
}