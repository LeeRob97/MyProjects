#!/usr/local php
<?php
/*
 Lee Chandler Robertson
 ----------------------
 Description: This file is a tcp server that runs locally and will receive from clients and output the client's response.
              Since a server tends to run infinite loop I have a signal handler that catchs the interupts allowing the server to
              finish any task before closing and exiting.

 Running: php lee_tcp_serv.php <portnumber>
 */


declare(ticks=1);
function sighand($sig, $info=NULL) {
    global $oktorun;
    echo "Signal Caught, exiting...\n";
    $sigmsg = "Signal ";
    switch ($sig) {
        case SIGINT:
            $sigmsg.="\"SIGTERM\"";
            $oktorun = 0;
            break;
        case SIGTERM:
            $sigmsg.="\"SIGTERM\"";
            $oktorun = 0;
            break;
        case SIGKILL:
            $sigmsg.="\"SIGKILL\"";
            $oktorun = 0;
            break;
        case SIGABRT:
            $sigmsg.="\"SIGABRT\"";
            break;
        case SIGHUP:
            $sigmsg.="\"SIGHUP\"";
            break;
        default:
            break;
    }
}

pcntl_signal(SIGTERM, "sighand");
pcntl_signal(SIGABRT, "sighand");
pcntl_signal(SIGINT, "sighand");
pcntl_signal(SIGHUP, "sighand");

function socket_error($socket){
    echo "SOCKET ERROR: ".socket_strerror(socket_last_error($socket))."\n";
}

/* SOCKET VARIABLES */
$oktorun = 1;
$socket = 0;
$msgsocket = 0;
$bytes_sent = 0;
$bytes_recd = "";

$l_read = array();
$l_write = array();
$l_exec = array();
/********************/

if(count($argv) < 2){
    echo "Please Enter a Port Number..\n";
}

if(!is_numeric($argv[1])){
    echo "Please Enter a Valid Port Number..\n";
}

$cli_hellomsg = "Hello Client";
$portno = (int)$argv[1];

if(($socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) == FALSE){
    echo "**** FAILED TO CREATE SOCKET ****\n";
    socket_error($socket);
    $oktorun = 0;
}

/***** Set the option to reuse the port *****/
socket_set_option($socket, SOL_SOCKET, SO_REUSEADDR, 1);

if(socket_bind($socket, "0.0.0.0", $portno) == FALSE){
    echo "**** FAILED TO BIND SOCKET ****\n";
    socket_error($socket);
    $oktorun = 0;
}

if(socket_listen($socket, 200) == FALSE){
    echo "*** SOCKET FAILED TO LISTEN ***\n";
    socket_error($socket);
    $oktorun = 0;
}

echo "*** WAITING ON NEW CLIENT ***\n\n";

while($oktorun){
    /** VARIABLES TO BE RESET AT THE BEGINNING OF EACH ITERATION **/
    array_push($l_read, $socket);

    $bytes_recd = "";
    if(@socket_select($l_read, $l_write, $l_exec, 1, 0)){
        if(($msgsocket = socket_accept($socket)) == FALSE){
            echo "FAILED TO ACCEPT CONNECTION ***\n";
            socket_error($socket);
            break;
        }
        echo "CLIENT CONNECTED..\n\n";

        /**** SAY HELLO TO CLIENT ****/

        $bytes_sent = socket_send($msgsocket, $cli_hellomsg, strlen($cli_hellomsg), 0);
        if($bytes_sent == FALSE){
            echo "FAILED TO SEND TO CLIENT..\n";
            break;
        }
        /**************************/

        /* HERE IS WHERE WE WAIT FOR CLIENTS RESPONSE */
        echo "Waiting on Client's Response..\n";
        while($bytes_recd != "quit") {
            $bytes_recd = socket_read($msgsocket, 1024, PHP_BINARY_READ);
            if (strlen($bytes_recd) > 0) {
                echo "\nCLIENT SAYS: $bytes_recd\n\n";
                if($bytes_recd == "quit"){
                    echo "CLIENT HAS LEFT..\n";
                    socket_close($msgsocket);
                }
            } else {
                echo "*** CLIENT READ ERROR...Possibly disconnected ***\n";
                socket_close($msgsocket);
                break;
            }
        }
    }
}
echo "*** ALERT: CLOSING LISTENING SOCKET\n";
socket_close($socket);
