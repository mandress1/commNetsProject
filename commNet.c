#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SCHED_PORT 23510    // ScheduleServer port. host: c-lnx001.engr.uiowa.edu
#define WEATH_PORT 23511    // WeatherServer port. host: c-lnx000.engr.uiowa.edu
#define SIZE 1024

char buff[SIZE];            // Because why not have global variables
const char* SCHED_NAME = "c-lnx001.engr.uiowa.edu";
const char* WEATH_NAME = "c-lnx000.engr.uiowa.edu";

int main(int argc, char* argv[])
{
    int mysockfd;
    int nread, len;         // Have these because example on slides
    time_t time;            // Same with this one too
    struct sockaddr_in my_addr; 
    char* hawkid;

    if (argc == 2)
    {
        hawkid = argv[1];
        printf("Hawkid: %s\n", hawkid);
    }
    else
    {
        printf("Error: Incorrect number of arguments supplied\n");
        return -1;
    }

    printf("Trying to open ScheduleServer\n");
    if ((mysockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Unable to open socket for ScheduleServer\n");
        return 2;
    }
    printf("Socket opened\n");

    // Following this example https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html
    memset((char*)&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;                   // Good old fashioned IPV4
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    // Telling the OS to grab the address. htonl converts long int into network representation
    my_addr.sin_port = htons(0);                    // Telling OS to pick an open port. htons converts short int to network represntation

    //testing to maa=ke sure port is working
    printf("Attempting to bind socket on this machine...\n");
    if (bind(mysockfd, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0)
    {
        printf("Error: unable to bind failed\n");
        return 3;
    }
    printf("Bind successful\n");

    return 0;
}