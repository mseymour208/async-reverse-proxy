#include "proxy.h"

void SocketLoop() {

    // Instantiate epoll instance
    int epoll_fd = epoll_create1(0);

    // Create original socket configured for non-blocking operation
    int original_socket = socket(AF_INET, SOCK_STREAM, O_NONBLOCK);
    
    // Binding setup
    struct sockaddr_in sock_address = {0};

    // Configuring address
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_address.sin_port = htons(8080);

    // Binding socket to specific network address
    int bind_status = bind(original_socket, (struct sockaddr *)&sock_address, sizeof(sock_address));

    // Marking the socket as passive; SOMAXCONN is the system call for max socket connections
    int listen_status = listen(original_socket, SOMAXCONN);

    

    // registered with epoll instance
    // Enter epoll loop
    // Wait for notification from epoll
    // accept() a new client socket
    // extract file descriptor
    // set to non blocking and register to epoll instance

}

