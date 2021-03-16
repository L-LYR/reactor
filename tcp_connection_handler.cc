#include "./def.hh"
#include "./channel.hh"
#include "./tcp_connection_handler.hh"

#include <unistd.h>
#include <errno.h>
#include <iostream>

TcpConnectionHandler::TcpConnectionHandler(EventLoop* event_loop, int connfd)
    : m_connfd(connfd), mp_event_loop(event_loop) {
    mp_channel = new Channel(event_loop, connfd);
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

    RequestHandler request_parser;

    while ((read_length = read(sockfd, line, max_lines)) > 0) {
        line[read_length] = '\0';
        request_parser.parse(line, read_length);
    }

    if (read_length < 0) {
        if (errno == ECONNRESET) {
            std::cout << "ECONNREST: close socket fd: " << sockfd
                      << std::endl;
            close(sockfd);
        } else if (errno == EAGAIN) {
            if (request_parser.generate_response()) {
                int ret = write(sockfd,
                                request_parser.get_response(),
                                request_parser.get_response_length());

                if (ret != request_parser.get_response_length()) {
                    std::cout << "error in write(), errno: " << errno << std::endl;
                }
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