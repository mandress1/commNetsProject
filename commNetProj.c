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

const char* SCHED_NAME = "c-lnx001.engr.uiowa.edu";
const char* WEATH_NAME = "c-lnx000.engr.uiowa.edu";

int isValidSchoolName(const char* schoolName);
int setUpConnection(struct sockaddr_in* toSetUp, int mode, const char* hostName, int hostPort);

int main(int argc, char* argv[])
{
    int schedfd;                    // ScheduleServer socket file descriptor
    int weathfd;                    // WeatherServer socket file descriptor
    int nread;
    struct sockaddr_in schedAddr;   // Socket for ScheduleServer
    struct sockaddr_in weathAddr;   // Socket for WeatherServer
    char* hawkid;
    char* usrInput;
    char* msgToSend;
    char* respBuff = (char*)malloc(SIZE*sizeof(char));

    if(argc < 2)
    {
        perror("Error: not enough arguments supplied.\nUsage: `./<executable_name>.exe <hawkid>\n");
        exit(-1);
    }

    hawkid = argv[1];

    /* setting udp first ScheduleServer */
    printf("Setting up socket for UDP connection\n");
    schedfd = setUpConnection(&schedAddr, SOCK_DGRAM, SCHED_NAME, SCHED_PORT);
    printf("ScheduleServer socket set up\n");

    /* setting up WeatherServer */
    printf("Setting up socket for TCP connection\n");
    weathfd = setUpConnection(&weathAddr, SOCK_STREAM, WEATH_NAME, WEATH_PORT);
    printf("Connection made to WeatherServer\n");

    do
    {
        printf("Enter a School name for weather forcast or \"quit\" to exit\nEntry: ");

    }while(strncmp(usrInput, "quit", strlen("quit")));

    close(weathfd);
    close(schedfd);
    return 0;
}

int setUpConnection(struct sockaddr_in* toSetUp, int mode, const char* hostName, int hostPort)
{
    int fd = 0;
    struct hostent* remHost;

    if(mode == SOCK_DGRAM)
    {
        if((fd = socket(AF_INET, mode, 0)) < 0)
        {
            perror("Error: unable to open socket for UDP communication\n");
            exit(1);
        }

        remHost = gethostbyname(hostName);
        toSetUp->sin_family = AF_INET;
        toSetUp->sin_port = htons(hostPort);
        bcopy(remHost->h_addr, (char*)&(toSetUp->sin_addr), remHost->h_length);
    }
    else
    {
        if((fd = socket(AF_INET, mode, 0)) < 0)
        {
            perror("Error: unable to open socket for TCP communication\n");
            exit(1);
        }

        remHost = gethostbyname(hostName);
        toSetUp->sin_family = AF_INET;
        toSetUp->sin_port = htons(hostPort);
        bcopy(remHost->h_addr, (char*)&(toSetUp->sin_addr), remHost->h_length);

        if(connect(fd, (struct sockaddr*)toSetUp, sizeof(*toSetUp)) < 0)
        {
            perror("Unable to establish TCP connection with host\n");
            exit(2);
        }
    }

    return fd;
}

int isValidSchoolName(const char* schoolName)
{
    
}