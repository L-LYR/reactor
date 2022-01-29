#ifndef MINI_MUDUO_TCP_SERVER_BASE
#define MINI_MUDUO_TCP_SERVER_BASE

#include <unordered_map>

#include "acceptor.hh"
#include "callback.hh"
#include "tcp_connection.hh"

class TcpServerBase : public AcceptorCallback {
public:
  explicit TcpServerBase(EventLoop *event_loop);
  ~TcpServerBase();

  virtual auto on_connection(int sockfd) -> void override;

  auto run() -> void;

  auto set_callback(Server *server) -> void;

private:
  Server *mp_server;
  EventLoop *mp_event_loop;
  Acceptor *mp_acceptor; // own
  std::unordered_map<int, TcpConnection *> m_connections;
};

#endif