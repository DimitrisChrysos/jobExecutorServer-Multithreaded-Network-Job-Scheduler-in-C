#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
// #include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "ServerCommands.h"
#include <semaphore.h>

ServerInfo *info;

void jobExecutorServer(int argc, char *argv[]) {

    // name the arguments
    int port = atoi(argv[1]);   // the port of the Server

    // create the server socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // set the socket
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
    bind(server_socket, (struct sockaddr *) &addr, sizeof(addr));

    // Prepare to accept connections
    listen(server_socket, 5);


    // keep the server open
    while (info->open) {
        
        // accept a new connection from the Commander
        int commander_socket = accept(server_socket, NULL, NULL);

        // read the message from the Commander and print it
        char commander_message[1024] = { 0 };
        read(commander_socket, commander_message, sizeof(commander_message));
        printf("SERVER: %s\n", commander_message);

        // send a message back to the Commander
        char server_message[256] = "The server received the message from the commander!";
        send(commander_socket, server_message, sizeof(server_message), 0);

        // close the Commander socket
        close(commander_socket);
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