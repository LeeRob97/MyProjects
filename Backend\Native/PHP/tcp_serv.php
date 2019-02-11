#!/usr/bin/env php
<?php
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

$cli_hellomsg = "Hello Mike Hawk\n";

if(($socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) == FALSE){
    echo "**** FAILED TO CREATE SOCKET ****\n";
    socket_error($socket);
    $oktorun = 0;
}

/***** Set the option to reuse the port *****/
socket_set_option($socket, SOL_SOCKET, SO_REUSEADDR, 1);

if(socket_bind($socket, "0.0.0.0", 8881) == FALSE){
    echo "**** FAILED TO BIND SOCKET ****\n";
    socket_error($socket);
    $oktorun = 0;
}

if(socket_listen($socket, 200) == FALSE){
    echo "*** SOCKET FAILED TO LISTEN ***\n";
    socket_error($socket);
    $oktorun = 0;
}

while($oktorun){
    /** VARIABLES TO BE RESET AT THE BEGINNING OF EACH ITERATION **/
    array_push($l_read, $socket);

    $msg_read = array();
    $msg_write = array();
    $msg_exec = array();

    $bytes_recd = "";
    echo "*** WAITING ON NEW CLIENT ***\n\n";
    if(socket_select($l_read, $l_write, $l_exec, 1, 0)){
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

        array_push($msg_read, $msgsocket);
        /* HERE IS WHERE WE WAIT FOR CLIENTS RESPONSE */
        echo "Waiting on Client's Response..\n";
        if(socket_select($msg_read, $msg_write, $msg_exec, 1, 0)){
            $bytes_recd = socket_read($msgsocket, 1024, PHP_BINARY_READ); //PHP_BINARY_READ FOR TESTING WILL BECOME PHP_NORMAL_READ
            if(strlen($bytes_recd) > 0){
                echo "\nCLIENT SAYS: $bytes_recd\n\n";
            }else{
                echo "*** CLIENT READ ERROR...Possibly disconnected ***\n";
                break;
            }
        }
        socket_close($msgsocket);
    }
}
echo "*** ALERT: CLOSING LISTENING SOCKET\n";
socket_close($socket);
