#include "server.h"
#include "../common/utils.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int create_server_socket(int port) { //start server
    int fd;
    struct sockaddr_in addr; //prepares socket to accept clients
    int opt = 1;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) die("socket failed");

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) die("setsockopt failed");

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        die("bind failed");

    if (listen(fd, 10) < 0)
        die("listen failed");
    return fd;
}



