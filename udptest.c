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
    int sockfd;
    int nread;
    struct sockaddr_in servAddr;
    struct hostent *h;
    char* tstmsg = "mandress Iowa";

    printf("Trying to get socket\n");
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("Error: unable to obtain socket\n");
        exit(1);
    }
    printf("Socket opened\n");

    printf("Setting up server shit\n");
    h = gethostbyname(SCHED_NAME);
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SCHED_PORT);
    bcopy(h->h_addr, (char*)&servAddr.sin_addr, h->h_length);

    printf("Server connection should be set up. Attempting to communicate\n");
    printf("Sending: %s\n", tstmsg);
    if (sendto(sockfd, tstmsg, sizeof(tstmsg), 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
        printf("Error: message failed to send\n");
        exit(2);
    }
    printf("Message sent\n");

    printf("Attempting to get response\n");
    socklen_t addrlen = sizeof(servAddr);
    if((nread = recvfrom(sockfd, buff, SIZE, 0, (struct sockaddr*)&servAddr, &addrlen)) < 0)
    {
        printf("Error: message recieve failed\n");
        exit(3);
    }
    buff[nread] = '\0';

    printf("Response: %s\n", buff);

    printf("Sending: %s\n", tstmsg);
    if (sendto(sockfd, tstmsg, sizeof(tstmsg), 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
        printf("Error: message failed to send\n");
        exit(2);
    }
    printf("Message sent\n");

    printf("Attempting to get response\n");
    if((nread = recvfrom(sockfd, buff, SIZE, 0, (struct sockaddr*)&servAddr, &addrlen)) < 0)
    {
        printf("Error: message recieve failed\n");
        exit(3);
    }
    buff[nread] = '\0';

    printf("Response: %s\n", buff);

    close(sockfd);

    return 0;
}