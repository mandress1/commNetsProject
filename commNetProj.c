#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define CONF_SIZE 14
#define OUT_BUFF_SIZE 256
#define IN_BUFF_SIZE 16384     // buffer size
#define SCHED_PORT 23510    // ScheduleServer port. host: c-lnx001.engr.uiowa.edu
#define WEATH_PORT 23511    // WeatherServer port. host: c-lnx000.engr.uiowa.edu

const char* SCHED_NAME = "c-lnx001.engr.uiowa.edu";
const char* WEATH_NAME = "c-lnx000.engr.uiowa.edu";
const char* SCHOOLS[] = {"Michigan","Indiana","Illinois","Maryland","Ohio_State","Northwestern","Iowa","Purdue","Michigan_State","Nebraska","Wisconsin","Minnesota","Rutgers","Penn_State"};



int min(int one, int two);
int isValidSchoolName(const char* schoolName, const char* validSchools[], int confSize);
int setUpConnection(struct sockaddr_in* toSetUp, int mode, const char* hostName, int hostPort);
char* remNull(char* str);

int main(int argc, char* argv[])
{
    int schedfd;                    // ScheduleServer socket file descriptor
    int weathfd;                    // WeatherServer socket file descriptor
    int nread;
    struct sockaddr_in schedAddr;   // Socket for ScheduleServer
    struct sockaddr_in weathAddr;   // Socket for WeatherServer
    socklen_t schedLen;
    char* hawkid;
    char* respBuff = (char*)malloc(IN_BUFF_SIZE*sizeof(char));
    char* outBuff = (char*)malloc(OUT_BUFF_SIZE*sizeof(char));
    char* usrInput = (char*)malloc(OUT_BUFF_SIZE*sizeof(char));

    if(argc < 2)
    {
        perror("Error: not enough arguments supplied.\nUsage: `./<executable_name>.exe <hawkid>\n");
        exit(-1);
    }

    hawkid = argv[1];

    /* setting udp first ScheduleServer */
    printf("Setting up socket for UDP connection\n");
    schedfd = setUpConnection(&schedAddr, SOCK_DGRAM, SCHED_NAME, SCHED_PORT);
    schedLen = sizeof(schedAddr);
    printf("ScheduleServer socket set up\n");

    /* setting up WeatherServer */
    printf("Setting up socket for TCP connection\n");
    weathfd = setUpConnection(&weathAddr, SOCK_STREAM, WEATH_NAME, WEATH_PORT);
    printf("Connection made to WeatherServer\n");

    write(weathfd, remNull(hawkid), sizeof(hawkid));
    printf("'%s' sent to WeatherServer\n", argv[1]);
    nread = read(weathfd, respBuff, IN_BUFF_SIZE);
    respBuff[nread] = '\0';
    printf("WeatherServer says: '%s'\n", respBuff);

    
    do
    {
        printf("Enter a School name for weather forcast or \"quit\" to exit\nEntry: ");
        scanf("%s", usrInput);

        if(isValidSchoolName(usrInput, SCHOOLS, CONF_SIZE))
        {
            // send <hawkid> <schoolName> to ScheduleServer
            int i, j;
            int idLen = strlen(hawkid);
            int inLen = strlen(usrInput);
            int totLen = idLen + inLen + 1;
            for(i = 0;i < idLen;i++)
            {
                outBuff[i] = hawkid[i];
            }
            printf("wow\n");
            outBuff[idLen] = ' ';
            for(i = 0;i < inLen;i++)
            {
                outBuff[idLen + i + 1] = usrInput[i];
            }
            sendto(schedfd, outBuff, totLen, 0, (struct sockaddr*)&schedAddr, sizeof(schedAddr));
            outBuff[totLen] = '\0';
            printf("'%s' sent to ScheduleServer\n", outBuff);

            // get <away>@<home> <3 letter code>
            nread = recvfrom(schedfd, respBuff, IN_BUFF_SIZE, 0, (struct sockaddr*)&schedAddr, &schedLen);
            respBuff[nread] = '\0';
            printf("ScheduleServer says: '%s\n", respBuff);
            for(i = strlen(respBuff)- 3,j = 0;i < strlen(respBuff);i++, j++)
            {
                outBuff[j] = respBuff[i];
            }
            
            // send <3 letter code> to WeatherServer
            write(weathfd, outBuff, j);
            outBuff[j] = '\0';
            printf("'%s' sent to WeatherServer\n", outBuff);

            // print weather server response
            nread = read(weathfd, respBuff, IN_BUFF_SIZE);
            respBuff[nread] = '\0';
            printf("WeatherServer says: %s\n", respBuff);
        }
        else if(strncmp(usrInput, "quit", min(strlen(usrInput), strlen("quit"))) == 0)
        {
            // send quit to WeatherServer
            write(weathfd, remNull(usrInput), sizeof(remNull(usrInput)));
            printf("'%s' sent to WeatherServer\n", usrInput);
            // tell user goodby
            nread = read(weathfd, respBuff, IN_BUFF_SIZE);
            respBuff[nread] = '\0';

            printf("WeatherServer says: '%s'\nNow exiting the program\n", respBuff);
        }
        else
        {
            printf("Error: input nor recognized.\nAll spaces must be underscores ('_') and school names must be correctly capitalized\n");
        }
    }while(strncmp(usrInput, "quit", min(strlen(usrInput), strlen("quit"))));
    


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

int isValidSchoolName(const char* schoolName, const char* validSchools[], int confSize)
{
    int i, nToCmp;
    for(i = 0;i < confSize;i++)
    {
        nToCmp = min(strlen(schoolName), strlen(validSchools[i]));
        if(strncmp(schoolName, validSchools[i], nToCmp) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int min(int one, int two)
{
    return (one < two)?(one):(two);
}

char* remNull(char* str)
{
    int ogLen = strlen(str), i;

    if(str[ogLen] != '\0')
    {
        char* newStr = (char*)malloc(ogLen*sizeof(char));

        for(i = 0;i < ogLen;i++)
        {
            newStr[i] = str[i];
        }

        return newStr;  
    }
    else
    {
        return str;
    }
}