/**********************************************************************************************************
 *  How to compile:                                                                                       *
 *          While in the same directory as the c file, type this command: gcc commNetProj.c -o client.exe *
 *                                                                                                        *
 *                                                                                                        *
 *  How to run:                                                                                           *
 *          While in the same directory as the executable, this command: ./client.exe <hawkid>            *
 *          example: ./client.exe mandress                                                                *
 *                                                                                                        *
 *  Programmer:                                                                                           *
 *          Name: Matt Andress                                                                            *
 *          ID:   00797286                                                                                *  
 **********************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define CONF_SIZE       14      // Size of big 10 conference
#define SCHED_PORT      23510   // ScheduleServer port
#define WEATH_PORT      23511   // WeatherServer port
#define IN_BUFF_SIZE    16384   // Buffer size for reading responses from server
#define OUT_BUFF_SIZE   256     // Buffer size for sending output to server


const char* SCHED_NAME = "c-lnx001.engr.uiowa.edu";     // ScheduleServer host name
const char* WEATH_NAME = "c-lnx000.engr.uiowa.edu";     // WeatherServer host name
const char* SCHOOLS[] = {"Michigan","Indiana","Illinois","Maryland","Ohio_State","Northwestern","Iowa","Purdue","Michigan_State","Nebraska","Wisconsin","Minnesota","Rutgers","Penn_State"};


int min(int one, int two);
int isValidSchoolName(const char* schoolName, const char* validSchools[], int confSize);
int setUpConnection(struct sockaddr_in* toSetUp, int mode, const char* hostName, int hostPort);
char* remNull(char* str);

int main(int argc, char* argv[])
{
    int schedfd;                                                // ScheduleServer socket file descriptor
    int weathfd;                                                // WeatherServer socket file descriptor
    int nread;                                                  // Number of bytes read from servers
    struct sockaddr_in schedAddr;                               // Socket for ScheduleServer
    struct sockaddr_in weathAddr;                               // Socket for WeatherServer
    socklen_t schedLen;                                         // Size of ScheduleServer socket struct
    char* hawkid;
    char* respBuff = (char*)malloc(IN_BUFF_SIZE*sizeof(char));  // Buffer for getting responses from server
    char* outBuff = (char*)malloc(OUT_BUFF_SIZE*sizeof(char));  // Buffer for outputing to servers
    char* usrInput = (char*)malloc(OUT_BUFF_SIZE*sizeof(char)); // Buffer for getting input from user

    if(argc < 2)
    {
        perror("Error: not enough arguments supplied.\nUsage: `./<executable_name>.exe <hawkid>\n");
        exit(1);
    }

    hawkid = argv[1];


    printf("Setting up socket for UDP connection\n");
    schedfd = setUpConnection(&schedAddr, SOCK_DGRAM, SCHED_NAME, SCHED_PORT);
    schedLen = sizeof(schedAddr);
    printf("ScheduleServer socket set up\n");

    
    printf("Setting up socket for TCP connection\n");
    weathfd = setUpConnection(&weathAddr, SOCK_STREAM, WEATH_NAME, WEATH_PORT);
    printf("Connection made to WeatherServer\n");

    write(weathfd, remNull(hawkid), sizeof(remNull(hawkid)));
    printf("'%s' sent to WeatherServer\n", argv[1]);
    nread = read(weathfd, respBuff, IN_BUFF_SIZE);
    respBuff[nread] = '\0';
    printf("WeatherServer says: '%s'\n", respBuff);
    

    // Main loop for program.
    do
    {
        /* Reseting buffers for next input */
        bzero(usrInput, OUT_BUFF_SIZE);                         
        bzero(respBuff, IN_BUFF_SIZE);
        bzero(outBuff, OUT_BUFF_SIZE);

        printf("Enter a School name for weather forcast or \"quit\" to exit\nEntry: ");
        scanf("%s", usrInput);

        if(isValidSchoolName(usrInput, SCHOOLS, CONF_SIZE))                 // User enters valid school
        {
            // send '<hawkid> <schoolName>' to ScheduleServer
            int i, j;
            int idLen = strlen(hawkid);
            int inLen = strlen(usrInput);
            int totLen = idLen + inLen + 1;

            // Peraring output
            for(i = 0;i < idLen;i++)
            {
                outBuff[i] = hawkid[i];
            }
            outBuff[idLen] = ' ';
            for(i = 0;i < inLen;i++)
            {
                outBuff[idLen + i + 1] = usrInput[i];
            }

            // Sending output to SceduleServer
            sendto(schedfd, outBuff, totLen, 0, (struct sockaddr*)&schedAddr, sizeof(schedAddr));
            outBuff[totLen] = '\0';
            printf("'%s' sent to ScheduleServer\n", outBuff);

            // get <away>@<home> <3 letter code>
            nread = recvfrom(schedfd, respBuff, IN_BUFF_SIZE, 0, (struct sockaddr*)&schedAddr, &schedLen);
            respBuff[nread] = '\0';
            printf("ScheduleServer says: '%s\n", respBuff);

            // Preping output to WeatherServer
            for(i = strlen(respBuff)- 3,j = 0;i < strlen(respBuff);i++, j++)
            {
                outBuff[j] = respBuff[i];
            }
            
            // send 3 letter code to WeatherServer
            write(weathfd, outBuff, j);
            outBuff[j] = '\0';
            printf("'%s' sent to WeatherServer\n", outBuff);
            sleep(1);       // Sleep cuz why not 

            // print weather server response
            nread = read(weathfd, respBuff, IN_BUFF_SIZE);
            respBuff[nread] = '\0';
            printf("WeatherServer says: %s\n", respBuff);
            //printf(" \n");
        }
        else if(strncmp(usrInput, "quit", min(strlen(usrInput), strlen("quit"))) == 0)      // User decides to quit
        {
            // send quit to WeatherServer
            write(weathfd, remNull(usrInput), sizeof(remNull(usrInput)));
            printf("'%s' sent to WeatherServer\n", usrInput);

            // tell user goodby
            nread = read(weathfd, respBuff, IN_BUFF_SIZE);
            respBuff[nread] = '\0';

            printf("WeatherServer says: '%s'\nNow exiting the program\n", respBuff);
        }
        else        // User is doesn't enter a valid choice
        {
            printf("Error: input nor recognized.\nAll spaces must be underscores ('_') and school names must be properlly capitalized\n");
        }
        
    }while(strncmp(usrInput, "quit", min(strlen(usrInput), strlen("quit"))));
    
    // Freeing allocated memory
    free(usrInput);
    free(respBuff);
    free(outBuff);

    close(weathfd);
    close(schedfd);
    return 0;
}

/*********************************************************************
 *  Description:                                                     *
 *      This function handles setting up sockets for both            *
 *      TCP and UDP.                                                 *
 *                                                                   *
 *  Arguments:                                                       *
 *      struct sockaddr_in* toSetUp:                                 *
 *                  pointer to socket to be set up                   *
 *      int mode:                                                    *
 *                  Mode for socket to be sut up as DGRAM or STREAM  *
 *      const char* hostName:                                        *
 *                  Name of hose to connect socket to                *
 *      int hostPort:                                                *
 *                  Port that the host is listening on               *
 *                                                                   *
 *  Return:                                                          *
 *      int fd:                                                      *
 *                  file descriptor returned from setting up socket  *
 *********************************************************************/
int setUpConnection(struct sockaddr_in* toSetUp, int mode, const char* hostName, int hostPort)
{
    int fd = 0;
    struct hostent* remHost;

    if(mode == SOCK_DGRAM)
    {
        if((fd = socket(AF_INET, mode, 0)) < 0)
        {
            perror("Error: unable to open socket for UDP communication\n");
            exit(2);
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
            exit(2);
        }

        remHost = gethostbyname(hostName);
        toSetUp->sin_family = AF_INET;
        toSetUp->sin_port = htons(hostPort);
        bcopy(remHost->h_addr, (char*)&(toSetUp->sin_addr), remHost->h_length);

        if(connect(fd, (struct sockaddr*)toSetUp, sizeof(*toSetUp)) < 0)
        {
            perror("Unable to establish TCP connection with host\n");
            exit(3);
        }
    }

    return fd;
}

/*********************************************************************
 *  Desctiption:                                                     *
 *      This function checks to see if the input given by the user   *
 *      checks to see if it as a valid Big 10 school.                *
 *                                                                   *
 *  Arguments:                                                       *
 *      const char* schoolName:                                      *
 *              User's input to be validated                         *
 *      const char* validSchools[]:                                  *
 *              An array of the valid Big 10 school names            *
 *      int confSize:                                                *
 *              Number of teams in the conference                    *
 *                                                                   *
 *  Returns:                                                         *
 *      1 if the user entered a valid school name                    *
 *      0 otherwise                                                  *
 *********************************************************************/
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

/*********************************************************************
 *  Desctription:                                                    *
 *      This function compares the two integers and returns the      *
 *      lowest value of the two                                      *
 *                                                                   *
 *  Arguments:                                                       *
 *      int one:                                                     *
 *          The first value to be compared                           *
 *      int two:                                                     *
 *                                                                   *
 *  Return:                                                          *
 *      int: THe smaller of the two values                           *
 *********************************************************************/
int min(int one, int two)
{
    return (one < two)?(one):(two);
}

/*********************************************************************
 * Description:                                                      *
 *      This function takes in a string and returns a string without *
 *      the null characer at the end                                 *
 *                                                                   *
 *  Arguments:                                                       *
 *      char* str:                                                   *
 *              String to bo copied without the null character       *
 *                                                                   *
 *  Return:                                                          *
 *      String without the null character at the end                 *
 *********************************************************************/
char* remNull(char* str)
{
    int ogLen = sizeof(str)/sizeof(str[0]), i;
    char* newStr = (char*)malloc(ogLen*sizeof(char));

    for(i = 0;i < ogLen;i++)
    {
        newStr[i] = str[i];
    }

    return newStr;  
}