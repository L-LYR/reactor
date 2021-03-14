#ifndef MINI_MODUO_TCP_SERVER
#define MINI_MODUO_TCP_SERVER

#include "./def.hh"
#include "./decl.hh"
#include "./acceptor_callback.hh"
#include "./acceptor.hh"

#include <sys/epoll.h>
#include <unordered_map>

class TcpServer : public AcceptorCallback {
  public:
    TcpServer();
    ~TcpServer() = default;

    virtual auto on_connection(int sockfd) -> void;

    auto run() -> void;

  private:
    int m_epfd;
    epoll_event m_events[max_events];
    Acceptor* mp_acceptor;
    std::unordered_map<int, TcpConnectionHandler*> m_connections;
};

#endif