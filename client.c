#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
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
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

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
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
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
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = 0;
    char outgoing[BUFFER_SIZE];
    memset(outgoing, 0, BUFFER_SIZE);
    socklen_t client_addr_len = (socklen_t)sizeof(client_addr);
    if (bind(client_socket, (struct sockaddr*)&client_addr, client_addr_len) < 0)
    {
        printf("Unable to bind!");
        close(client_socket);
        exit(-1);
    }
    
    sendto(client_socket, &outgoing, 1, 0, (struct sockaddr*)&server_addr, server_addr_len);

    close(client_socket);
    return 0;
}