#!/usr/bin/env php
<?php
    function sock_close($sock){
        if($sock > 0){
            socket_shutdown($sock);
            socket_close($sock);
        }
    }

    function socket_error($socket){
        echo "SOCKET ERROR: ".socket_strerror(socket_last_error($socket))."\n";
    }

    /* SOCKET VARIABLES */
    $oktorun = 1;
    $socket = 0;
    $bytes_sent = 0;
    $bytes_recd = 0;
    /********************/

    $input = "";

    if(($socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) == FALSE){
        echo "**** FAILED TO CREATE SOCKET ****\n";
        socket_error($socket);
        exit(0);
    }

    if(socket_connect($socket, "0.0.0.0", 8881) == FALSE){
        echo "**** FAILED TO CONNECT SOCKET ****\n";
        socket_error($socket);
        exit(0);
    }

    /***** COMMUNICATON WITH SERVER START *****/
    $bytes_recd = socket_read($socket, 1024, PHP_NORMAL_READ);
    if(strlen($bytes_recd) > 0){
        echo "SERVER SAYS: $bytes_recd\n\n";
    }
    echo "Type message: or enter \"quit\" to exit.\n";
    while($input != "quit"){
        $input = readline("Message: ");
        $bytes_sent = socket_send($socket, $input, strlen($input), 0);
        if($bytes_sent == FALSE){
            echo "\nFAILED TO SEND TO CLIENT..\n\n";
            break;
        }
        echo "\nSENT TO SERVER...\n\n";
        echo "YOU ENTERED: $input\n\n";
    }

    echo " *** ALERT: CLOSING SOCKET\n";
    sock_close($socket);