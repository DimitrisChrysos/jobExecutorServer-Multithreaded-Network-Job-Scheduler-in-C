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
#include "ServerCommands.h"
#include <semaphore.h>

ServerInfo *info;

void handle_commander(int server_socket) {
    // accept a new connection from the Commander
    int commander_socket = accept(server_socket, NULL, NULL);

    // read the number of words of the job
    int total_words;
    read(commander_socket, &total_words, sizeof(int));
    printf("SERVER: total_words = %d\n", total_words);

    // read the total_len of the job
    int total_len;
    read(commander_socket, &total_len, sizeof(int));
    printf("SERVER: total_len = %d\n", total_len);

    // read the job from the Commander and print it
    char *commander_message = (char*)calloc(total_len, sizeof(char));
    read(commander_socket, commander_message, total_len*sizeof(char));
    printf("SERVER: %s\n\n", commander_message);

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

    // send a message back to the Commander
    char server_message[256] = "The server received the message from the commander!";
    send(commander_socket, server_message, sizeof(server_message), 0);

    // free the commander_message
    free(commander_message);

    // free the memory of "tokenized"
    for (int i = 0; i < total_words; i++) {
        if (tokenized[i] != NULL) {
            free(tokenized[i]);
        }
    }

    // close the Commander socket
    close(commander_socket);
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


    // keep the server open
    while (info->open) {
        
        // handle the Commander-Server communication
        handle_commander(server_socket);
    }
}

int main(int argc, char *argv[]) {

    // create a Queue for the jobs
    Queue* myqueue = createQueue();

    // Init the ServerInfo struct and set the global pointer
    ServerInfo myServerInfo = {myqueue, 1, 1};
    info = &myServerInfo;

    // call the jobExecutorServer function
    jobExecutorServer(argc, argv);

    // delete the Queue
    deleteQueue(info->myqueue);
}