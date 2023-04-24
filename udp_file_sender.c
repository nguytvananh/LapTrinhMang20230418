#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LENGTH 20

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s <server-IP-address> <port> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sender = socket(AF_INET, SOCK_DGRAM, 0);
    if (sender < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    FILE *fp = fopen(argv[3], "rb");
    if (fp == NULL)
    {
        perror("fopen() failed");
        exit(EXIT_FAILURE);
    }

    if (sendto(sender, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) != strlen(argv[3]))
    {
        perror("sendto() failed");
        exit(EXIT_FAILURE);
    }

    char buf[MAX_LENGTH + 1];
    memset(buf, 0, MAX_LENGTH + 1);
    while (fgets(buf, MAX_LENGTH + 1, fp) != NULL)
    {
        if (sendto(sender, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) != strlen(buf))
        {
            perror("sendto() failed");
            exit(EXIT_FAILURE);
        }
        memset(buf, 0, MAX_LENGTH + 1);
    }
    sendto(sender, NULL, 0, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Sent file %s to %s:%d successfully!\n", argv[3], argv[1], atoi(argv[2]));

    fclose(fp);
    close(sender);

    return 0;
}