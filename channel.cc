#include "./channel.hh"
#include "./callback.hh"

#include <iostream>
#include <sys/epoll.h>

Channel::Channel(int epfd, int sockfd) : m_epollfd(epfd),
                                         m_sockfd(sockfd),
                                         m_events(0),
                                         m_revents(0),
                                         m_callback(nullptr) {}

auto Channel::set_callback(ChannelCallback* callback) -> void { m_callback = callback; }

auto Channel::set_revent(int revent) -> void { m_revents = revent; }

auto Channel::get_sockfd() -> int { return m_sockfd; }

auto Channel::handle_event() -> void {
    if (m_revents & EPOLLIN) {
        m_callback->on_in(m_sockfd);
    }
}

auto Channel::enable_read() -> void {
    m_events |= EPOLLIN;
    update();
}

auto Channel::update() -> void {
    epoll_event ev;
    ev.data.ptr = this;
    ev.events = m_events;
    if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_sockfd, &ev) == -1) {
        std::cout << "error in epoll_ctl(), errno: " << errno << std::endl;
    }
}