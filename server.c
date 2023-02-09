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

/*
https://www.rfc-editor.org/rfc/rfc868
[...]
The time is the number of seconds since 00:00 (midnight) 1 January 1900
GMT, such that the time 1 is 12:00:01 am on 1 January 1900 GMT; this
base will serve until the year 2036.

For example:

   the time  2,208,988,800 corresponds to 00:00  1 Jan 1970 GMT,

             2,398,291,200 corresponds to 00:00  1 Jan 1976 GMT,

             2,524,521,600 corresponds to 00:00  1 Jan 1980 GMT,

             2,629,584,000 corresponds to 00:00  1 May 1983 GMT,

        and -1,297,728,000 corresponds to 00:00 17 Nov 1858 GMT.
*/
#define TIME_1900_S_1970 2208988800

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
                time_t t = time(NULL)+TIME_1900_S_1970;

                unsigned long prep = htonl(t);

                if (sendto(my_socket, (void*)&prep, sizeof(unsigned long), 0, (struct sockaddr*)&client_address, addr_len) < 0)
                    printf("Error Could Not Send!\n");
                else
                {
                    printf("Connection from (%s:%d)\n", ip, (unsigned short)client_address.sin_port);
                }
            }
        }
        memset(incomming, '\0', BUFFER_SIZE);
    }

    close(my_socket);
    return 0;
}
