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
    struct hostent* h;              // Used for getting servers
    struct sockaddr_in schedAddr;   // Socket for ScheduleServer
    struct sockaddr_in weathAddr;   // Socket for WeatherServer
    char* hawkid;
    char* msgToSend;

    if (argc < 2)
    {
        perror("Error: not enough arguments supplied.\nUsage: `./<executable_name>.exe <hawkid>\n");
        exit(-1);
    }

    hawkid = argv[1];

    /* setting udp first ScheduleServer */
    printf("Setting up socket for UDP connection\n");
    if ((schedfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error: could not open socket for ScheduleServer\n");
        exit(1);
    }
    printf("Socket opened for ScheduleServer\n");
    h = gethostbyname(SCHED_NAME);                              // Getting schedule server address
    schedAddr.sin_family = AF_INET;                             // IPV4 woot woot
    schedAddr.sin_port = htons(SCHED_PORT);                     // Look up top for it
    bcopy(h->h_addr, (char*)&schedAddr.sin_addr, h->h_length);  // Telling socket address to listen for
    printf("ScheduleServer socket set up\n");

    

    return 0;
}