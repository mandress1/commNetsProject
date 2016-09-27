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

int main(void)
{

	return 0;
}