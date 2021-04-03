#include "./channel.hh"

#include <sys/epoll.h>

#include <iostream>

#include "./callback.hh"
#include "./event_loop.hh"

Channel::Channel(EventLoop* event_loop, int sockfd)
    : m_sockfd(sockfd),
      m_events(0),
      m_revents(0),
      m_state(is_new),
      m_callback(nullptr),
      mp_event_loop(event_loop) {}

auto Channel::set_callback(ChannelCallback* callback) -> void { m_callback = callback; }

auto Channel::set_revents(int revent) -> void { m_revents = revent; }

auto Channel::set_state(ChannelState state) -> void { m_state = state; }

auto Channel::get_events() -> int { return m_events; }

auto Channel::get_sockfd() -> int { return m_sockfd; }

auto Channel::get_state() -> ChannelState { return m_state; }

auto Channel::handle_event() -> void {
    if (m_revents & EPOLLIN) {
        m_callback->handle_read();
    }
    if (m_revents & EPOLLOUT) {
        m_callback->handle_write();
    }
}

auto Channel::enable_read() -> void {
    m_events |= EPOLLIN;
    update();
}

auto Channel::enable_write() -> void {
    m_events |= EPOLLOUT;
    update();
}

auto Channel::disable_write() -> void {
    m_events &= ~EPOLLOUT;
    update();
}

auto Channel::is_writing() -> bool {
    return m_events & EPOLLOUT;
}

auto Channel::update() -> void {
    mp_event_loop->update(this);
}