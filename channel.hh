#ifndef MINI_MUDUO_CHANNEL
#define MINI_MUDUO_CHANNEL

#include "./decl.hh"

class Channel {
  public:
    enum ChannelState : short {
        is_new = 0,
        is_added
    };
    Channel(EventLoop* event_loop, int sockfd);
    ~Channel() = default;

    auto set_callback(ChannelCallback* callback) -> void;
    auto set_revents(int revent) -> void;
    auto set_state(ChannelState state) -> void;

    auto get_events() -> int;
    auto get_sockfd() -> int;
    auto get_state() -> ChannelState;

    auto handle_event() -> void;

    auto enable_read() -> void;
    auto enable_write() -> void;
    auto disable_write() -> void;
    auto is_writing() -> bool;

  private:
    void update();

    int m_sockfd;
    int m_events;
    int m_revents;
    ChannelState m_state;
    ChannelCallback* m_callback;
    EventLoop* mp_event_loop;
};

#endif