#ifndef MINI_MUDUO_TCP_SERVER_BASE
#define MINI_MUDUO_TCP_SERVER_BASE

#include <unordered_map>

#include "./callback.hh"
#include "./decl.hh"
#include "./def.hh"

class TcpServerBase : public AcceptorCallback {
public:
  explicit TcpServerBase(EventLoop *event_loop);
  ~TcpServerBase() = default;

  virtual auto on_connection(int sockfd) -> void override;

  auto start() -> void;

  auto set_callback(Server *server) -> void;

private:
  Server *mp_server;
  Acceptor *mp_acceptor;
  EventLoop *mp_event_loop;
  std::unordered_map<int, TcpConnection *> m_connections;
};

#endif