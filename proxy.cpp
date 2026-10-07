#include "proxy.h"
#include "chunked_array.h"

void socket_loop() {

    // Instantiate epoll instance
    int epoll_fd = epoll_create1(0);

    // Create server socket configured for non-blocking operation
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
    

    // Instantiate chunked array
    chunked_array<Connection> storage;

    event_dispatcher(original_socket, epoll_fd, event_vec, storage);

}

// event_dispatcher
// Returns: N/A
// Parameters: Server socket file descriptor (fd), epoll instance fd, event vector containing epoll event structures,
// chunked array containing Connection structures
// Purpose: The epoll instance will sleep until there is an incoming connection. The kernel will wake up our epoll instance
// and look through our event vector, see if we have a new socket connection or a new connection state for an existing socket
// (read, write, disconnect, forward). If we have a new connection, we accept it using our server socket to spawn a new client
// socket file descriptor
void event_dispatcher(int original_fd, int epoll_fd, vector<struct epoll_event> &event_vec, chunked_array<Connection> &storage) {

    struct sockaddr_in client_addr = {0};
    socklen_t client_addr_size = sizeof(client_addr);
    while(true) {
        int num_events = epoll_wait(epoll_fd, event_vec.data(), 10, -1);
        for (int i = 0; i < num_events; i++) {
            // NEW socket connection
            if (event_vec[i].data.fd == original_fd) {
                // accept() a new client socket
                int client_socket = accept(original_fd, (struct sockaddr *)&client_addr, &client_addr_size);

                // set to non blocking and register to epoll instance
                struct epoll_event ev = {0};
                ev.events = EPOLLIN;
                set_socket(client_socket, ev, epoll_fd);

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
                    client_read(i, event_vec, storage);
                }
                if (set_flags & EPOLLOUT) {
                    // socket is writing
                }
                if (set_flags & (EPOLLERR | EPOLLHUP)) {
                    client_disconnect(i, event_vec, epoll_fd, storage);
                    // Socket disconnected/encountered an error
                }

                // Non-blocking socket condition
                // Connection lifecycle management

            }
        }
    }
}

void client_read(int idx, vector<struct epoll_event> &event_vec, chunked_array<Connection> &storage) {
    // Socket is reading
    Connection& active = storage[event_vec[idx].data.fd];
    char temp_buffer[4096];
    const char* terminator = "\r\n\r\n";

    // Reading loop
    while(true) {
        // Read in data from client socket
        ssize_t bytes_read = read(event_vec[idx].data.fd, temp_buffer, sizeof(temp_buffer));

        // If the socket gets blocked, break the loop
        if (bytes_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
        }

        // Client socket closed
        if (bytes_read == 0) {
            close(event_vec[idx].data.fd);
            break;
        }
        // Move our temporary buffer into our read buffer
        active.read_buffer.insert(active.read_buffer.end(), temp_buffer, temp_buffer + bytes_read);
    }

    // Search our read buffer for a null terminator
    auto it = search(active.read_buffer.begin(), active.read_buffer.end(), terminator, terminator + 4);
    if (it != active.read_buffer.end()) {
        // Print out the read buffer
        string str(active.read_buffer.begin(), active.read_buffer.end());
        cout << str << endl;
    } else {
        // Null terminator not found
        cout << "No null terminator" << endl;
    }

}

void client_write(int idx, int epoll_fd) {

}

void client_disconnect(int idx, vector<struct epoll_event> &event_vec, int epoll_fd, chunked_array<Connection> &storage) {
    // Epoll deregistration
    int shutdown_fd = event_vec[idx].data.fd;

    int disconnect = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, shutdown_fd, NULL);
    // Closing fd
    close(shutdown_fd);
    // Reset connection slot
    // Populate into chunked array
    
    storage[shutdown_fd].fd = -1;
    storage[shutdown_fd].state = ConnectionState::shutdown;
    storage[shutdown_fd].backend_fd = -1;

    storage[shutdown_fd].read_buffer = {};
    storage[shutdown_fd].write_buffer = {};


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

int main() {
    socket_loop();
    return 0;
}