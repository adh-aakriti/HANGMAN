#include "server.h"
#include "../common/utils.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int create_server_socket(int port) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        die("socket failed");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        die("setsockopt failed");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        die("bind failed");

    if (listen(server_fd, 10) < 0)
        die("listen failed");

    return server_fd;
}

