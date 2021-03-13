#ifndef MINI_MODUO_CHANNEL
#define MINI_MODUO_CHANNEL

#include "./decl.hh"

// each channel is responsible for only one fd's IO
class Channel {
  private:
    void update();

    int m_epollfd;
    int m_sockfd;
    int m_events;
    int m_revents;
    ChannelCallback* m_callback;

  public:
    Channel(int epfd, int sockfd);
    ~Channel() = default;

    auto set_callback(ChannelCallback* callback) -> void;
    auto set_revent(int revent) -> void;
    auto get_sockfd() -> int;
    auto handle_event() -> void;
    auto enable_read() -> void;
};

#endif