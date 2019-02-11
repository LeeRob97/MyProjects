//LEE ROBERTSON | CSCE 3530 | CLIENT_HW3

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
 
#define SERVER "129.120.151.94" //IP address of server

/* 
    THIS STRUCT IS USED FOR CONTAINING THE CLIENTS 
    DATA FOR THE SERVER TO FILL.
*/
struct ipvar {
    char ip[20];
    int timeout;
    int tid;
    char myip[20];
};
 
int main(int argc, char **argv){
    struct sockaddr_in serv_addr; //server address
    int s, slen=sizeof(serv_addr), portno; //s - socket, slen (length the of the server), portno - port number
    srand(time(0)); // randomize our transaction ID's
    int tID = rand() % 500 + 1;
    size_t bytessent = 0, bytesread = 0; //flags used to determine if the send and receive are successful
    struct ipvar *temp = 0; //address our ip variable struct

    temp = calloc(sizeof(struct ipvar), 1); //allocate memory for our struct

    system("clear"); //clear screen

    /* CREATE OUR SOCKET */
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        printf("**** SOCKET CREATE ERROR ***\n\n");
        return 0;
    }
 
    /* HERE WE SET UP OUR SERVER STRUCT */
    memset((char *) &serv_addr, 0, sizeof(serv_addr)); //CLEAR OUR SERVER ADDRESS STRUCT
    portno = atoi(argv[1]); //CONVERT OUR PORT NUMBER TO AN INTEGER
    serv_addr.sin_family = AF_INET; //TYPE OF IP ADDRESS
    serv_addr.sin_port = htons(portno); //ASSIGN PORTNO.

    /* HERE WE GET THE APPROPIATE ADDRESS FOR OUR SERVER */ 
    if (inet_aton(SERVER , &serv_addr.sin_addr) == 0) {
        printf("*** ERROR *** inet_aton(): FAILED\n");
        return 0;
    }

    /* INITIALIZE OUR VARIABLES IN OUR STRUCT */
    strcpy(temp->ip,"0.0.0.0");
    temp->timeout = 0;
    temp->tid = tID;

    /* SEND DHCP DISCOVER */
    bytessent = sendto(s, (struct ipvar*)temp, (1024+sizeof(temp)), 0 ,(struct sockaddr *) &serv_addr, slen);
    if(bytessent < 0){
        printf("FAILED TO SEND TO SERVER: %s\n\n", strerror(errno));
        return 0;
    }else{
        printf("**** SENDING DHCP DISCOVER ****\n");
        printf("YIADDR:         %s\n", temp->ip);
        printf("TRANSACTION ID: %d\n", temp->tid);
        printf("*******************************\n\n");

        /* HERE IS DETERMINED IF THE SERVER WAS ABLE TO OFFER A IP ADDRESS */
        bytesread = recvfrom(s, temp, sizeof(*temp), 0, (struct sockaddr *) &serv_addr, &slen);
        if(temp->tid == -1){ //THE SERVER RESPONDS WITH A -1 TRANSACTION ID FOR FAILURE
            printf("  *********** ALERT ***********\n");
            printf("DHCP SERVER HAS NO IP's TO OFFER\n\nGOODBYE...\n\n");
            free(temp);
            return 0;
        }

        /* THE SERVER WAS ABLE TO SEND A VAILD IP ADDRESS*/
        if(bytesread > 0){
            printf("***** RECEIVED DHCP OFFER *****\n");
            printf("YIADDR:         %s\n", temp->myip);
            printf("TRANSACTION ID: %d\n", temp->tid);
            printf("TIMEOUT:        %d\n", temp->timeout);
            printf("*******************************\n\n");
        }else{
            printf("*** ERROR: COULD NOT READ FROM SERVER: REASON (%s)", strerror(errno));
            free(temp);
            return 0;
        }

        /* HERE WE SEND OUR DHCP REQUEST */
        strcpy(temp->ip,temp->myip);
        temp->tid += 1;
        bytessent = sendto(s, (struct ipvar*)temp, (1024+sizeof(temp)), 0 ,(struct sockaddr *) &serv_addr, slen);
        if(bytessent > 0){
            printf("***** SENDING DHCP REQUEST *****\n");
            printf("YIADDR:         %s\n", temp->ip);
            printf("TRANSACTION ID: %d\n", temp->tid);
            printf("TIMEOUT:        %d\n", temp->timeout);
            printf("********************************\n\n");
        }else{
            printf("*** ERROR: COULD NOT SEND TO SERVER: REASON (%s)", strerror(errno));
            free(temp);
            return 0;
        }

        /* THE SERVER HAS RESPONDED TO OUR REQUEST */
        bytesread = recvfrom(s, temp, sizeof(*temp), 0, (struct sockaddr *) &serv_addr, &slen);
        if(bytesread > 0){
            if(temp->tid == -1){ //THE SERVER RESPONDS WITH A -1 TRANSACTION ID FOR FAILURE
                printf("  *********** ALERT ***********\n");
                printf("DHCP SERVER HAS INDICTED WRONG ACK...\n\n");
            }else{
                printf("***** RECEIVED DHCP ACK *****\n");
                printf("YIADDR:         %s\n", temp->ip);
                printf("TRANSACTION ID: %d\n", temp->tid);
                printf("TIMEOUT:        %d\n", temp->timeout);
                printf("*******************************\n\n");
            }
        }else{
            printf("*** ERROR: COULD NOT READ FROM SERVER: REASON (%s)", strerror(errno));
        }
    }
    free(temp); //FREE OUR TEMP POINTER
    close(s); //CLOSE OUR SOCKET

    return 0;
}
