#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "ServerCommands.h"
#include <semaphore.h>

Triplet* issueJob(char* job, int commander_socket);

char* commands(char** tokenized, char* unix_command, int commander_socket) {
    if (strcmp(tokenized[0], "issueJob" ) == 0) {
        
        Triplet* returned_message = issueJob(unix_command, commander_socket);
        char* message = format_triplet(returned_message);
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
        
        info->open = 0; // close the server 
        char buf[] = "SERVER TERMINATED";
        char* message = (char*)malloc(sizeof(char)*(strlen(buf) + 1));
        strcpy(message, buf);
        return message;
    }
}

// void signal_handerUSR2(int sig) {}

// void exec_commands_in_queue(int sig) {

//     // If queued processes exist execute them, until we reach the concurrency limit
//     if (info->running_queue->size < info->concurrency && info->myqueue->size != 0) {
        
//         // execute processes until capacity (info->concurrency) reached
//         for (int i = info->running_queue->size ; i < info->concurrency ; i++) {

//             // Check if there are no processes left
//             if (info->myqueue->size == 0)
//                 break;

//             // get the first process "job" of the queue to be executed
//             Triplet* mytriplet = info->myqueue->first_node->value;
//             int len = strlen(mytriplet->job);
//             char job[len];
//             strcpy(job, mytriplet->job);

//             // find the "amount" of words in the "job" to be executed
//             int amount = 0;
//             int total_len = strlen(job) + 1;
//             for (int i = 0 ; i < total_len ; i++) {
//                 if (job[i] == ' ') {
//                     amount++;
//                 } 
//             }
//             amount++;
            
//             // tokenize the string "job" using "amount + 1" for the tokenized array to end with NULL
//             char** tokenized = (char **)malloc((amount+1)*sizeof(char*));
//             for (int i = 0 ; i < amount ; i++) {
//                 tokenized[i] = (char*)malloc((total_len+1) * sizeof(char));
//             }
//             tokenized[amount] = NULL;
//             char* tok = strtok(job, " ");
//             int count = 0;
//             while (tok != NULL) {
//                 if (count == amount) {
//                     break;
//                 }
//                 strcpy(tokenized[count], tok);
//                 tok = strtok(NULL, " ");
//                 count++;
//             }
//             tokenized[amount] = NULL;
            

//             // remove the front process from the queue and add it to the running_queue
//             Triplet* removed_triplet = dequeue(info->myqueue);
//             enqueue(info->running_queue, removed_triplet);

//             // fix the queuePosition for every item of the main queue 
//             int qSize = info->myqueue->size;
//             Node* temp_node = info->myqueue->first_node;
//             for (int i = 0 ; i < qSize ; i++) {
//                 Triplet* tempTriplet = temp_node->value;
//                 tempTriplet->queuePosition = i;
//                 temp_node = temp_node->child;
//             }
            
//             // create a new process and replace it using execvp with the wanted process
//             pid_t pid = fork();
//             if (pid == 0) { // child process

//                 // execute the wanted process
//                 execvp(tokenized[0], tokenized);
//             }
            
//             // add the pid of the process to the triplet (usefull to terminate the process)
//             removed_triplet->pid = pid;

//             // free the memory of "tokenized"
//             for (int i = 0; i < amount + 1; i++) {
//                 if (tokenized[i] != NULL) {
//                     free(tokenized[i]);
//                 }
//             }
//         }
//     }
// }

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
