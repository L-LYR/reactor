#include "./selector.hh"

#include <errno.h>

#include "./logger.hh"

Selector::Selector() {
  m_epfd = ::epoll_create1(EPOLL_CLOEXEC);
  if (m_epfd <= 0) {
    error("error in epoll_create1(), errno: %d\n", errno);
  }
}

auto Selector::poll(std::vector<Channel *> &p_channels) -> void {
  int fds = ::epoll_wait(m_epfd, m_events, max_events, -1);
  if (fds == -1) {
    error("error in epoll_wait(), errno: %d\n", errno);
    return;
  }
  for (int i = 0; i < fds; i++) {
    Channel *p_channel = static_cast<Channel *>(m_events[i].data.ptr);
    p_channel->set_revents(m_events[i].events);
    p_channels.push_back(p_channel);
  }
}

auto Selector::update(Channel *channel) -> void {
  struct epoll_event ev;
  ev.data.ptr = channel;
  ev.events = channel->get_events();

  int op;
  if (channel->get_state() == Channel::ChannelState::is_new) {
    op = EPOLL_CTL_ADD;
    channel->set_state(Channel::ChannelState::is_added);
  } else {
    op = EPOLL_CTL_MOD;
  }

  ::epoll_ctl(m_epfd, op, channel->get_sockfd(), &ev);
}