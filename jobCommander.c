#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "queue.h"
#include <semaphore.h>

#define PACKET_CAPACITY 100 // the max capacity of each packet

int jobCommander(int argc, char *argv[]) {
    
    // name the arguments
    int port = atoi(argv[2]);   // the port of the Server
    char* job = argv[3];    // the job to be sent, to the Server

    // create a socket for this commander
    int commander_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // set the socket
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // connect to the socket
    connect(commander_fd, (struct sockaddr *) &addr, sizeof(addr));

    // send the job to the server
    send(commander_fd, job, strlen(job), 0);

    // read a message from the server and print it
    char server_message[256];
    read(commander_fd, &server_message, sizeof(server_message));
    printf("The server sent the data: %s\n", server_message);

    // close the commander socket
    close(commander_fd);

    return 0;
}




void main(int argc, char *argv[]) {
    
    // call the jobCommander function
    jobCommander(argc, argv);
}