#ifndef MINI_MUDUO_CHANNEL
#define MINI_MUDUO_CHANNEL

#include "./decl.hh"

class Channel {
  public:
    Channel(EventLoop* event_loop, int sockfd);
    ~Channel() = default;

    auto set_callback(ChannelCallback* callback) -> void;

    auto set_revents(int revent) -> void;
    auto get_events() -> int;
    auto get_sockfd() -> int;

    auto handle_event() -> void;

    auto enable_read() -> void;

  private:
    void update();

    int m_epollfd;
    int m_sockfd;
    int m_events;
    int m_revents;
    ChannelCallback* m_callback;
    EventLoop* mp_event_loop;
};

#endif