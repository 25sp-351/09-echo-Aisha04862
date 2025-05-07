#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

#define SETS_PORT 2345

int main(int argc, char *argv[]) {
    int port = SETS_PORT;
    int verbose = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port = atoi(argv[++i]);
        } else {
            fprintf(stderr, "Usage: %s [-v] [-p port]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    start_server(port, verbose);
    return 0;
}