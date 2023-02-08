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
        printf("Connecting to: %d.%d.%d.%d:%d\n",
               sa.sin_addr.s_addr & 0xff,
               (sa.sin_addr.s_addr >> 8) & 0xff,
               (sa.sin_addr.s_addr >> 16) & 0xff,
               (sa.sin_addr.s_addr >> 24) & 0xff,
               port);
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
    int port;
    if ((port = get_port(argc, argv)) != -1)
    {
        if (get_ip(argc, argv, port, server_addr) == 0)
            exit(-1);
    } else {
        printf("Missing parameters: <ipv4> <port>\n");
        exit(-1);
    }
    socklen_t server_addr_len = (socklen_t)sizeof(server_addr);

    ////////////////////////////////////////////////// Client config
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        printf("Not Good!\n");
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(52365);
    char outgoing[BUFFER_SIZE];
    memset(outgoing, 0, BUFFER_SIZE);
    socklen_t client_addr_len = (socklen_t)sizeof(client_addr);



    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed");

    // Here Starts the measurement 
    unsigned long now;
    memset((void*)&now, 0, sizeof(unsigned long));
    now = (unsigned long)time(NULL);
    if (sendto(client_socket, outgoing, 0, 0, (struct sockaddr*)&server_addr, server_addr_len) < 0)
    {
        printf("Hmmm");
    }
    else
    {
        unsigned int t;
        int size = recvfrom(client_socket, (void*)&t, sizeof(unsigned int), 0, (struct sockaddr *)&server_addr, &server_addr_len);
        if (size)
        {
            printf("Local information:\n\t%s\n", ctime(&now));

        }
        else
            printf("Not Good\n");
    }

    close(client_socket);
    return 0;
}