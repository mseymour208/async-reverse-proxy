#include "proxy.h"

void SocketLoop() {

    // Instantiate epoll instance
    int epoll_fd = epoll_create1(0);

    // Create original socket configured for non-blocking operation
    int original_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (original_socket < 0) {
        // socket creation failure
    }
    
    // Binding setup
    struct sockaddr_in sock_address = {0};

    // Configuring address
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_address.sin_port = htons(8080);

    // Binding socket to specific network address
    int bind_status = bind(original_socket, (struct sockaddr *)&sock_address, sizeof(sock_address));
    if (bind_status < 0) {
        // bind() failure
    }

    // Marking the socket as passive; SOMAXCONN is the system call for max socket connections
    int listen_status = listen(original_socket, SOMAXCONN);
    if (listen_status < 0) {
        // listen() failure
    }

    // Mark as non blocking
    int flags = fcntl(original_socket, F_GETFL, 0);
    if (flags == -1) {
        // get flag error
    }
    flags |= O_NONBLOCK;
    if (fcntl(original_socket, F_SETFL, flags) == -1) {
        // Set flag error
    }

    // register original socket with epoll
    struct epoll_event ev;   // ev structure is data carrier from user space -> linux kernel epoll engine
    ev.events = EPOLLIN;  // What specific events to monitor (EPOLLIN : wake up thread whenever new data)
    ev.data.fd = original_socket;  // Immediately hand exact data payload to original socket
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, original_socket, &ev) == -1) {
        // epoll registration error
    }

    // Enter epoll loop

    // Event array structure
    vector<struct epoll_event> event_vec(10);

    // Polling block
    while (true) {
        int num_events = epoll_wait(epoll_fd, *event_vec, 10, -1)
    }


    // Wait for notification from epoll
    // accept() a new client socket
    // extract file descriptor
    // set to non blocking and register to epoll instance

}

