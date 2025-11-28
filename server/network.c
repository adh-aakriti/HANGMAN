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
        die("setsockopt");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        die("setsockopt");

    address.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", &(address.sin_addr)) <= 0)
        die("Invalid address/ Address not supported");
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        die("listen");

    return server_fd;
}


