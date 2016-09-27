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

int main(int argc, char* argv[])
{
    int sockfd, client_sockfd;
    int nread, len;         // Have these because example on slides
    time_t time;            // Same with this one too
    struct sockaddr_in serv_addr, client_addr; // These too
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
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Unable to open socket for ScheduleServer\n");
        return 2;
    }
    printf("Socket opened\n");

    return 0;
}