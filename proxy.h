#ifndef PROXY_H
#define PROXY_H

#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <algorithm>

using namespace std;

// initializing epoll and socket loop
void socket_loop();
void set_socket(int fd, struct epoll_event &ev, int ep_fd);

// Sets one or more states for each connection
enum class ConnectionState {
    reading,
    waiting,
    forwarding,
    shutdown
};

// Stores data for each client connection
struct Connection {
    int fd;
    vector<char> read_buffer;
    vector<char> write_buffer;
    ConnectionState state;
    int backend_fd;
};




#endif