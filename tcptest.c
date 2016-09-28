#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define SCHED_PORT 23510    // ScheduleServer port. host: c-lnx001.engr.uiowa.edu
#define WEATH_PORT 23511    // WeatherServer port. host: c-lnx000.engr.uiowa.edu
#define SIZE 1024

char buff[SIZE];            // Because why not have global variables
const char* SCHED_NAME = "c-lnx001.engr.uiowa.edu";
const char* WEATH_NAME = "c-lnx000.engr.uiowa.edu";

int main(void)
{
    int sockfd;
    int nread;
    struct sockaddr_in servAddr;
    struct hostent* h;
    char* tstmsg = "mandress";

    printf("Trying to get socket\n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error: couldn't open socket\n");
        exit(1);
    }
    printf("Socket opened!\n");

    printf("Setting up server shit\n");
    h = gethostbyname(WEATH_NAME);
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(WEATH_PORT);
    bcopy(h->h_addr, (char*)&servAddr.sin_addr, h->h_length);

    printf("Attempting to connect...\n");
    if(connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
        printf("Error: enable to establish connection\n");
        exit(2);
    }
    printf("Connection established\n");

    printf("Attemping to send data: %s\n", tstmsg);
    write(sockfd, tstmsg, sizeof(tstmsg));
    printf("Message sent\nAttempting to get response\n");
    nread = read(sockfd, buff, SIZE);
    buff[nread] = '\0';
    printf("Response: %s\n", buff);

    close(sockfd);
    return 0;
}