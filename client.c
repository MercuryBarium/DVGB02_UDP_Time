#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include "common.h"

int get_port(int argc, char *argv[])
{
    if (argc < 3)
        return -1;
    else
        return atoi(argv[2]);
}

int get_ip(int argc, char *argv[], int port, struct sockaddr_in sa)
{
    if (inet_pton(AF_INET, argv[1], &sa.sin_addr) == 1)
    {
        /*printf("Connecting to: %d.%d.%d.%d:%d\n",
               sa.sin_addr.s_addr & 0xff,
               (sa.sin_addr.s_addr >> 8) & 0xff,
               (sa.sin_addr.s_addr >> 16) & 0xff,
               (sa.sin_addr.s_addr >> 24) & 0xff,
               port);*/
        return 1;
    }
    else
    {
        printf("Error converting IP address\n");
        return 0;
    }
}

int main(int argc, char *argv[])
{

    struct sockaddr_in client_addr, server_addr;

    server_addr.sin_family = AF_INET;
    int port;
    if ((port = get_port(argc, argv)) != -1)
    {
        server_addr.sin_port = htons(port);
        if (get_ip(argc, argv, port, server_addr) == 0)
            exit(-1);
    }
    else
    {
        printf("Missing parameters: <ipv4> <port>\n");
        exit(-1);
    }
    socklen_t server_addr_len = (socklen_t)sizeof(server_addr);

    ////////////////////////////////////////////////// Client config
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        printf("Not Good!\n");
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    char outgoing[BUFFER_SIZE];
    memset(outgoing, 0, BUFFER_SIZE);
    socklen_t client_addr_len = (socklen_t)sizeof(client_addr);

    if (setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed");

    // Here Starts the measurement
    unsigned long now;
    memset((void *)&now, 0, sizeof(unsigned long));
    now = (unsigned long)time(NULL);
    printf("Local information:\n\t%s\nSending UDP Packet:\n\tIP address: %s\n\tPort: %s\n",
           ctime(&now), argv[1], argv[2]);
    
    if (sendto(client_socket, outgoing, 0, 0, (struct sockaddr *)&server_addr, server_addr_len) < 0)
    {
        printf("Hmmm");
    }
    else
    {
        unsigned int t;
        unsigned int local = time(NULL);    //  Our time
        int size = recvfrom(client_socket, (void *)&t, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &server_addr_len);
        if (size)
        {
            printf("size:%d\n",size);
            t = ntohl(t)-TIME_1900_TO_1970;
            time_t prep = (time_t)t;
            int diff = ((int)local)-((int)t);
            printf("%s\tDifference:%d\n",ctime(&prep), diff);
            if (diff <= 0 && diff >= -1)
            {
                time_t o = (time_t)local;
                printf("yahooo!\n\tOur time: %s\n\tTheir time: %s\n\tDiff: %d\n", ctime(&o), ctime(&prep), diff);
            }
            else
                printf("Nahoo!");
        }
        else
            printf("Not Good\n");
    }

    close(client_socket);
    return 0;
}