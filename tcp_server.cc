#include "./def.hh"
#include "./channel.hh"
#include "./tcp_server.hh"
#include "./tcp_connection_handler.hh"

#include <errno.h>
#include <sys/epoll.h>
#include <string.h>
#include <vector>
#include <iostream>

TcpServer::TcpServer() : m_epfd(-1), mp_acceptor(nullptr) {
    memset(m_events, 0, sizeof(m_events));
}

auto TcpServer::on_connection(int connfd) -> void {
    m_connections[connfd] = new TcpConnectionHandler(m_epfd, connfd);
}

auto TcpServer::run() -> void {
    m_epfd = epoll_create1(EPOLL_CLOEXEC);
    if (m_epfd <= 0) {
        std::cout << "error in epoll_create1(), errno: " << errno << std::endl;
    }
    std::cout << "epoll fd: " << m_epfd << std::endl;
    mp_acceptor = new Acceptor(m_epfd);
    mp_acceptor->set_callback(this);
    mp_acceptor->run();

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