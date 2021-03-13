#include "tcp_server.hh"

auto tcp_server::run() -> void {
    // listen socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);  // TCP
    if (listen_fd < 0) {
        std::cout << "error in socket()" << std::endl;
    }
    fcntl(listen_fd, F_SETFL, O_NONBLOCK);  // non-block io

    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  // reusable addr in bind

    // bind
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(10086);
    if (-1 == bind(listen_fd, (sockaddr *)&server_addr, sizeof(server_addr))) {
        std::cout << "error in bind()" << std::endl;
    }
    // listen
    if (-1 == listen(listen_fd, max_listen_fd)) {
        std::cout << "error in listen()" << std::endl;
    }
    // info
    std::cout << "bind result: "
              << "[" << inet_ntoa(server_addr.sin_addr) << ":"
              << ntohs(server_addr.sin_port) << "]"
              << std::endl;
    // epoll
    int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) {
        std::cout << "error in epoll_create1()" << std::endl;
    }
    epoll_event e;
    e.data.fd = listen_fd;
    e.events = EPOLLIN;
    if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &e)) {
        std::cout << "error in epoll_ctl(), errno: " << errno << std::endl;
    }

    // endless loop
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(sockaddr_in);
    epoll_event events[max_events];
    int conn_fd, sock_fd;
    char line[max_lines];
    int read_length;

    while (true) {
        int fds = epoll_wait(epoll_fd, events, max_events, -1);
        if (fds == -1) {
            std::cout << "error in epoll_waits()" << std::endl;
            break;
        }

        for (int i = 0; i < fds; ++i) {
            if (events[i].data.fd == listen_fd) {
                conn_fd = accept(listen_fd, (sockaddr *)&client_addr,
                                 (socklen_t *)&client_len);
                if (conn_fd > 0) {
                    std::cout << "new connection from "
                              << "[" << inet_ntoa(client_addr.sin_addr) << ":"
                              << ntohs(client_addr.sin_port) << "]"
                              << " accept socket fd: " << conn_fd << std::endl;
                } else {
                    std::cout << "error in accept(), errno: " << errno
                              << std::endl;
                }
                fcntl(conn_fd, F_SETFL, O_NONBLOCK);
                e.data.fd = conn_fd;
                e.events = EPOLLIN;
                if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &e)) {
                    std::cout << "error in epoll_ctl(), errno: " << errno
                              << std::endl;
                }
            } else if (events[i].events & EPOLLIN) {
                if ((sock_fd = events[i].data.fd) < 0) {
                    std::cout << "EPOLLIN sock_fd < 0" << std::endl;
                    continue;
                }
                memset(line, 0, sizeof(line));
                if ((read_length = read(sock_fd, line, max_listen_fd)) < 0) {
                    if (errno == ECONNRESET) {
                        std::cout << "ECONNREST: close socket fd: " << sock_fd
                                  << std::endl;
                        close(sock_fd);
                    } else if (read_length == 0) {
                        std::cout << "read nothing from socket fd, close"
                                  << std::endl;
                        close(sock_fd);
                    } else {
                        if (write(sock_fd, line, read_length) != read_length) {
                            std::cout << "error in write()" << std::endl;
                        }
                    }
                }
            }
        }
    }
}