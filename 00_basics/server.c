// Server side
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

#define PORT 8080

int servSocket, connectSocket;
long valread;

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // char *hello = "Hello from server";
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    // Creating socket file descriptor
    if ((servSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    memset(address.sin_zero, '\0', sizeof address.sin_zero);


    if (bind(servSocket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
    if (listen(servSocket, 10) < 0)
    {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((connectSocket = accept(servSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char buffer[30000] = {0};
        valread = read( connectSocket , buffer, 30000);
        printf("%s\n",buffer );
        write(connectSocket , hello , strlen(hello));
        printf("------------------Hello message sent-------------------\n");
        close(connectSocket);
    }

	printf("hihi\n");
    return 0;
}
