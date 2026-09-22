#include "proxy.h"
#include "chunked_array.h"

void socket_loop() {

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

    // register original socket with epoll
    struct epoll_event ev;   // ev structure is data carrier from user space -> linux kernel epoll engine
    ev.events = EPOLLIN;  // What specific events to monitor (EPOLLIN : wake up thread whenever new data)
    set_socket(original_socket, ev, epoll_fd);

    // Event array structure
    vector<struct epoll_event> event_vec(10);
    struct sockaddr_in client_addr = {0};

    // Instantiate chunked array
    chunked_array<Connection> storage;

    socklen_t client_addr_size = sizeof(client_addr);

    // Polling block
    while (true) {
        // Wait for notification from epoll
        int num_events = epoll_wait(epoll_fd, event_vec.data(), 10, -1);
        for (int i = 0; i < num_events; i++) {

            // NEW socket connection
            if (event_vec[i].data.fd == original_socket) {
                // accept() a new client socket
                int client_socket = accept(original_socket, (struct sockaddr *)&client_addr, &client_addr_size);

                // set to non blocking and register to epoll instance
                set_socket(client_socket, ev, epoll_fd);

                // Instantiate connection
                Connection& new_socket{client_socket, ConnectionState::reading};

                // Populate into chunked array
                storage[client_socket].fd = client_socket;
                storage[client_socket].state = ConnectionState::reading;
                storage[client_socket].backend_fd = -1;


            } 
            // NOT a new socket connection
            else {
                // Non-blocking reads                
                uint32_t set_flags = event_vec[i].events;

                if (set_flags & EPOLLIN) {
                    // Socket is reading
                    Connection& active = storage[event_vec[i].data.fd];
                    char temp_buffer[4096];

                    // Reading loop
                    while(1) {
                        // Read in data from the connected socket
                        ssize_t bytes_read = read(event_vec[i].data.fd, temp_buffer, sizeof(temp_buffer));
                        // If the socket gets blocked, break the loop
                        if (bytes_read == -1) {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                break;
                            }
                        }
                        // Connected socket closed
                        if (bytes_read == 0) {
                            close(event_vec[i].data.fd);
                            break;
                        }
                        // Append our temp buffer to our read buffer
                        active.read_buffer.insert(active.read_buffer.end(), temp_buffer, temp_buffer + bytes_read);
                    }
                    
                }
                if (set_flags & EPOLLOUT) {
                    // socket is writing
                }
                if (set_flags & (EPOLLERR | EPOLLHUP)) {
                    // Socket disconnected/encountered an error
                }

                // Non-blocking socket condition
                // Connection lifecycle management

            }
        }
    }

}

void set_socket(int fd, struct epoll_event &ev, int ep_fd) {
    // Mark as non blocking
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        // get flag error
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        // Set flag error
    }

    ev.data.fd = fd;
    // Register socket with epoll
    if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        // epoll registration error
    }
}

