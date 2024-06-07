#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "../include/queue.h"
#include <semaphore.h>

// creates a big string, to sent the job to the Server < Needs free()! >
char* create_job(int argc, char *argv[]) {

    int start_index = 3;
    int end_index = argc - 1;
    
    if (start_index == end_index) {
        char* return_string = (char*)malloc(sizeof(char)*(strlen(argv[start_index]) + 1));
        strcpy(return_string, argv[start_index]);
        return return_string;
    }

    int total_len = 0;
    for (int i = start_index ; i <= end_index ; i++) {
        total_len += strlen(argv[i]);   // for each argument
        total_len += 1; // for the space chars and the '\0' at the end
    }

    char* return_string = (char*)malloc(sizeof(char)*total_len);
    strcpy(return_string, argv[start_index]);
    for (int i = start_index + 1 ; i <= end_index ; i++) {
        strcat(return_string, " ");
        strcat(return_string, argv[i]);
    }

    return return_string;
}

int jobCommander(int argc, char *argv[]) {
    
    // name the arguments
    char* port = argv[2];   // the port of the Server
    char* job = create_job(argc, argv);    // the job to be sent, to the Server
    char* server_name = argv[1];    // the name of the machine

    // set the hint address
    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family = AF_UNSPEC;

    // get the results from the hint address
    struct addrinfo *result;
    getaddrinfo(server_name, port, &hint, &result);

    // Find the actuall address and connect with the server:
    // this is done by tring to create a socket and connect to the server
    // if any of those two fails, it means it's not the proper address
    // so we check the next address from the results that getaddrinfo() gave us
    int commander_fd;
    struct addrinfo *temp_addr_info = result;
    while (temp_addr_info != NULL) {

        // the info of the temp address
        int family = temp_addr_info->ai_family;
        int type = temp_addr_info->ai_socktype;
        int protocol = temp_addr_info->ai_protocol;
        struct sockaddr *address = temp_addr_info->ai_addr;
        socklen_t size = temp_addr_info->ai_addrlen;

        // try creating a socket for this commander
        if ((commander_fd = socket(family, type, protocol)) == -1) {
            temp_addr_info = temp_addr_info->ai_next;
            continue;
        }

        // try connecting to the server with the socket
        if (connect(commander_fd, address, size) == -1) {
            close(commander_fd);
            temp_addr_info = temp_addr_info->ai_next;
            continue;
        }

        // if we arrive here, it means we have found the wanted address
        break;
    }

    // free all the results
    freeaddrinfo(result);

    // send 1 if exit, else send 0
    int exit = -1;
    if (strcmp(argv[3], "exit") == 0) {
        exit = 1;
    }
    send(commander_fd, &exit, sizeof(int), 0);

    // send the number of words of the job, the length of the job and the job to the server
    int total_words = argc - 3;
    send(commander_fd, &total_words, sizeof(int), 0);   // send number of words
    int total_len = strlen(job) + 1;
    send(commander_fd, &total_len, sizeof(int), 0); // send job length
    send(commander_fd, job, strlen(job) + 1, 0);    // send the job
    
    // free the memory for the job
    free(job);

    // read the length of the message and the message
    int len;
    read(commander_fd, &len, sizeof(int));  // read length
    char server_message[len];
    read(commander_fd, &server_message, sizeof(server_message));    // read the message
    printf("%s\n", server_message);

    // check if commander makes an issueJob command, if yes print the message returned when
    // the job is finished
    if (strcmp(argv[3], "issueJob") == 0) {
        
        // read the length of the output and the output of the command
        int output_len;
        read(commander_fd, &output_len, sizeof(int));   // read length

        // If out_put_len = -1 -> it is a stopped job, so we shouldn't try reading it's output
        if (output_len != -1) {
            char output[output_len];
            read(commander_fd, &output, sizeof(char)*output_len); // read message

            // print the output
            printf("%s\n", output);
        }
    }

    // close the commander socket
    close(commander_fd);

    return 0;
}




void main(int argc, char *argv[]) {
    
    // call the jobCommander function
    jobCommander(argc, argv);
}