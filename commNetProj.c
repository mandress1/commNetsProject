#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SIZE 1024           // buffer size
#define SCHED_PORT 23510    // ScheduleServer port. host: c-lnx001.engr.uiowa.edu
#define WEATH_PORT 23511    // WeatherServer port. host: c-lnx000.engr.uiowa.edu

char buff[SIZE];            // Because why not have global variables
const char* SCHED_NAME = "c-lnx001.engr.uiowa.edu";
const char* WEATH_NAME = "c-lnx000.engr.uiowa.edu";

int main(int argc, char* argv[])
{
    int schedfd;                    // ScheduleServer socket file descriptor
    int weathfd;                    // WeatherServer socket file descriptor
    int nread;
    struct hostent* h;              // Used for getting servers
    struct sockaddr_in schedAddr;   // Socket for ScheduleServer
    struct sockaddr_in weathAddr;   // Socket for WeatherServer
    char* hawkid;
    char* msgToSend;

    if(argc < 2)
    {
        perror("Error: not enough arguments supplied.\nUsage: `./<executable_name>.exe <hawkid>\n");
        exit(-1);
    }

    hawkid = argv[1];

    /* setting udp first ScheduleServer */
    printf("Setting up socket for UDP connection\n");
    if((schedfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error: could not open socket for ScheduleServer\n");
        exit(1);
    }
    printf("Socket opened for ScheduleServer\n");
    h = gethostbyname(SCHED_NAME);                              // Getting ScheduleServer address
    schedAddr.sin_family = AF_INET;                             // IPV4 woot woot
    schedAddr.sin_port = htons(SCHED_PORT);                     // Look up top for it
    bcopy(h->h_addr, (char*)&schedAddr.sin_addr, h->h_length);  // Telling socket address to listen for
    printf("ScheduleServer socket set up\n");

    /* setting up WeatherServer */
    printf("Setting up socket for TCP connection\n");
    if((weathfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: could not open socket fo WeatherServer\n");
        exit(2);
    }
    printf("Socket opened for WeatherServer\n");
    h = gethostbyname(WEATH_NAME);                              // Getting WeatherServer address
    weathAddr.sin_family = AF_INET;                             // IPV4
    weathAddr.sin_port = htons(WEATH_PORT);                     // at the top
    bcopy(h->h_addr, (char*)&weathAddr.sin_addr, h->h_length);  // copy address
    printf("WeatherServer socket set up\nTrying to connect\n");
    if(connect(weathfd, (struct sockaddr*)&weathAddr, sizeof(weathAddr)) < 0)
    {
        perror("Couldn't set up connection to WeatherServer\n");
        exit(3);
    }
    printf("Connection made to WeatherServer\n");

    /* sending messages */
    printf("Sending mandress Iowa to ScheduleServer\n");
    if(sendto(schedfd, "mandress Iowa", sizeof("mandress Iowa"), 0, (struct sockaddr*)&schedAddr, sizeof(schedAddr)) < 0)
    {
        perror("Error: failed to send message to ScheduleServer\n");
        exit(4);
    }
    printf("Message sent\nGetting response\n");
    socklen_t addrlen = sizeof(schedAddr);
    if((nread = recvfrom(schedfd, buff, SIZE, 0, (struct sockaddr*)&schedAddr, &addrlen)) < 0)
    {
        perror("Error: failed to get response from server\n");
        exit(5);
    }
    buff[nread] = '\0';
        printf("response: %s\n", buff);

    printf("Sending mandress to WeatherServer\n");
    write(weathfd, "mandress", sizeof("mandress"));
    printf("Messgae sent\n");
    nread = read(weathfd, buff, SIZE);
    buff[nread] = '\0';
    printf("WeatherServer response: %s\n", buff);


    close(weathfd);
    close(schedfd);


    return 0;
}