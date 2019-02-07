#!/usr/bin php
<?php
/*
Lee Chandler Robertson
----------------------
Description: This file is a tcp client that will send generic messages to the server and then it will exit.

Running: php lee_tcp_cli.php <portnumber>
*/

    /*
        The purpose of this function is close the TCP socket properly by notifying setting the FIN bit and then closing
        the file descriptor.
    */
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

    if(count($argv) < 2){
        echo "Please Enter a Port Number..\n";
    }

    if(!is_numeric($argv[1])){
        echo "Please Enter a Valid Port Number..\n";
    }

    $portno = (int)$argv[1];

    if(($socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) == FALSE){
        echo "**** FAILED TO CREATE SOCKET ****\n";
        socket_error($socket);
        exit(0);
    }

    if(socket_connect($socket, "0.0.0.0", $portno) == FALSE){
        echo "**** FAILED TO CONNECT SOCKET ****\n";
        socket_error($socket);
        exit(0);
    }

    echo "*** CONNECTED TO SERVER ***\n\n";

    /***** COMMUNICATON WITH SERVER START *****/
    $bytes_recd = socket_read($socket, 1024, PHP_BINARY_READ);
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