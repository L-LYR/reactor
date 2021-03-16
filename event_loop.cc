#include "./event_loop.hh"
#include "./selector.hh"

EventLoop::EventLoop()
    : m_terminate(false), m_selector(new Selector) {}

auto EventLoop::run() -> void {
    std::vector<Channel*> p_channels;
    while (!m_terminate) {
        m_selector->poll(p_channels);
        for (auto p_channel : p_channels) {
            p_channel->handle_event();
        }
        p_channels.clear();
    }
}

auto EventLoop::update(Channel* channel) -> void {
    m_selector->update(channel);
}