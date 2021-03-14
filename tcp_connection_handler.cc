#include "./tcp_connection_handler.hh"
#include "./channel.hh"
#include "./def.hh"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

TcpConnectionHandler::TcpConnectionHandler(int epfd, int connfd) : m_epfd(epfd), m_connfd(connfd) {
    mp_channel = new Channel(epfd, connfd);
    mp_channel->set_callback(this);
    mp_channel->enable_read();
}

auto TcpConnectionHandler::on_in(int sockfd) -> void {
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
        std::cout << "content: \n"
                  << line << std::endl;
        if (write(sockfd, line, read_length) != read_length) {
            std::cout << "error in write()" << std::endl;
        }
    }
}