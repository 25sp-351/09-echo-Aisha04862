#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    int socket;
    int verbose;
    struct sockaddr_in addr;
} client_info;

void *handle_client(void *arg) {
    client_info *info = (client_info *)arg;
    char buffer[BUFFER_SIZE];
    FILE *fp = fdopen(info->socket, "r+");

    if (fp == NULL) {
        perror("fdopen");
        close(info->socket);
        free(info);
        pthread_exit(NULL);
    }

    if (info->verbose) {
        printf("Connected: %s:%d\n",
               inet_ntoa(info->addr.sin_addr), ntohs(info->addr.sin_port));
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        fputs(buffer, fp);     // echo back to the client
        fflush(fp);            

        if (info->verbose) {
            printf("Received: %s", buffer);  // print result to terminal
        }
    }

    if (info->verbose) {
        printf("Disconnected: %s:%d\n",
               inet_ntoa(info->addr.sin_addr), ntohs(info->addr.sin_port));
    }

    fclose(fp);
    close(info->socket);
    free(info);
    pthread_exit(NULL);
}

void start_server(int port, int verbose) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Echo server listening on port %d\n", port);

    while (1) {
        client_info *info = malloc(sizeof(client_info));
        if (!info) {
            perror("malloc");
            continue;
        }

        socklen_t addr_len = sizeof(info->addr);
        info->socket = accept(server_fd, (struct sockaddr *)&info->addr, &addr_len);
        if (info->socket < 0) {
            perror("accept");
            free(info);
            continue;
        }

        info->verbose = verbose;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, info) != 0) {
            perror("pthread_create");
            close(info->socket);
            free(info);
            continue;
        }

        pthread_detach(tid); 
    }

    close(server_fd);
}