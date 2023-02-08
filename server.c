#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"

int my_socket;

void sigint_handler(int sig)
{
    signal(SIGINT, sigint_handler);
    printf("Stopping sockets.");
    close(my_socket);
    fflush(stdout);
    exit(0);
}

int main(int argc, char const *argv[])
{
    time_t now;

    struct tm start_date;
    start_date.tm_hour = 0; // 00:00
    start_date.tm_year = 0; // 1900
    start_date.tm_mday = 1; // Jan 1st
    start_date.tm_mon = 0;  // Jan


    if (argc < 2)
    {
        printf("Missing argument port.\n");
        exit(-1);
    }
    int port = atoi(argv[1]);
    signal(SIGINT, sigint_handler);
    if ((my_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        return -1;
    struct sockaddr_in my_address, client_address;
    char incomming[BUFFER_SIZE];
    int addr_len = sizeof(client_address);
    memset(incomming, '\0', BUFFER_SIZE);

    my_address.sin_family = AF_INET;
    my_address.sin_port = htons(port);
    my_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(my_socket, (struct sockaddr *)&my_address, sizeof(my_address)) < 0)
    {
        printf("Unable to bind!");
        return -1;
    }

    printf("Running UDP Time Server on Port: %d\n", port);

    while (1)
    {
        int recSize = recvfrom(my_socket, incomming, BUFFER_SIZE, 0,
                               (struct sockaddr *)&client_address, &addr_len);
        if (recSize >= 0)
        {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_address.sin_addr, ip, INET_ADDRSTRLEN);
            //printf("%s", incomming);
            if (recSize != 0)
                printf("Connection from (%s:%d): UDP Packet is not empty, ignoring.\n", ip, (unsigned short)client_address.sin_port);
            else
            {
                printf("Connection from (%s:%d)\n", ip, (unsigned short)client_address.sin_port);
                unsigned int t;
                memset((void*)&t, 0, sizeof(unsigned int));
                t = htonl((unsigned int)time(NULL));
                if (sendto(my_socket, (void*)&t, sizeof(unsigned int), 0, (struct sockaddr*)&client_address, addr_len) < 0)
                    printf("Faaan");
                else
                {
                    printf("Sending time()\t->\t %u\n\n", t);
                }
            }
        }
        memset(incomming, '\0', BUFFER_SIZE);
    }

    close(my_socket);
    return 0;
}
