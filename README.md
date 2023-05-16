# Webserver
## 개요
C/C++를 활용해 NginX의 워커프로세스를 모방한 웹서버를 만들어보며 웹 관련 지식을 습득

## 구상 흐름
![IMG_0311](https://github.com/whaleshade/Webserver/assets/89024499/4f7940e1-6c66-441f-9d0e-6790f696abce)


## 웹서버 관련 지식
웹서버는 전송계층에 해당한다. 전송계층은 데이터가 어떻게 전송될지를 결정하는 계층으로 크게 TCP와 UDP가 있다.

HTTP서버는 TCP연결로 만들어지는데, udp는 전송이 신뢰성이 낮기 때문이다. Apache나 NginX같은 HTTP서버는 모두 TCP위에서 만들어졌다.

![](https://velog.velcdn.com/images/augus-xury/post/baaa01fd-e20e-4e81-9745-f29c7d7e9502/image.jpg)

웹서버는 클라이언트의 요청을 받아 정적 페이지를 응답해주고, 필요한 경우 WAS를 통해 DB를 이용하게 된다.


## RFC
인터넷 기술 표준
IP(Internet Protocol)는 'RFC791', TCP는 'RFC793', Domain Names는 'RFC1035', HTTP(Hypertext Transfer Protocol)는 'RFC2616' 

RFC 1945 : HTTP/1.0 (1996)
RFC 2616 : HTTP/1.1 (1999)
RFC 7540 : HTTP/2 (2015)

RFC 7230부터 7235까지가 HTTP/1.1에 대한 문서다.
1. RFC 7230, HTTP/1.1: Message Syntax and Routing
2. RFC 7231, HTTP/1.1: Semantics and Content
3. RFC 7232, HTTP/1.1: Conditional Requests
4. RFC 7233, HTTP/1.1: Range Requests
5. RFC 7234, HTTP/1.1: Caching
6. RFC 7235, HTTP/1.1: Authentication



## 소켓 프로그래밍

서버 프로그램에서 소켓을 만들면 (socket()) 이것으로 클라이언트의 연결을 감지(랑데뷰 소켓)하게 된다.
클라이언트와의 연결이 이뤄지면 별도의 소켓을 만들어 이것을 통해 통신을 하게 된다.

### 소켓 생성 socket()
TCP연결을 구현하기 위해 소켓 프로그래밍을 진행
```c
int sockfd = socket(int domain, int type, int protocol);
```

첫번째 인자에는 소켓이 생성될 도메인을 넣는다. HTTP 소켓 통신에는 PF_INET으로 진행.

```
PF_LOCAL        Host-internal protocols, formerly called PF_UNIX,
PF_UNIX         Host-internal protocols, deprecated, use PF_LOCAL,
PF_INET         Internet version 4 protocols,
PF_ROUTE        Internal Routing protocol,
PF_KEY          Internal key-management function,
PF_INET6        Internet version 6 protocols,
PF_SYSTEM       System domain,
PF_NDRV         Raw access to network device,
PF_VSOCK        VM Sockets protocols

```


두번째 인자에는 서비스할 타입을 지정해 준다. 주로 TCP나 UDP 중 하나가 사용된다. HTTP 통신에는  TCP에 해당하는 SOCK_STREAM으로 설정한다.
```
SOCK_STREAM		provides sequenced, reliable, two-way connection based byte streams.  An out-of-band data transmission mechanism may be supported.  

SOCK_DGRAM		supports datagrams (connectionless, unreliable messages of a fixed (typically small) maximum length).
 
SOCK_RAW 		provide access to internal network protocols and interfaces. available only to the super-user.
```
>Raw socket?
https://mangkyu.tistory.com/16

> 참고로 SOCK_STREAMD 설명에 나오는 단어인 out-of-band란 데이터에 우선순위를 매겨 보내는 방법이다. 논리적인 스트림에서 벗어나 긴급한 메시지를 보내기 때문에 이런 이름이 붙었다.

세번째 인자에는 통신에 사용하고자 하는 특정한 프로토콜을 넣는다. 서비스 타입에 따른 프로토콜을 특정하는 인자이다. 같은 프로토콜이더라도 데이터 전송 방식에 차이가 있을 수 있다. TCP의 경우 프로토콜이 한 종류이므로 0을 넣어준다.

>
```c
#include <sys/socket.h>
...
...
if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
{
    perror(“cannot create socket”); 
    return 0; 
}
```

### 소켓 바인딩 bind()
transport 주소를 소켓에 할당(바인딩)하는 것
```c
int bind(int sockfd, const struct sockaddr *address, socklen_t address_len);
```

첫번재 인자에는 소켓으로 만들어진 소켓fd을 넣어준다.
두번째 인자에는 서버측 소켓주소 정보에 대한 구조체를 넣어준다. 소켓 주소란 IP주소 + port 번호이다. <netinet/in.h>에 정의된 구조체인 sockaddr_in을 사용한다.

```c
struct sockaddr_in 
{ 
    __uint8_t         sin_len; 
    sa_family_t       sin_family; 
    in_port_t         sin_port; 
    struct in_addr    sin_addr; 
    char              sin_zero[8]; 
};
```

sin_family에는 항상 AF_INET을 설정(Address Family), 
sin_port는 0으로 설정하면 OS가 남는 port 번호를 자동 할당해주는데 클라이언트가 서버 포트를 알아야 접속하므로 고정값을 할당하면 된다.
sin_addr는 소켓 주소를 적는데, 이는 서버컴퓨터의 IP주소를 말한다. 아무거나 사용할 때는 0.0.0.0에 해당하는 INADDR_ANY를 넣는다.

>bind() 두번째 인자는 sockaddr 구조체로 받는 이유?
sockaddr_in은 IP기반 통신에 사용되는 주소를 담는 구조체이다. sockaddr는 어떠한 주소 타입이라도 받겠다는 것이다. '_in'이 internet을 의미한다.

마지막 인자는 소켓주소 구조체의 크기를 넣는데, 이는 sizeof(sockaddr_in)으로 해준다.

```c
#include <sys/socket.h> 
… 
struct sockaddr_in address;
const int PORT = 8080; //Where the clients can reach at
/* htonl converts a long integer (e.g. address) to a network representation */ 
/* htons converts a short integer (e.g. port) to a network representation */ 
memset((char *)&address, 0, sizeof(address)); 
address.sin_family = AF_INET; 
address.sin_addr.s_addr = htonl(INADDR_ANY); 
address.sin_port = htons(PORT); 
if (bind(server_fd,(struct sockaddr *)&address,sizeof(address)) < 0) 
{ 
    perror(“bind failed”); 
    return 0; 
}
```
> 컴퓨터마다 데이터를 저장하는 방식이 다 다르므로 (리틀엔디안과 빅엔디안 등) 일종의 시리얼라이제이션이 필요하다. 네트워크 byte order인 빅엔디안을 따르도록 데이터의 구조를 만들어야 한다.
```
htonl = host to network로 long int 자료형 데이터를 변환
htons = host to network로 short int 자료형 데이터를 변환
ntohl = network to host로 long int 자료형 데이터를 변환
ntohs = network to host로 short int 자료형 데이터를 변환
```

### 연결 대기 listen()
```c
int listen(int sockfd, int backlog);
```
첫번째 인자에는 소켓fd를 넣어준다.
두번째 인자에는 커넥션을 대기할 큐의 크기를 넣어주게 된다. 

### 연결 수락 accept()
```c
int accept(int sockfd, struct sockaddr *restrict address, socklen_t *restrict address_len);
```
첫번째 인자에 생성된 소켓의 fd, 두번째 인자에 주소정보를 넣는다. 세번째에는 주소정보의 크기를 넣는다.
accept에 성공하면 read/write를 해서 통신을 할 수 있는 fd를 리턴해 준다.

### 연결 요청 connect()

```c
int connect(int sockfd, const struct sockaddr *address, socklen_t address_len);
```
첫번째 인자에 클라이언트 쪽에서 만든 소켓fd, 두번째 인자에 서버주소 정보, 세번째 인자에 서버주소 크기를 넣는다.


```c

/** Server side socket programming **/
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
int main(void)
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    //char *msg = "hello from server";
    char *msg = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 200\n\n<h1>Hello</h1><li>good</li>";

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
    for (;;)
    {
        printf("\n========== Waiting for new connection ==========\n\n");
        system("netstat -an | grep 8080");
        printf("-----------------------\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        system("netstat -an | grep 8080");
        printf("-----------------------\n");
        char buffer[30000] = {0};
        valread = read(new_socket, buffer, 30000);
        printf("%s\n", buffer);
        system("netstat -an | grep 8080");
        write(new_socket, msg, strlen(msg));
        printf("------------------message sent-------------------\n");
        close(new_socket);
    }
    return 0;
}

```

```c
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
    char *hello = "Hello from client";
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
    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    valread = read( sock , buffer, 1024);
    printf("%s\n", buffer );
    return 0;
}

```

# time_wait
![](https://velog.velcdn.com/images/augus-xury/post/42d02a7d-76d5-4688-96a0-8c911f9a271f/image.jpeg)
상태 보기 리눅스 커맨드
lsof -i:8080 또는 netstat -an | grep 8080

TIME_WAIT 상태가 늘어나면 서버의 할당 가능할 fd가 고갈되어 커넥션 타임아웃이 발생한다는 얘기...
# HTTP

이제 메시지에 http로 데이터를 주고받으면 인터넷이 된다. 
우리가 사용하는 chrome, safari, firefox등의 인터넷 브라우저는 인터넷 주소에 default로 www.example.com:80 이런식으로 80 포트번호를 붙여준다. 80포트는 IANA에서 설정한 약속으로 http가 80포트로 할당 돼 있다.
https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.txt

>인터넷창에 주소를 적는 행위도 웹브라우저가 대신 해주는 client request다.

------------

## 멀티플렉싱 서버

서버는 수많은 클라이언트들과 소켓 통신을 해야하기 때문에 그에 해당하는 수의 소켓fd를 대량으로 관리해야 한다.

이 때 서버를 블로킹 방식으로 동작하게 만들면, 하나의 클라이언트가 write를 하여 서버가 read를 할 때까지 다른 동작은 하지 않게 되므로 다수의 클라이언트가 기다리게 된다는 문제가 발생한다.

따라서 논블로킹 방식으로 만들어야 하는데, 
1. polling 방식
2. I/O 멀티플렉싱
3. 콜백/시그널 활용

폴링방식은 반복적으로 요청이 왔는지 확인하는 것이다. 스레드가 지속적으로 소켓의 정보를 읽어야 하기 때문에 자원이 많이 소비된다는 단점이 있다.

따라서 이용하게 된 방법이 select를 이용한 I/O 멀티플렉싱인데, 서버 프로세스가 커널에 특정한 이벤트를 기다리게 하고, 원하는 이벤트와 개수가 채워지면 프로세스에 알려주는 방식이다.

생성된 소켓fd를 대량으로 관리하는 시스템콜은 select -> poll -> epoll로 발전해 왔다.
리눅스에서 사용되는 epoll 시스템콜은 맥OS에선 kqueue라고 불린다.

### select를 안 쓰는 이유?
우선 select는 다음과 같이 사용한다.
```
fd_set fds; //fd_set 구조체 타입
FD_ZERO(&fds);
FD_SET(400, &fds);
select(401, &fds, NULL, NULL, NULL);

```
관리하는 fd의 개수에 상관 없이 최대 fd값 + 1을 select의 인자로 설정해줘야 한다.




poll -> epoll
	->kqueue (BSD에서 지원되는 epoll)


------------
### kqueue
```c
#include <sys/time.h>
#include <sys/event.h>
#include <sys/types.h>

/* event queue 생성 */
int kqueue(void);

```

kqueue함수는 kqueue를 사용하기 위한 선언부라고 생각하면 된다. 커널 이벤트(kevent) 큐를 생성하고 해당하는 fd값을 리턴한다. 일반적으로 kqueue에서 관리하는 이벤트 큐는 자식 프로세스가 상속할 수 없다. 에러시 -1을 리턴.

kqueue는 발생한 이벤트들을 차례대로 queue에 담아 한꺼번에 알려주는 역할을 하게 됨.



### kevent

```c
/* event 등록 및 가져오기 */
int kevent( int kq, 							// kqueue() 함수가 반환한 fd
            const struct kevent * changelist,	// 이벤트큐에 등록할 이벤트 구조체
            int nchanges, 						// 등록할 이벤트 구조체의 개수
	        struct kevent * eventlist,			// 이벤트 발생시 들어오는 이벤트 배열
            int nevents, 						// 들어오는 이벤트의 최대값
	        const struct timespec * timeout ); 	// 기다리는 시간, NULL이면 계속 대기
```
커널 이벤트 큐에 이벤트들을 등록하거나 처리가 안 되는 이벤트들을 반환하고자 할 때 사용하는 함수다. 리턴값은 변화가 생긴(eventlist에 들어간) 이벤트의 개수다.
이벤트란 READ, WRITE, ADD 등을 말하는데 이를 큐에 등록한다는 것은 해당하는 변화가 있을때 알려주겠다는 것.

```c
struct kevent {
    uintptr_t ident;        // 이벤트 ident(identifier)
    int16_t   filter;       // 이벤트 필터
    uint16_t  flags;        // 일반적인 kqueue 플래그
    uint32_t  fflags;       // 필터 플래그 값
    intptr_t  data;         // 필터 데이터 값
    void      *udata;       // 사용자 정의 데이터
};
```

#### EV_SET() 
kevent 구조체를 초기화하기 위한 매크로

```c
EV_SET(&kev, ident, filter, flags, fflags, data, udata);
```

>kevent?
kevent는 커널 이벤트로 ident와 커널필터의 쌍 및 옵션 등으로 정의된다.(man page에 정의). ident는 fd값인데 여기서는 accept에서 반환한 fd값을 넣을 예정(클라이언트와 서버가 연결된 이벤트). 커널필터는 어떤 이벤트를 받아올 것인가에 대한 설정 사항.

**헷갈리는 부분: flags랑 fflags에서 블로그 실제 적용 예시랑 man page 설명 인자 위치가 왜 다른가..

>#### flags
EV_ADD : kqueue에 이벤트 추가, 다시 추가하면 기존값을 변경해 들어감.
EV_ENABLE : kevent()가 필터에 의해 트리거된 이벤트를 리턴할지 허용
EV_DISABLE : 필터는 계속 동작하지만, 트리거된 이벤트를 리턴하지 않음.
EV_DELETE : kevent 큐에서 이벤트를 삭제.
EV_ONESHOT : 필터에 해당하는 첫 이벤트를 리턴하고 해당 이벤트는 큐에서 삭제.
EV_CLEAR : 사용자가 이벤트를 받고난 후 이벤트 상태를 리셋. 이벤트의 변화만을 추척하기 좋음.
EV_EOF : 필터 한정 EOF를 확인하기 위한 세팅?? // evnetlist 결과 flag에 비트연산하는듯.. eg) eventList[i].flags & EV_EOF
EV_OOBAND : Out of band data 있을때 알려줌
EV_ERROR : ??

>#### fflags
EVFILT_READ : 읽을 수 있는 상태일때 알려주는 필터 플래그
EVFILT_WRITE : 쓸수 있는 상태를 알려주는 필터 플래그
EVFILT_VNODE :
EVFILT_PROC : 
EVFILT_SIGNAL :
EVFILT_TIMER :


만약 kqueue에  read할 데이터가 들어있는 kevent가 있는지 확인하려면 
```c
struct kevent ev;
EV_SET(&ev, sockfd, EVFILT_READ, EV_ADD, 0, 0, 0);
```
만약 write할 kevent가 있는지 확인하려면
```c
struct kevent ev;
EV_SET(&ev, sockfd, EVFILT_WRITE, EV_ADD, 0, 0, 0);
```

#### kevent 동작 방식

```c
#include <sys/time.h>
#include <sys/event.h>
#include <sys/types.h>

int main() {

[...]

/*=================
 	kqueue 초기화 
==================*/

int nKqueue = kqueue();
int idx = 0;
struct kevent changeList[N]; // N은 관리할 이벤트 개수
struct kevent eventList[N]; // 결과를 담을 kevent 배열


/*=================
 	kEvent 준비 
==================*/

struct kevent ev;

EV_SET(&ev, serverSockFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
changeList[idx] = ev; 

idx++;

[...]


/*=================
  kEvent 커널에 등록 
==================*/

int nEventCount = 0;
nEventCount = kevent(nKqueue, changeList, idx + 1, eventList, N, NULL); //idx + 1 이 등록된 이벤트의 개수가 됨


/*=================
  	kEvent 처리 
==================*/

for(int i = 0; i < N; i++)
{
	//에러가 있을 경우
	if(eventList[i].flags & EV_ERROR)
	{
		if(eventList[i].ident == sserverSockFd)
        	//서버 소켓 에러
        else 
        	//클라이언트 소켓 에러
	}
	
    //Read 이벤트
	if(eventList[i].filter == EVFILT_READ )
	{
		if (eventList[i].ident == serverSockFd)
        	//클라이언트 요청 accept
		[...]
	}
    
    //Write 이벤트
 	if(eventList[i].filter == EVFILT_WRITE)
    {
    	//클라이언트에 response 전송
    	[...]
    }
}

/*=================
  	kqueue 종료 
==================*/
close(nKqueue);

return (0); }

```

------------

### fcntl 사용법

fcntl은 non-blocking을 위해 사용


```c
#include <unistd.h>
#include <fcntl.h>

fcntl(server_socket, F_SETFL, O_NONBLOCK); 
```

첫번째 인자 : 변경하려는 파일의 fd, 여기서는 서버 소켓의 fd
두번째 인자: F_SETFL옵션은 세번째 인자의 파일 상태 속성을 설정하는 옵션. O_APPEND, O_NONBLOCK, O_SYNC, O_DSYNC, O_RSYNC, O_FSYNC, O_ASYNC 등이 있음.
세번째 인자: 우리는 논블로킹 동작을 위해 과제에서 요구하는 대로  O_NONBLOCK을 설정. 


이 상태에서 read, write를 하면 논블로킹 동작이 일어나게 된다.

간단히 이해를 해보자.

예를들어 아래 코드는 키보드에서 버퍼로 입력을 받아 출력을 해주는 코드다.

```c
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define FD_IN 0
#define FD_OUT 1

int main(){
        char buf[32] = {0,};

        //fcntl(FD_IN, F_SETFL, O_NONBLOCK);

        read(FD_IN, buf, sizeof(buf));
		
        write(FD_OUT, "input : ", sizeof("input : "));
        write(FD_OUT, buf, sizeof(buf));
    
        return (0);
}

```
이 상태에서 hello를 입력 해야 input : hello가 출력된다.
하지만 주석을 해제하여 논블로킹으로 만들면 input : 이 출력되면서 프로그램이 종료돼 버린다.


------------

## CGI
>The Common Gateway Interface, or CGI, is a set of standards that define how information is exchanged between the web server and a custom script.

