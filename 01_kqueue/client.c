/** Client side socket programming **/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080

int main(void)
{
    int sock = 0; long valread;
    struct sockaddr_in serv_addr;
    char *hello = "GET HTTP/1.1 I_Want_server_response";//HTTP 방식으로 담아줘야
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses text form to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    while (1)
    {
        send(sock, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        valread = read(sock, buffer, 1024);
        printf("%s\n", buffer);
        sleep(1);
    }
    return 0;
}
