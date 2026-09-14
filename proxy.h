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

using namespace std;

// initializing epoll and socket loop
void SocketLoop();




#endif