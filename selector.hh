#ifndef MINI_MUDUO_SLECTOR
#define MINI_MUDUO_SLECTOR

#include <sys/epoll.h>

#include <vector>

#include "./channel.hh"
#include "./def.hh"

class Selector {
public:
  Selector();
  ~Selector() = default;
  auto poll(std::vector<Channel *> &p_channels) -> void;
  auto update(Channel *channel) -> void;

private:
  int m_epfd;
  epoll_event m_events[max_events];
};

#endif