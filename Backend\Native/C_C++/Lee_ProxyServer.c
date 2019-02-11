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

char *trim(char *string, size_t stringlength, size_t *outputlength);
size_t getheader(char *head, size_t stringlength);
char *urlrequest(char *url, size_t *bytesreturn);
char *checkcache(char *urlname);
int validurl(const char *url);
void listfile(const char *url);
void clearfile();

int main(int argc, char **argv){
    size_t urllen = 0, bytesread = 0;
    int sock = 0, urlsock = 0, msgsocket, filesadded = 0, i = 0, oktorun = 0;
    unsigned long portno = 0;
    char buffer[256], urlresp[65536], url[50];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_size;
    
    if (argc < 2) { //if not enough arguments have been passed
        printf("ERROR, no port provided\n");
        exit(1);
    }
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0){ //creating a socket of type TCP
        printf("*** Error: Could not create socket..\n\n"); //if creation of socket failed
        exit(1);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    
    portno = atoi(argv[1]); //get port number and convert string to int
    
    if((portno <= 0) || (portno <= 1023)){ //if invalid port is given
        printf("*** Error: Invaild port number..\n\n");
        scanf("Please enter valid port number: %lu", &portno);
    }
    serv_addr.sin_family = AF_INET; //IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY; //Any IP address can connect
    serv_addr.sin_port = htons(portno); //Sets port number
    
    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0){ //bind server IP and port to socket 
       printf("***ERROR on binding: %d\n\n", errno);
       return 0;
    }
     
    listen(sock, 5); //listening for connections *NOTE: max of 5 connections
    
    printf("Server(): Now listening for connections...\n\n"); //server is not ready for clients
    
    oktorun = 1; //flag to allow while loop to run and read and write to clients

    clearfile();
    /***  Program Start  ***/
    while(oktorun){
        memset((char *)buffer, 0, sizeof(buffer)); //setting the buffer to zero
        unsigned long bytesrecd = 0, bytessent = 0; // variable to keep track of vowels present in string
        memset(url, 0, sizeof(url));

        
        cli_size = sizeof(struct sockaddr_in);
        if((msgsocket = accept(sock, (struct sockaddr *) &cli_addr, &cli_size)) < 0){ //creation of socket to send data on
            printf("*** msgsocket failed to accept (%d)...\n\n", errno);
            oktorun = 0;
        }
        printf("Client is connected...\n\n");
        
        while((read(msgsocket, buffer , 255)) > 0){ //read from buffer of size
            if(strncmp(buffer, "quit", 3) == 0){ //if client sends a "bye"
                printf("Client quits...Waiting for another connection.\n\n");
                close(msgsocket); //close socket
                break; 
            }
            if(buffer){
                trim(buffer, strlen(buffer), &urllen);
                strncpy(url, buffer, urllen);
                if(!validurl(url)){
                    sprintf(urlresp, "ERROR: %s is not a valid url...", url);
                }else{
                    printf("\nClient has requested for: %s\n\n", url);

                    strcpy(urlresp, urlrequest(url, &bytesread));
                    if(strlen(urlresp) != strlen(url)){
                        bytessent = write(msgsocket, urlresp, strlen(urlresp)); //send or "write" to client
                        if (bytessent < 0) {
                            printf("ERROR writing to Client..\n\n"); //if nothing was sent to client
                        }else{
                            printf("The Webpage has been successfully sent to client.\n\n");
                            memset(urlresp, 0, sizeof(urlresp));
                        }
                    }else{
                        //printf("ERROR Response\n\n");
                        sprintf(urlresp, "ERROR: %s is not a valid url...", url);
                        bytessent = write(msgsocket, urlresp, strlen(urlresp)); //send or "write" to client
                        if (bytessent < 0) {
                            printf("ERROR writing to Client..\n\n"); //if nothing was sent to client
                        }else{
                            printf("The Webpage has been successfully sent to client.\n\n");
                            memset(urlresp, 0, sizeof(urlresp));
                        }
                    }
                }
            }

            memset(buffer, 0, sizeof(buffer)); //setting the buffer to zero
            memset(urlresp, 0, sizeof(urlresp)); //setting the buffer to zero
            memset(url, 0, sizeof(url)); //setting the buffer to zero
        }
    }
    printf("Closing listening socket..\n");
    close(sock); //close listening socket

    return 0; 
}

void clearfile(){
    FILE *fp;
    fp = fopen("list.txt", "w");
    if(fp){
        fclose(fp);
    }
}

void listfile(const char *url){
    FILE *fp,*tempfp, *trfp, *twfp;
    static int counter = 0;
    int cl = 0;
    char line[256];
    memset(line, 0, sizeof(line));
    fp = fopen("list.txt", "a");
    fprintf(fp, "%s\n", url);
    fclose(fp);
    if(counter == 5){
        twfp = fopen("tmp.txt", "w");
        trfp = fopen("list.txt", "r");
        while ((fgets(line, 256, trfp)) != NULL){
                /* skip the line at given line number */
                if (cl != 0){
                    fprintf(twfp, "%s", line);
                }
                cl++;
        }
        fclose(trfp);
        fclose(twfp);
        remove("list.txt"); // remove the original file 
        rename("tmp.txt", "list.txt"); // rename the temporary file to original name
        return;
    }
    counter++;
}

int validurl(const char *url){
    static const char *nospecial="0123456789" 
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            ".";

    while(*url){ // Loop until (*url) == 0.  (*url) is about equivalent to url[0].
        if(strchr(nospecial, *url) == NULL) return 0;
        url++; // Jump to the next character.  Adding one to a pointer moves it ahead one element.
    }
    return 1; // Return 1 for success.
}

char *urlrequest(char *url, size_t *bytesreturn){
    SOCKET urlsock = 0;
    size_t urllen = 0, headerlen = 0;
    unsigned long bytessent = 0;
    char urlresp[65536], get[50], header[100], fname[50];
    char *reply = 0, *replyheader = 0;
    struct hostent *server;
    struct sockaddr_in saddr;
    FILE *fp = 0;
    *bytesreturn = 0;
    
    memset(urlresp, 0, sizeof(urlresp));
    memset(fname, 0, sizeof(fname));
    memset(header, 0, sizeof(header));
    
    if ((urlsock = socket(AF_INET, SOCK_STREAM, 0)) <= 0){ //creating a socket of type TCP
        printf("*** Error: Could not create socket..\n\n"); //if creation of socket failed
        return;
    }
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(80);
    if(url){
        /* Checking if we have the index file cached */
        reply = checkcache(url);
        if(reply){
            printf("**** %s WAS CACHED... Sending FILE****\n\n", url);
            return reply; //return contents
        }
        
        sprintf(get,"GET / HTTP/1.1\r\nHOST: %s\r\n\r\n",url); //get request for html file
        server = gethostbyname(url); //IP address of server
        if(server){ //valid connection
            memcpy(&saddr.sin_addr, server->h_addr, server->h_length);
            if(connect(urlsock,(struct sockaddr*)&saddr, sizeof(saddr)) < 0){ //unable to connect to server
                printf("ERROR connecting the server...%s", strerror(errno));
                return;
            }
            bytessent = write(urlsock, get, strlen(get)); //send or "write" to client
            if(bytessent){
                if((read(urlsock, urlresp , 65535)) > 0){ //read with a max packet size of 65535
                    reply = urlresp;
                    close(urlsock); //close our socket
                    strncpy(header,urlresp,sizeof(header)); //get header from packet
                    if(strstr(header, "200")){ //if header contains 200 code
                        /* Create file for our cache */
                        sprintf(fname,"%s.html",url);
                        fp = fopen(fname, "w"); //store file to our cache
                        fprintf(fp, "%s", urlresp); 
                        fclose(fp);
                        
                        listfile(url);

                        printf("The webpage has been successfully cached with filename: %s\n\n", fname);
                        /***************************/
                        
                        *bytesreturn = strlen(reply);
                        return reply;
                    }else{
                        /* Do not cache send raw packet back to client */
                        printf("\nHTTP Response is not 200..Sending back to client..\n\n");
                        replyheader = header;
                        *bytesreturn = strlen(replyheader);
                        return replyheader;
                    }
                }else{
                    printf("ERROR reading from the url...%s", strerror(errno));
                }
            }else{
                printf("Failed to write...\n\n");
            }
        }else{
            printf("ERROR: %s\n", strerror(errno));
        }
    }
    close(urlsock);
    return url;
}

char *trim(char *string, size_t stringlength, size_t *outputlength) {
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

/* This function checks our cache for any of the given file */
char *checkcache(char *urlname){
    FILE *fp = 0, *mainfp = 0;
    char contents[65536], *retval = 0;
    char fname[50];
    char line[256];
    memset(contents, 0, sizeof(contents));
    memset(fname, 0, sizeof(fname));
    memset(line, 0, sizeof(line));

    if(!urlname) return urlname; //not a valid pointer
    
    mainfp = fopen("list.txt", "r");
    if(mainfp){
        while(fgets(line, sizeof(line), mainfp) != NULL){
            if(!strncmp(urlname, line, strlen(urlname))){
                sprintf(fname, "%s.html", urlname); //create file name
                if(access(fname, F_OK) != -1 ) { // file exists
                    fp = fopen(fname, "r"); //open file to get contents
                    if(fp){ //valid file
                        fread(contents, 1, sizeof(contents), fp); //store contents
                        retval = contents; //put into return val
                    }
                    fclose(fp); //close file
                }
            }
        }
    }
    fclose(mainfp);
    return retval;
}
