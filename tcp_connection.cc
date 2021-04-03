#include "./tcp_connection.hh"

#include <errno.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "./channel.hh"
#include "./event_loop.hh"
#include "./server.hh"

TcpConnection::TcpConnection(EventLoop* event_loop, int connfd)
    : mp_event_loop(event_loop) {
    mp_channel = new Channel(event_loop, connfd);
    mp_channel->set_callback(this);
    mp_channel->enable_read();
}

auto TcpConnection::handle_read() -> void {
    int sockfd = mp_channel->get_sockfd();

    if (sockfd < 0) {
        std::cout << "error (sockfd < 0)" << std::endl;
        return;
    }

    int ret = m_in_buffer.read_from_sockfd(sockfd);

    if (ret < 0) {
        if (errno == ECONNRESET) {
            std::cout << "ECONNREST: close socket fd: " << sockfd
                      << std::endl;
            close(sockfd);
        } else {
            std::cout << "error in read(), errno: " << errno << std::endl;
        }
    } else if (ret == 0) {
        std::cout << "read nothing from socket fd, close " << sockfd
                  << std::endl;
        close(sockfd);
    } else {
        mp_server->on_message(this, m_in_buffer);
    }
}

auto TcpConnection::handle_write() -> void {
    int ret = 0;
    int sockfd = mp_channel->get_sockfd();
    if (mp_channel->is_writing()) {
        ret = ::write(sockfd, m_out_buffer.readable_begin(), m_out_buffer.readable_bytes());
        if (ret < 0) {
            std::cout << "error in write(), errno: " << errno << std::endl;
        } else {
            m_out_buffer.retrieve(ret);
            if (m_out_buffer.readable_bytes() == 0) {
                mp_channel->disable_write();
                mp_event_loop->queue_loop(this);
            }
        }
    }
}

auto TcpConnection::run() -> void {
    mp_server->on_write_done(this);
}

auto TcpConnection::send(const char* data, size_t length) -> void {
    int ret = 0;
    int sockfd = mp_channel->get_sockfd();
    if (!mp_channel->is_writing() && m_out_buffer.readable_bytes() == 0) {
        ret = write(sockfd, data, length);
        if (ret < 0) {
            std::cout << "errno in write(), errnor: " << errno << std::endl;
        } else if (size_t(ret) == length) {
            mp_event_loop->queue_loop(this);
        }
    }
    if (size_t(ret) < length) {
        m_out_buffer.append(data + ret, length - ret);
        if (mp_channel->is_writing()) {
            mp_channel->enable_write();
        }
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