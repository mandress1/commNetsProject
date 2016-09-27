#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SCHED_PORT 23510 // ScheduleServer port. host: c-lnx001.engr.uiowa.edu
#define WEATH_PORT 23511 // WeatherServer port. host: c-lnx000.engr.uiowa.edu

int main(int argc, char* argv[])
{
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

    return 0;
}