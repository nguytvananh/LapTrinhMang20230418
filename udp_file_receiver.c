#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LENGTH 1024

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int receiver = socket(AF_INET, SOCK_DGRAM, 0);
    if (receiver < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(receiver, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    char filename[MAX_LENGTH];
    memset(filename, 0, MAX_LENGTH);
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int bytes_received = recvfrom(receiver, filename, MAX_LENGTH, 0, (struct sockaddr *)&client_addr, &client_addr_len);
    if (bytes_received < 0)
    {
        perror("recvfrom() failed");
        exit(EXIT_FAILURE);
    }
    strcat(filename, ".out");

    char buf[MAX_LENGTH];
    memset(buf, 0, MAX_LENGTH);
    while (1)
    {
        bytes_received = recvfrom(receiver, buf, MAX_LENGTH, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (bytes_received < 0)
        {
            perror("recvfrom() failed");
            exit(EXIT_FAILURE);
        }
        if (bytes_received == 0)
        {
            break;
        }
        FILE *fp = fopen(filename, "ab");
        if (fp == NULL)
        {
            perror("fopen() failed");
            exit(EXIT_FAILURE);
        }
        if (fwrite(buf, 1, bytes_received, fp) != bytes_received)
        {
            perror("fwrite() failed");
            exit(EXIT_FAILURE);
        }
        fclose(fp);
    }

    close(receiver);
    printf("File received from %s:%d successfully!\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    printf("File %s saved successfully!\n", filename);

    return 0;
}