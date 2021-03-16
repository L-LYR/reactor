#include "./channel.hh"
#include "./callback.hh"
#include "./event_loop.hh"

#include <iostream>
#include <sys/epoll.h>

Channel::Channel(EventLoop* event_loop, int sockfd) : m_sockfd(sockfd),
                                                      m_events(0),
                                                      m_revents(0),
                                                      m_callback(nullptr),
                                                      mp_event_loop(event_loop) {}

auto Channel::set_callback(ChannelCallback* callback) -> void { m_callback = callback; }

auto Channel::set_revents(int revent) -> void { m_revents = revent; }

auto Channel::get_events() -> int { return m_events; }

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
    mp_event_loop->update(this);
}