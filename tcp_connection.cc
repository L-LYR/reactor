#include "./channel.hh"
#include "./server.hh"
#include "./tcp_connection.hh"

#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string>

TcpConnection::TcpConnection(EventLoop* event_loop, int connfd)
    : m_connfd(connfd), mp_event_loop(event_loop), m_read_length(0) {
    mp_channel = new Channel(event_loop, connfd);
    mp_channel->set_callback(this);
    mp_channel->enable_read();
}

auto TcpConnection::on_in(int sockfd) -> void {
    if (sockfd < 0) {
        std::cout << "error (sockfd < 0)" << std::endl;
        return;
    } else if (sockfd != m_connfd) {
        std::cout << "error sockfd, want " << m_connfd << " get " << sockfd << std::endl;
    }
    m_read_length = ::read(sockfd, m_read_buffer, max_lines);
    if (m_read_length < 0) {
        if (errno == ECONNRESET) {
            std::cout << "ECONNREST: close socket fd: " << sockfd
                      << std::endl;
            close(sockfd);
        } else {
            std::cout << "error in read(), errno: " << errno << std::endl;
        }
    } else if (m_read_length == 0) {
        std::cout << "read nothing from socket fd, close " << sockfd
                  << std::endl;
        close(sockfd);
    } else {
        m_read_buffer[m_read_length] = '\0';
        mp_server->on_message(this, std::string(m_read_buffer));
    }
}

auto TcpConnection::regester_in(Server* server) -> void {
    mp_server = server;
}

auto TcpConnection::establish() -> void {
    if (mp_server != nullptr) {
        mp_server->on_connection(this);
    }
}

auto TcpConnection::write(const std::string& data) -> void {
    int ret = ::write(m_connfd, data.c_str(), data.length());
    if (ret < 0) {
        std::cout << "error in write(), errno: " << errno << std::endl;
    } else if (ret < static_cast<int>(data.length())) {
        std::cout << "left " << data.length() - ret << " bytes" << std::endl;
    }
}