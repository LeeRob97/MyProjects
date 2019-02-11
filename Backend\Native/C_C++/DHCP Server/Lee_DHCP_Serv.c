//LEE ROBERTSON | CSCE 3530 | SERVER_HW3

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

typedef int SOCKET;

/* FUNCTION PROTOTYPES */
void listfile(char *url);
void clientinfo(char *IP, int ID, int lifetime);
void dhcpoffer(char *IP, int ID, int lifetime);
void dhcpreq(char *IP, int ID, int lifetime);
void dhcpack(char *IP, int ID, int lifetime);
char *trim(char *string, size_t stringlength, size_t *outputlength);

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
    SOCKET sock = 0; 
    int oktorun = 0, portno = 0;
    char tempip[20];
    struct sockaddr_in serv_addr, cli_addr;
    int cli_size = sizeof(cli_addr);
    struct ipvar *temp = 0;

    temp = calloc(sizeof(struct ipvar), 1);
    
    if (argc < 2) { //if not enough arguments have been passed
        printf("ERROR, NO PORT NUMBER GIVEN\n");
        return 0;
    }
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){ //creating a socket of type DUP
        printf("*** Error: Could not create socket..\n\n"); //if creation of socket failed
        return 0;
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]); //get port number and convert string to int
    
    if((portno <= 0) || (portno <= 1023)){ //if invalid port is given
        printf("*** Error: RESERVED port number..\n\n");
        return 0;
    }
    
    serv_addr.sin_family = AF_INET; //IPv4
    serv_addr.sin_port = htons(portno); //Sets port number
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //Any IP address can connect
    
    if(bind(sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0){ //bind server IP and port to socket 
       printf("***ERROR on binding: %s\n\n", strerror(errno));
       return 0;
    }

    system("clear");
    printf("UDP Server(): Now listening for connections...\n\n"); //server is not ready for clients
    oktorun = 1; //flag to allow while loop to run and read and write to clients 

    /***  Program Start  ***/
    while(oktorun){
        unsigned long bytessent = 0; 
        int bytesread = 0;
        size_t iplen = 0;
        memset(tempip, 0, 20);

        /* CLIENT INITIAL CONNECTION */
        printf("\nWaiting on Clients To Connect..\n");
        bytesread = recvfrom(sock, temp, sizeof(*temp), 0, (struct sockaddr *) &cli_addr, &cli_size);
        if(bytesread > 0){
            clientinfo(temp->ip, temp->tid, temp->timeout);

            /* Fetch IP for DHCP OFFER */
            listfile(tempip); //pull from file
            trim(tempip, strlen(tempip), &iplen);
            if(iplen == 0){
                temp->tid = -1; //SIGNAL TO CLIENT THE IP ADDRESS CANNOT BE ASSIGNED
                printf("     *** ALERT ***\n");
                printf("*** NO IP's TO OFFER *** \n\n");
                bytessent = sendto(sock, (struct ipvar*)temp, (1024+sizeof(temp)), 0 ,(struct sockaddr *) &cli_addr, cli_size);
                break;
            }

            /* IP ADDRESSES CAN BE ASSIGNED AT THIS POINT */
            strncpy(temp->myip, tempip, iplen);
            temp->timeout = 3600;
            dhcpoffer(temp->myip, temp->tid, temp->timeout);
            bytessent = sendto(sock, (struct ipvar*)temp, (1024+sizeof(temp)), 0 ,(struct sockaddr *) &cli_addr, cli_size);

            /* CLIENTS DHCP REQUEST */
            if(bytessent > 0){
                bytesread = recvfrom(sock, temp, sizeof(*temp), 0, (struct sockaddr *) &cli_addr, &cli_size);
                if(bytesread > 0){
                    dhcpreq(temp->ip, temp->tid, temp->timeout);
                }else{
                    printf("*** ERROR: COULD NOT READ FROM CLIENT: REASON (%s)", strerror(errno));
                    break;
                }
            }else{
                printf("*** ERROR: COULD NOT SEND TO CLIENT: REASON (%s)", strerror(errno));
                break;
            }

            /* SEND DHCP ACK */
            if(!strcmp(temp->ip, temp->myip)){ //ensure client ip matches the new given ip address
                dhcpack(temp->ip, temp->tid, temp->timeout);
            }else{
                temp->tid = -1;
                printf("*** ERROR: INVALID IP ***\n\n");
            }

            bytessent = sendto(sock, (struct ipvar*)temp, (1024+sizeof(temp)), 0 ,(struct sockaddr *) &cli_addr, cli_size);
            if(bytessent < 0){
                printf("*** ERROR: COULD NOT SEND TO CLIENT: REASON (%s)", strerror(errno));
                break;
            }
            memset(temp, 0, sizeof(struct ipvar));
        }else{
            printf("*** ERROR: COULD NOT READ FROM CLIENT: REASON (%s)", strerror(errno));
        }
    }

    printf("Closing listening socket..\n");
    close(sock); //close listening socket

    free(temp);
    return 0; 
}

void clientinfo(char *IP, int ID, int lifetime){
    printf("\n********* DHCP DISCOVER *********\n");
    printf("YIADDR:           %s\n", IP);
    printf("TRANSACTION ID:   %d\n", ID);
    if(lifetime > 0){
        printf("LIFE TIME:      %d\n", lifetime);
    }
    printf("*********************************\n\n");
    return;
}

void dhcpoffer(char *IP, int ID, int lifetime){
    printf("\n*********** DHCP OFFER ***********\n");
    printf("YIADDR:           %s\n", IP);
    printf("TRANSACTION ID:   %d\n", ID);
    printf("LIFE TIME:        %d\n", lifetime);
    printf("***********************************\n\n");
    return;
}

void dhcpreq(char *IP, int ID, int lifetime){
    printf("\n*********** DHCP REQUEST **********\n");
    printf("YIADDR:           %s\n", IP);
    printf("TRANSACTION ID:   %d\n", ID);
    printf("LIFE TIME:        %d\n", lifetime);
    printf("************************************\n\n");
    return;
}

void dhcpack(char *IP, int ID, int lifetime){
    printf("\n*********** DHCP ACK ***********\n");
    printf("YIADDR:           %s\n", IP);
    printf("TRANSACTION ID:   %d\n", ID);
    printf("LIFE TIME:        %d\n", lifetime);
    printf("***********************************\n\n");
    return;
}

void listfile(char *ip){
    FILE *fp,*tempfp, *trfp, *twfp;
    static int counter = 0;
    int cl = 0;
    char line[25];
    //char tempip[25];


    memset(line, 0, sizeof(line));

    fp = fopen("IPaddress.txt", "r");
    fgets(ip, 20, fp);

    fclose(fp);

    twfp = fopen("tmp.txt", "w");
    trfp = fopen("IPaddress.txt", "r");
    while ((fgets(line, 25, trfp)) != NULL){
        /* skip the line at given line number */
        if (cl != 0){
            fprintf(twfp, "%s", line);
        }
        cl++;
    }
    fclose(trfp);
    fclose(twfp);
    remove("IPaddress.txt"); // remove the original file 
    rename("tmp.txt", "IPaddress.txt"); // rename the temporary file to original name
    return;
}

char *trim(char *string, size_t stringlength, size_t *outputlength) {
	//static char mt[] = "";
	size_t i = 0;
	char *retval = 0;
	if (!string || !outputlength) return string;
	retval = string;
	for (i = 0; i < stringlength; i++) {
		if (retval == string) {
			if (string[i] != ' ' &&
				string[i] != '\t' &&
				string[i] != '\r' &&
				string[i] != '\n') {
				retval = &string[i];
				break;
			}
		}
	}
	// Here, "retval" is the first non-whitespace character.
	if ( /* It was ALL whitespace */ i == stringlength) {
		*outputlength = 0;
		return retval;
	}
	*outputlength = stringlength - (retval - string);
	for (i = (stringlength - 1); &string[i] > retval; i--) {
		if (string[i] != ' ' &&
			string[i] != '\t' &&
			string[i] != '\r' &&
			string[i] != '\n') {
			*outputlength -= (stringlength - (i + 1));
			break;
		}
	}
	return retval;
}
