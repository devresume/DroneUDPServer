/*
    Simple UDP Server
*/

#include "commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <termios.h>

#define BUFLEN 2041  //Max length of buffer
#define PORT 14551   //The port on which to listen for incoming data

int main()
{
    int s;
    struct sockaddr_in server, si_other;
    socklen_t slen;
    int recv_len;
    char buf[BUFLEN];
    //WSADATA wsa;



    //Initialise winsock
    printf("\nInitialising Winsock...");
//    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
//    {
//        //printf("Failed. Error Code : %d", WSAGetLastError());
//        exit(EXIT_FAILURE);
//    }
    printf("Initialised.\n");

    //Create a socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("Could not create socket");
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    if(setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        printf("Set socket options error \n");
        //exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    //Bind
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        //printf("Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done");

    slen = sizeof si_other;
    //keep listening for data
    while (1)
    {
        printf("Waiting for data...");
        fflush(stdout);

        //clear the buffer by filling null, it might have previously received data
        memset(buf, '\0', BUFLEN);


        //try to receive some data, this is a blocking call
        while((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            //printf("recvfrom() failed with error code");
            //exit(EXIT_FAILURE);
            //std::cout << " Got: " << recv_len << " bytes" << std::endl;
        }

        float x = 0.0, y = 0.0, z = 0.0, yaw = 0.0;
        std::string str = std::string(buf);
        int i = atoi(str.c_str());
        if(i != 0)
        {
            setControls(i, x, y, z, yaw);
        }
        printf("%f, %f, %f, %f", x, y, z, yaw);

        //print details of the client/peer and the data received
        char ipbuf[INET_ADDRSTRLEN];
        printf("Received packet from %s:%d\n", inet_ntop(AF_INET, &si_other.sin_addr, ipbuf, sizeof(ipbuf)), ntohs(si_other.sin_port));
        printf("Data: %s\n", buf);

        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            printf("sendto() failed with error code");
            exit(EXIT_FAILURE);
        }
    }

    close(s);
    //WSACleanup();

    return 0;
}
