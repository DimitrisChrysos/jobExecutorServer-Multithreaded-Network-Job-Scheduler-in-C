#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

int main() {
    
    // create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    // check for error with the connection
    if (connection == -1) {
        printf("There was an error making a connection to the remote socket \n\n");
    }

    // send a message to the server
    char hello[] = "Hello from the client";
    send(network_socket, hello, strlen(hello), 0);

    // receive data from the server
    char server_responce[256];
    // recv(network_socket, &server_responce, sizeof(server_responce), 0);
    read(network_socket, &server_responce, sizeof(server_responce));

    // print out the server's responce
    printf("The server sent the data: %s\n", server_responce);

    // and then close the socket
    close(network_socket);

    return 0;
}