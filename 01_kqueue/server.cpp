
/** Server side socket programming **/
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>  // ㅊㅏ이?
// #include <arpa/inet.h> //
#include <string.h>
#include <fcntl.h>



#include <iostream>
#include <string>
#include <vector>

//kqueue
#include <sys/time.h>
#include <sys/event.h>
#include <sys/types.h>


#define PORT 8080
int main(void)
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // char *msg = "hello from server";
    //char *msg = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 100\n\nHello world!";
    char *msg = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 200\n\n<h1>Hello</h1><li>good time</li>";

    // Socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {

        perror("In socket");
        exit(EXIT_FAILURE);
    }

    // Socket resue address option
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    //FOR NON BLOCKING?
    // fcntl(server_fd, F_SETFL, O_NONBLOCK);


    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    /* KQUEUE */
    int kq = kqueue();
    if (kq == -1 )
    {
        perror("In kqueue");
        exit(EXIT_FAILURE);
    }

    std::vector<struct kevent> change_list; // kevent vector for changelist
    struct kevent event_list[8];       // kevent array for eventlist


    struct kevent temp_event;

    /* 서버 소켓에 대해 읽는 이벤트 등록 : 클라가 요청할 때 */
    //  EV_SET(&event, ident, filter, flags, fflags, data, udata);
    EV_SET(&temp_event, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    change_list.push_back(temp_event);

    int new_events;
    struct kevent *curr_event;
    for (;;)
    {
        std::cout << "\n++++++++++++++++++-========START OF WHILE ROOP=========-++++++++++++++++++\n" << std::endl;
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);//SENSE NEW_EVENTS(연결 감지)
        if (new_events == -1)
        {
            perror("In kqueue");
            exit(EXIT_FAILURE);
        }

        std::cout << "new_events sensed : " << new_events << std::endl; //check new_events
        //event_list에 이벤트 담겼으므로 change_list는 비워줌
        change_list.clear(); // clear change_list for new changes

        printf("\n========== Waiting for new connection ==========\n\n");
        system("netstat -an | grep 8080");
        printf("-----------------------\n");
        for (int i = 0; i < new_events; i++)
        {
            std::cout << "xxxxxxxxx DEALING WITH KEVENTS xxxxxxxxxxxxxxxx" << std::endl;
            std::cout << "              count : " << i << std::endl;
            curr_event = &event_list[i];
            if (curr_event->flags & EV_ERROR)
            {
                if (curr_event->ident == server_fd)
                {
                    perror("server error : ");
                    exit(1);
                }
                else
                {
                    perror("client error : ");
                    close(curr_event->ident); //이때의 ident는?
                }
            }

            if (curr_event->filter == EVFILT_READ)
            {
                /* server_fd의 읽기 이벤트 일때 */
                if (curr_event->ident == server_fd)
                {
                    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
                    {
                        perror("In accept");
                        exit(EXIT_FAILURE);
                    }
                    std::cout << "socket accepted : client : " << new_socket << std::endl;

                    fcntl(new_socket, F_SETFL, O_NONBLOCK);

                    /* new_socket에 대해 읽는 이벤트 감지 등록 */
                    EV_SET(&temp_event, new_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    change_list.push_back(temp_event);
                    /* new_socket에 대해 쓰는 이벤트 감지 등록 */
                    EV_SET(&temp_event, new_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    change_list.push_back(temp_event);

                }

                /* 연결된 소켓의 읽기 이벤트 일때 */
                if (curr_event->ident == new_socket)
                {
                    // system("netstat -an | grep 8080");
                    printf("-----------------------\n");
                    char buffer[30000] = {0};
                    //while()
                        valread = read(new_socket, buffer, 30000);
                    std::cout << "From clinet : " << buffer << std::endl;
                    system("netstat -an | grep 8080");
                }
            }

            if (curr_event->filter == EVFILT_WRITE)
            {
                printf("------------------message sending-------------------\n");
                std::cout << "server is sending a msg" << std::endl;
                write(new_socket, msg, strlen(msg));
                printf("------------------message sent-------------------\n");
                // close(new_socket);
            }

            std::cout << "\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n" << std::endl;
        }
        std::cout << "\n++++++++++++++++++-========END OF WHILE ROOP=========-++++++++++++++++++\n" << std::endl;
    }
    return 0;
}
