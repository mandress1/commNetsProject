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
    struct sockaddr_in my_addr;     // This computers address
    struct sockaddr_in serv_addr;   // Servers address
    struct hostent* hp;             // Getting from dns 
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

    /* Setting up socket for this computer */
    printf("Trying to open socket on this computer\n");
    if ((mysockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Unable to open socket for ScheduleServer\n");
        return 2;
    }
    printf("Socket opened\n");

    // Following this example https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html
    memset((char*)&my_addr, 0, sizeof(my_addr));    // Setting address to 0s to be initialized
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
    /* Socket opened and bound to connect to server */

    /* Opening socket to ScheduleServer now */
    memset((char*)&serv_addr, 0, sizeof(serv_addr));    // Setting server address to 0  to be setup later
    serv_addr.sin_family = AF_INET;                     // IPV4
    serv_addr.sin_port = SCHED_PORT;                    // Port ScheduleServer Listening on 23510

    hp = gethostbyname(SCHED_NAME);                     // Getting host by given name for ScheduleServer
    if (!hp)
    {
        printf("Error: couldn't get server address\n");
        return 4;
    }
    printf("Host found by name. Setting up socket...\n");
    memcpy((void*)&serv_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    printf("Server socket settup. Wooo!\n");

    return 0;
}