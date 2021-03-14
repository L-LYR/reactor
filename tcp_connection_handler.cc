#include "./tcp_connection_handler.hh"
#include "./channel.hh"
#include "./def.hh"

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
    char line[max_lines + 1];
    if (sockfd < 0) {
        std::cout << "error (sockfd < 0)" << std::endl;
        return;
    }

    while ((read_length = read(sockfd, line, max_lines)) > 0) {
        line[read_length] = '\0';
        std::cout << "read length: " << read_length << std::endl;
        m_parser.parse(line, read_length);
    }

    if (read_length < 0) {
        if (errno == ECONNRESET) {
            std::cout << "ECONNREST: close socket fd: " << sockfd
                      << std::endl;
            close(sockfd);
        } else if (errno == EAGAIN) {
            if (m_parser.has_finished()) {
                std::string response = m_parser.generate_response();
                std::cout << response << std::endl;
                int ret = write(sockfd, response.c_str(), response.length());
                if ((std::string::size_type)ret != response.length()) {
                    std::cout << "error in write(), errno: " << errno << std::endl;
                }
                m_parser.reset();
            }
        } else {
            std::cout << "errno in read(), errno: " << errno << std::endl;
        }
    } else if (read_length == 0) {
        std::cout << "read nothing from socket fd, close " << sockfd
                  << std::endl;
        close(sockfd);
    }
}