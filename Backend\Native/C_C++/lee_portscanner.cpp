/*
    LEE ROBERTSON | FINAL VERSION
    --------------------------------------------------------

    NOTES: I HAVE DISCOVEREDSOME SERVERS HAD PLACED TIMEOUTS ON THERE CONNECTIONS, DUE TO 
           THIS IT MAY EXTEND MY TIMEOUT IF THE USER HAS CHOSEN TO USE A TIMEOUT.

           UDP USES A SELECT TO SOMEWHAT HELP AND SEE IF PORT IS OPEN, IF SELECT IS NOT USED THE RECV()
           WOULD BLOCK, HALTING THE PROGRAM. 

           TCP USES A SELECT IF THE USES CHOOSES THE OPTION TO "--ENABLE_TIMEOUT" THIS WILL THEREFORE,
           SET THE SOCKET TO NON-BLOCKING AND USES A SELECT OF 500 MILLISECONDS.

    Compiling: g++ lee_portscanner.cpp //compile using c++11

    Running: EXAMPLE: <executable name> --port 80 --ip 10.10.10.1 --transport udp

    <executable name> --help //for options

*/
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/* SOCKET FILES */

#include <netdb.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;

typedef int SOCKET;

/* FUNCTION PROTOTYPES */
void help();
void UDP(vector<string> IP, vector<string> PORT, int flag);
void TCP(vector<string> IP, vector<string> PORT, int flag);
char *getprotocolmessage(int port, char *ip);
char *getprotocolservice(int port);
int close_sock(int socket);
int SetSocketBlockingEnabled(int fd, int blocking);
vector<string> explode( const string &explodeme, const string &delimiter);
string substrip(string s, string delim);

int main(int argc, char **argv){
    int ind = -1; //index variable used to erase any improper IP's
    int i = 0, both = 0, tcp = 0, udp = 0, flag = 0; //loop and flag variables
    FILE *fp = 0; //if a file is given
    char tempip[20]; 
    setservent(1); //set our servent db to stay open

    /* 
        VECTORS THAT ARE USED FOR:
            1.) TO STORE COMMAND LINE ARGUMENTS.
            2.) PORT AND IP NUMBERS.
    */
    vector<string> optline;
    vector<string> portno;
    vector<string> ip;
    vector<string> transport;
    vector<string> defaultportno;
    string option, temp;

    
    if (argc < 2) { //if not enough arguments have been passed
        printf("ERROR, NOT ENOUGH ARGUMENTS GIVEN..\n");
        help();
        return 0;
    }

    /* HERE WE FILL OUR ARRAY OF DEFAULT PORT NUMBERS */
    for(i = 1; i < 1025; i++){
        defaultportno.push_back(to_string(i)); 
    }
    /**************************************************/

    for(i = 1; i < argc; i++){
        option.append(argv[i]); //HERE I FORM MY COMMAND LINE ARGUMENTS INTO A STRING
    }

    optline = explode(option, "--");
    for(i = 0; i < optline.size(); i++){
        /* HERE WE INTERPRET THE COMMANDS */
        if(optline[i].length() == 0) continue;
        if (optline[i] == "help"){
            help();
            return 0;
        }else if(!optline[i].find("showopen")){
            cout << endl;
            cout << "* NOTE * THESE MAY HAVE CHANGED...\n";
            cout << "-----------------------------------------------------" << endl;
            cout << "      IP       |          PORT         |    PROTOCOL " << endl;
            cout << "-----------------------------------------------------" << endl;
            cout << "162.244.35.50  |          22,80        |      TCP    " << endl;
            cout << "192.168.1.1    |          53,80        |      TCP    " << endl;
            cout << "-----------------------------------------------------" << endl << endl;
            return 0;
        }else if(!optline[i].find("enable_timeout")){
            cout << "\n** ALERT ** ENABLE TIMEOUT SET\n";
            cout << "------------------------------\n";
            cout << "TIMEOUT VALUE: 500 milliseconds\n\n";
            flag = 1;
        }else if(!optline[i].find("port")){
            optline[i] = substrip(optline[i], "port");
            portno = explode(optline[i], ",");
            string front;
            string back;
            int j = 0;
            char *p = 0;
             /* HERE IS WHERE THE "-" CHECK OCCURS */
            for(j = 0; j < portno.size(); j++){
                char delim[] = "-";
                char *haystack = (char *)portno[j].c_str();
                p = strstr(haystack, delim); //HERE STRSTR WILL RETURN IF "-" FOUND OR NULL IF NOT
                if(p){ // "-" FOUND
                    front = portno[j].substr(0,portno[j].find("-")); //GET PORT NUMBER IN-FRONT
                    back = portno[j].substr(portno[j].find("-") + 1); //GET PORT NUMBER BEHIND
                    portno.push_back(front); //PUSH FRONT PORT NUMBER INTO PORTNO VECTOR
                    break;
                }
            }
            if(p){
                portno.erase(portno.begin() + j);
                int diff = atoi(back.c_str()) - atoi(front.c_str());
                int z = 0;
                int sum = atoi(front.c_str());
                for(; z < diff; z++){
                    sum++;
                    string port = to_string(sum);
                    portno.push_back(port);
                }
            }
        }else if(!optline[i].find("ip")){
            optline[i] = substrip(optline[i], "ip");
            ip = explode(optline[i], ",");
            string front;
            string back;
            int j = 0;
            char *p = 0;
            string notmask, subnet, tempsubnet;
            /* HERE THE SAME STEPS ARE USED LIKE PORT NUMBER IF "-" IS FOUND */
            for(j = 0; j < ip.size(); j++){
                char delim[] = "-";
                char *haystack = (char *)ip[j].c_str();
                p = strstr(haystack, delim);
                if(p){
                    notmask = ip[j].substr(0,ip[j].find("-"));
                    subnet = notmask.substr(0, notmask.find_last_of("."));
                    front = notmask.substr(notmask.find_last_of(".") + 1);
                    back = ip[j].substr(ip[j].find("-") + 1);
                    tempsubnet = subnet;
                    tempsubnet.append("."+front);
                    ip.push_back(tempsubnet);
                    break;
                }
            }
            if(p){
                ip.erase(ip.begin() + j);
                int diff = atoi(back.c_str()) - atoi(front.c_str());
                int z = 0;
                int sum = atoi(front.c_str());
                for(; z < diff; z++){
                    sum++;
                    tempsubnet = subnet;
                    tempsubnet.append("."+to_string(sum));
                    ip.push_back(tempsubnet);
                }
            }
        }else if(!optline[i].find("transport")){
            temp = substrip(optline[i], "transport");
            transport = explode(temp, ",");
        }else if(!optline[i].find("file")){
            string filename;
            char line[20];
            filename = substrip(optline[i], "file");
            fp = fopen(filename.c_str(), "r");
            if(!fp){
                cout << "* ERROR * INVALID FILE ***\n\n";
            }else{
                while(fgets(line, 20, fp) != NULL){
                    string str(line);
                    str.erase(remove(str.begin(), str.end(), '\n'), str.end());
                    ip.push_back(str);
                }
            }
        }else{
            cout << "UNKNOWN COMMAND: " << optline[i] << endl;
        }
    }

    /* 
        HERE WE:
        (1) REMOVE ANY FOREIGN CHARACTERS 
        (2) CHECK IF ANY "-" WERE MISSED TO ENSURE WE HIT ALL PORTS
    */
    for(i = 0; i < portno.size(); i++){
        int intport = 0;
        intport = atoi(portno[i].c_str());
        string front;
        string back;
        int j = 0;
        char *p = 0;

        /* HERE IS WHERE THE "-" CHECK OCCURS */
        for(j = 0; j < portno.size(); j++){
            char delim[] = "-";
            char *haystack = (char *)portno[j].c_str();
            p = strstr(haystack, delim); //HERE STRSTR WILL RETURN IF "-" FOUND OR NULL IF NOT
            if(p){ // "-" FOUND
                front = portno[j].substr(0,portno[j].find("-")); //GET PORT NUMBER IN-FRONT
                back = portno[j].substr(portno[j].find("-") + 1); //GET PORT NUMBER BEHIND
                portno.push_back(front); //PUSH FRONT PORT NUMBER INTO PORTNO VECTOR
                break;
            }
        }
        if(p){ //IF "-" FOUND
            portno.erase(portno.begin() + j); //REMOVE ILLEGAL EXPRESSION
            int diff = atoi(back.c_str()) - atoi(front.c_str()); //CALCULATE DIFFERENCE FROM PORT NUMBERS
            int z = 0;
            int sum = atoi(front.c_str()); //THIS IS USED TO INCREMENT TO OUR GIVEN PORT NUMBER
            for(; z < diff; z++){ //FILL VALUES FOR PORT NUMBERS 
                sum++;
                string port = to_string(sum);
                portno.push_back(port);
            }
        }
        if(intport > 65535){ //INVALID PORT NUMBER -OUT OF RANGE
            cout << "INVALID PORT GIVEN * OUT OF RANGE *\n";
            return 0;
        }
    }
    cout << endl;

    /* CHECK FOR EMPTY IPS IN OUR ARRAY */
    for(i = 0; i < ip.size(); i++){
        if(ip[i].length() == 0){
            ind = i;
        }
    }
    if(ind != -1){
        ip.erase(ip.begin() + ind); //remove invalid IP
    }
    /**************************/

    /* HERE WE DETERMINE IF IP IS PRESENT FOR PORT SCANNING */
    if(ip.size() == 0){
        cout << "PLEASE ENTER A VALID IP: ";
        cin >> tempip;
        ip.push_back(tempip);
    }else if(ip[0].length() == 0){ //THIS STATEMENT IS A FAIL SAFE INCASE A FOREIGN VALUE IS IN THE ARRAY
        cout << "PLEASE ENTER A VALID IP: ";
        cin >> tempip;
        ip.push_back(tempip);
    }
    /****************************************/

    /* HERE WE DETERMINE WHICH PROTOCOLS TO USE */ 
    if(transport.size() > 0){
        for(i = 0; i < transport.size(); i++){
            if((transport[i] == "udp") || (transport[i] == "UDP")){
                udp = 1; //SET UDP FLAG
                if(tcp == 1){ //CHECK IF TCP FLAG HAS BEEN SET
                    both = 1; //IF SO, THEN BOTH NEED TO BE SET
                }
            }else if((transport[i] == "tcp") || (transport[i] == "TCP")){
                /* SAME AS ABOVE BUT VICE-VERSA */
                tcp = 1;
                if(udp == 1){
                    both = 1;
                }
            }else{ //IF GIVEN PROTOCOL CANNOT BE DETERMINED USE DEFAULT OPTION
                cout << "UNKNOWN TRANSPORT: USING DEFAULT OPTION..\n\n";
                both = 1;
            }
        }
    }else{ //USE DEFAULT IF NO OPTIONS ARE GIVEN
        both = 1;
    }
    /******************************************/

    /* ENTRY POINT FOR OUR SOCKETS */
    if(portno.size() > 0){ //PORT NUMBER GIVEN
        if(both == 1){ //TCP AND UDP FLAG HAS BEEN SET
            UDP(ip, portno, flag);
            TCP(ip, portno, flag);
        }else if(tcp == 1){ //ONLY TCP FLAG HAS BEEN SET
            TCP(ip, portno, flag);
        }else if(udp == 1){ //ONLY UDP FLAG HAS BEEN SET
            UDP(ip, portno, flag);
        }
    }else{ //USE DEFAULT PORT NUMBER
        cout << "\n* ALERT * NO PORT GIVEN USING DEFAULT [1-1024]\n\n";
        if(both == 1){
            UDP(ip, defaultportno, flag);
            TCP(ip, defaultportno, flag);
        }else if(tcp == 1){
            TCP(ip, defaultportno, flag);
        }else if(udp == 1){
            UDP(ip, defaultportno, flag);
        }
    }

    /* END OF PROGRAM */
    cout << "END OF LIST..\n\n";
    endservent();
    return 0; 
}
/* HELP MENU*/
void help(){
    printf("\n\n****************************** MENU OPTIONS ******************************\n");
    printf("******************         NOTE TYPE AS SHOWN           ******************\n");
    printf("--------------------------------------------------------------------------\n");
    printf("OPTIONS:          DEFINITIONS                                             \n");
    printf("-------           -----------                                             \n");
    printf("--help            DISPLAYS THE OPTION MENU.                               \n\n");
    printf("--showopen        DISPLAYS KNOWN OPEN IP AND PORTS.                       \n\n");
    printf("--port            THIS OPTION IS USED FOR TARGETING SPECIFIC PORTS        \n");
    printf("                  (EX: --port 23 OR --port 23, 25) commas are important!! \n\n");
    printf("--ip **REQUIRED   THIS OPTION IS USED FOR SINGLE OR MULTIPLE IP ADDRESS.  \n");
    printf("                  *NOTE* IF MULTIPLE IPs ARE GIVEN USE COMMAS TO SEPARATE.\n");
    printf("                  (EX: --ip 10.10.10.2 OR --ip 10.10.10.2, 10.10.10.3)    \n\n");
    printf("--file            THIS OPTION TAKES A FILE NAME WITH A LIST OF IP ADDRESS.\n\n");
    printf("--transport       THIS OPTION IS USED IF THE USER WOULD LIKE TO USE A     \n");
    printf("                  SPECIFIC TYPE OF PROTOCOL. (EX: TCP OR UDP)             \n\n");
    printf("--enable_timeout  THIS OPTION IS USED IF THE USER WOULD LIKE TO USE A     \n");
    printf("                  extend timeout OF the TCP PROTOCOL.                     \n");
    printf("**************************************************************************\n\n");
}

/* STRING FUNCTIONS */

/*
    THE PURPOSE OF THIS FUNCTION IS TO REMOVE GIVEN STRINGS FROM THE MAIN STRING
*/
string substrip(string s, string delim){
    string::size_type i = s.find(delim);

    if (i != string::npos){
        s.erase(i, delim.length());
    }

    return s;
}
/*
    THE PURPOSE OF THIS FUNCTION IS TO SET HELP PARSE THE ARGUMENTS
    GIVEN FOR THE PORT SCAN PROGRAM. IT TAKES A DELIMITER AND BASED ON THAT
    WILL CREATE A ARRAY OF STRINGS WHEN THAT DELIMINATOR IS HIT.

    ON ERROR: EMPTY ARRAY IS GIVEN.
*/
vector<string> explode(const string &str, const string &delim){
    vector<string> arr; //return value
    int str_len = str.length(); //given string length
    int del_len = delim.length(); //deliminator length

    /* LOOP VARIABLES */
    int i = 0; 
    int k = 0;

    /*
        if no delimiter is given return empty array
    */
    if (del_len==0) return arr;

    while(i < str_len){ //iterator through string
        int j = 0;
        while ((i+j < str_len) && (j < del_len) && (str[i+j] == delim[j])){
            j++;
        }

        //Deliminator has been found
        if (j == del_len){ 
            arr.push_back(str.substr(k, i-k)); //here we take the value from the deliminator and push onto return array
            i += del_len;
            k = i;
        }else{
            i++;
        }
    }
    arr.push_back(str.substr(k, i-k));
    return arr;
}
/********************/

/*
    THIS FUNCTIONS PURPOSE WAS TO BE USED IN UDP TO SEND THE PROPER TYPE OF PACKET
    IN WHICH WE COULD GET A RESPONSE FROM THE SERVER.

    ON ERROR: DEFAULT MESSAGE IS SENT.
*/
char *getprotocolmessage(int port, char *ip){
    static char message[100];
    memset(message, 0, 100);
    if(!ip){
        sprintf(message, "GET");
        return message;
    }

    switch(port){
        case 53:
            sprintf(message, "DNS");
            break;
        case 80:
            sprintf(message, "GET / HTTP/1.1\r\nHOST: %s\r\n\r\n", ip);
            break;
        default:
            sprintf(message, "GET");
            break;
    }
    return (char *)message;
}

/*
    THE PURPOSE OF THIS FUNCTION IS TO RETURN WHAT TYPE OF SERVICE
    IS USED REGARDING THE PORT NUMBER. 

    ON ERROR: UNKNOWN IS RETURNED
*/
char *getprotocolservice(int portnum, const char *proto){
    struct servent *port; 
    port = getservbyport(htons(portnum), proto); //HERE THE OUR PORT NUM IS CONVERTED TO SHORT FOR NETWORK BIT
    if(!port){ //GETSERVBYPORT RETURNED NULL
        return (char *)"UNKNOWN";
    }
    return port->s_name; //THE FUNCTION HAS FOUND THE GIVEN PORT NAME
}

/*
    THE PURPOSE OF THIS FUNCTION IS TO BE USED ON
    TCP SOCKETS TO ENSURE THE FYN BIT IS SET THEN
    FINALLY CLOSE OUR SOCKET

    ON ERROR: -1 IS RETURNED
*/
int close_sock(int socket){
    int sc = 0;

    shutdown(socket, 2);
    sc = close(socket);

    return sc;
}

/*
    THE PURPOSE OF THIS FUNCTION IS TO SET THE TCP SOCKET
    TO NON-BLOCKING. THIS IS DONE TO ALLOW OUR CONNECT() FUNCTION
    TO NOT LOCK UP OUR CODE. WHICH IN TURN THE FOLLOWING SELECT()
    WILL CATCH ANYTHING INTERESTING ON THE SOCKET.

    ON ERROR: 0 IS RETURNED
*/
int SetSocketBlockingEnabled(int fd, int blocking) {
    if(fd < 0){ 
        return 0;
    }
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags == -1){ 
        return 0;
    }

    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);

    return (fcntl(fd, F_SETFL, flags) == 0) ? 1 : 0;
}

/*
    THIS FUNCTION HANDLES ALL THE UDP TRANSPORT.
    WITH THE GIVEN VECTOR OF IPS AND PORT NUMBERS IT WILL TEST
    EACH ADDRESS AND PORT TO SEE IF THEY ARE LISTENING AND IF THE SOCKET
    GETS A RESPONSE WE SET THAT SOCKET AS OPEN.

    ON ERROR: THE FUNCTION WILL SKIP OR RETURN DEPENDING ON THE GIVEN ERROR.
*/
void UDP(vector<string> IP, vector<string> PORT, int flag){
    int i, j; 
    vector<string> port = PORT;
    vector<string> ip = IP;

    for(i = 0; i < ip.size(); i++){
        char *SERVER = (char *)ip[i].c_str();
        int opencounter = 0;
        cout << endl << "SCANNING IP: " << SERVER << endl;
        cout << "-- -- -- -- -- -- -- --\n";
        cout << "UDP PORT          STATE           SERVICE\n";
        cout << "------------------------------------------\n";
        for(j = 0; j < port.size(); j++){
            SOCKET s = 0;
            struct sockaddr_in serv_addr;
            int portno = 0, openportfound = 0, sr;
            socklen_t slen=sizeof(serv_addr);
            size_t bytesread = 0, bytessent = 0;
            char buffer[255];
            string result = "";
            char *message = 0, *service = 0;
            timeval timer;
            fd_set rfds;


            if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
                printf("**** SOCKET CREATE ERROR ***\n\n");
                return;
            }
            memset((char *) &serv_addr, 0, sizeof(serv_addr));
            portno = atoi(port[j].c_str()); //The port on which to listen for incoming data
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(portno);

            message = getprotocolmessage(portno, SERVER);
            service = getprotocolservice(portno, "udp");
            
            if (inet_aton(SERVER , &serv_addr.sin_addr) == 0) {
                printf("*** ERROR *** inet_aton(): FAILED\n");
                break;
            }

            FD_ZERO(&rfds);
            FD_SET(s, &rfds);

            bytessent = sendto(s, message, strlen(message), 0 ,(struct sockaddr *) &serv_addr, slen);
            if(bytessent < 0){ //TEST 
                cout << "ERROR " << strerror(errno) << endl;
                break;
            }

            /* HERE WE USE SELECT() TO KEEP THE PROGRAM FROM LOCKING UP DUE TO NO RESPONSE */
            /* HERE OUR TIMEVAL STRUCT IS SET FOR THE SELECT() */
            timer.tv_usec = 500000; //will allow select to wait for 500 milliseconds which is more than enough
            timer.tv_sec = 0; 
            if((select((s+1), &rfds, NULL, NULL, &timer)) > 0){ //We use this timeout to see if select determine there is data to read.
                if(FD_ISSET(s, &rfds)){ //if FD_ISSET() returns valid fds to read from
                    bytesread = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr *) &serv_addr, &slen);
                    if(bytesread > 0){ //VALIDATE DATA HAS BEEN RETURNED FROM GIVEN IP AND PORT
                        cout << "UDP SERVER RESPONDED: " << buffer << endl;
                    }
                    result = "open"; //SET OUR RESULT TO OPEN
                    openportfound = 1; //SET THE FLAG FOR DISPLAY PURPOSES
                }
            }
            
            string space; //THIS VARIABLE IS JUST USED FOR FORMATTING
            if(portno > 999){
                space =  "              ";
            }else if(portno > 99){
                space =  "               ";
            }else{
                space =  "                ";
            }

            if(openportfound){ //DISPLAY WHAT PORT WAS FOUND AND WHAT SERVICE ITS RUNNING
                opencounter++;
                cout << portno << space << result << "          " << service << "\n\n";
            }
            close(s); //CLOSE THIS SOCKET
        }
        if(opencounter == 0){ //IF WE HAVE NOT FOUND AN OPEN PORT FROM GIVEN PORT RANGE
            cout << "No Open Ports or Host Down\n";
        }
        cout << endl;
    }
}

/*
    THIS FUNCTION HANDLES ALL THE TCP TRANSPORT.
    WITH THE GIVEN VECTOR OF IPS AND PORT NUMBERS IT WILL TEST
    EACH ADDRESS AND PORT TO SEE IF THEY ARE LISTENING BY TRYING THE CONNECT()
    AND IF THAT RETURNS 0 WE HAVE SUCCESSFULLY CONNECTED

    ON ERROR: THE FUNCTION WILL SKIP OR RETURN DEPENDING ON THE GIVEN ERROR.
*/
void TCP(vector<string> IP, vector<string> PORT, int flag){
    int i, j; 

    /* DOUBLE CHECK WE HAVE ACTUAL PORTS TO CHECK */
    if(PORT.size() == 0){
        cout << "* ERROR * NO PORTS ARE GIVEN\n\n";
        return;
    }

    vector<string> port = PORT;
    vector<string> ip = IP;
    cout << "\n* NOTE * TCP MAY TAKE A FEW MINUTES\n"; 
    if(!flag){
        cout << "\n            -TYPE \"--enable_timeout\" TO PUT A TIMEOUT.\n"; 
    }
    for(i = 0; i < ip.size(); i++){
        char *SERVER = (char *)ip[i].c_str();
        int opencounter = 0;
        int blockenabled = 0, sr = 0;
        cout << endl << "SCANNING IP: " << SERVER << endl;
        cout << "-- -- -- -- -- -- -- --\n";
        cout << "TCP PORT          STATE           SERVICE\n";
        cout << "------------------------------------------\n";
        for(j = 0; j < port.size(); j++){
            SOCKET sock = 0; 
            int portno, bytesent = 0, cc = 0, sc = 0, openportfound = 0;
            char *service = 0;
            struct sockaddr_in serv_addr;
            string result = "closed";
            fd_set rfds, wfds;
            timeval timer;
            

            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0){ //creation of socket
                cout << "*** Error: Could not create socket..\n\n";
            }

            if(flag){
                blockenabled = SetSocketBlockingEnabled(sock, 0);
            }

            memset(&serv_addr, 0, sizeof(serv_addr)); //clearing our struct
            portno = atoi(port[j].c_str()); //The port on which to listen for incoming data
            serv_addr.sin_family = AF_INET; // IP type
            serv_addr.sin_port = htons(portno); //setting port number

            if (!inet_pton(AF_INET, SERVER, &serv_addr.sin_addr.s_addr)) { //getting whats needed to connect to server
                cout << "Invalid IP Address\n\n";
                return;
            }

            /* HERE WE SET OUR FILE DESCRIPTORS */
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);
            FD_SET(sock, &rfds);
            FD_SET(sock, &wfds);

            /*  HERE IS WHERE WE DETERMINE IF THE PORT IS OPEN OR CLOSED  */
            cc = connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
            if (cc != -1) { //connecting to socket allowing connection to server
                result = "open";
                openportfound = 1;
            }
            if(cc == -1 && flag){
                timer.tv_usec = 500000; //500 milliseconds
                timer.tv_sec = 0;
                if(select((sock+1), &rfds, &wfds, NULL, &timer) > 0){
                    if((sr = FD_ISSET(sock, &wfds))){
                        result = "open";
                        openportfound = 1;
                    }
                }
            }
            service = getprotocolservice(portno, "tcp"); //OUR FUNCTION CALL TO DETERMINE WHAT TYPE OF SERVICE IS USED ON THAT PORT
            /* HERE WE DETERMINE OUR PRINT SPACING BASED ON PORTNO SIZE */
            string space;
            if(portno > 999){
                space =  "              ";
            }else if(portno > 99){
                space =  "               ";
            }else{
                space =  "                ";
            }

            if(openportfound){
                opencounter++;
                cout << portno << space << result << "          " << service << "\n\n";
            }

            /* HERE WE CLOSE OUR SOCKET */
            sc = close_sock(sock); //THIS FUNCTION GRACEFULLY SHUTSDOWN OUR CURRENT CONNECTION
            if(sc == -1){
                cout << "FAILED TO CLOSE: " << strerror(errno) << endl;
                return;
            }
        }
        if(opencounter == 0){ //THIS IP HAS NOT PORTS ITS LISTENING ON
            cout << "No Open Ports or Host Down\n";
        }
        cout << endl;
    }
}
