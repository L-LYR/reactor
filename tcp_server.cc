#include "./tcp_server.hh"
#include "./def.hh"
#include "./decl.hh"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <iostream>

TcpServer::TcpServer() : m_epfd(-1), m_listenfd(-1) {}

auto TcpServer::bind_and_listen() -> void {
    // listen socket
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);  // TCP
    if (m_listenfd < 0) {
        std::cout << "error in socket(), errno: " << errno << std::endl;
    }
    if (-1 == fcntl(m_listenfd, F_SETFL, O_NONBLOCK)) {
        std::cout << "error in fcntl()" << std::endl;
    }  // non-block io

    int on = 1;
    if (-1 == setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
        std::cout << "error in setsockopt(), errno: " << errno << std::endl;
    }  // reusable addr in bind

    // bind
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(10086);
    if (-1 == bind(m_listenfd, (sockaddr*)&server_addr, sizeof(server_addr))) {
        std::cout << "error in bind(), errno: " << errno << std::endl;
    }
    // listen
    if (-1 == listen(m_listenfd, max_listen_fd)) {
        std::cout << "error in listen(), errno: " << errno << std::endl;
    }
    // show info
    std::cout << "bind result: "
              << "[" << inet_ntoa(server_addr.sin_addr) << ":"
              << ntohs(server_addr.sin_port) << "]"
              << std::endl;
}

auto TcpServer::connect() -> void {
    sockaddr_in client_addr;
    socklen_t client_length = sizeof(sockaddr_in);
    int connfd = accept(m_listenfd, (sockaddr*)&client_addr, (socklen_t*)&client_length);
    if (connfd < 0) {
        std::cout << "error in accept(), connfd: " << connfd << " errno: " << errno << std::endl;
        return;
    }
    std::cout << "new connection from [" << inet_ntoa(client_addr.sin_addr)
              << ":" << ntohs(client_addr.sin_port) << "]"
              << " new socket fd: " << connfd << std::endl;

    fcntl(connfd, F_SETFL, O_NONBLOCK);

    Channel* p_channel = new Channel(m_epfd, connfd);
    p_channel->set_callback(this);
    p_channel->enable_read();
}

auto TcpServer::echo(int sockfd) -> void {
    int read_length;
    char line[max_lines];
    if (sockfd < 0) {
        std::cout << "error (sockfd < 0)" << std::endl;
        return;
    }
    memset(line, 0, sizeof(line));
    if ((read_length = read(sockfd, line, max_lines)) < 0) {
        if (errno == ECONNRESET) {
            std::cout << "ECONNREST: close socket fd: " << sockfd
                      << std::endl;
            close(sockfd);
        } else {
            std::cout << "errno in read(), errno: " << errno << std::endl;
        }
    } else if (read_length == 0) {
        std::cout << "read nothing from socket fd, close"
                  << std::endl;
        close(sockfd);
    } else {
        std::cout << "constent: \n"
                  << line << std::endl;
        if (write(sockfd, line, read_length) != read_length) {
            std::cout << "error in write()" << std::endl;
        }
    }
}

auto TcpServer::OnIn(int sockfd) -> void {
    std::cout << "OnIn: " << sockfd << std::endl;

    if (sockfd == m_listenfd) {
        connect();
    } else {
        echo(sockfd);
    }
}

auto TcpServer::run() -> void {
    bind_and_listen();

    m_epfd = epoll_create1(EPOLL_CLOEXEC);
    if (m_epfd <= 0) {
        std::cout << "error in epoll_create1()" << std::endl;
    }

    Channel* p_listen_channel = new Channel(m_epfd, m_listenfd);
    p_listen_channel->set_callback(this);
    p_listen_channel->enable_read();

    std::vector<Channel*> channels;
    while (true) {
        int fds = epoll_wait(m_epfd, m_events, max_events, -1);
        if (fds == -1) {
            std::cout << "error in epoll_wait(), errno: " << errno << std::endl;
            break;
        }

        for (int i = 0; i < fds; i++) {
            Channel* p_channel = static_cast<Channel*>(m_events[i].data.ptr);
            p_channel->set_revent(m_events[i].events);
            channels.push_back(p_channel);
        }

        for (auto p : channels) {
            p->handle_event();
        }

        channels.clear();
    }
}