#ifndef MINI_MUDUO_ECHO_SERVER
#define MINI_MUDUO_ECHO_SERVER

#include <string>

#include "./decl.hh"
#include "./echo_service.hh"
#include "./server.hh"
#include "./tcp_server_base.hh"

class EchoServer : public Server, public Runnable {
public:
  EchoServer(EventLoop *event_loop);
  ~EchoServer() = default;

  auto start() -> void;

  virtual auto on_connection(TcpConnection *tcp_connection) -> void;
  virtual auto on_message(TcpConnection *tcp_connection, Buffer &buffer)
      -> void;
  virtual auto on_write_done(TcpConnection *tcp_connection) -> void;

  virtual auto run(void *param) -> void;

private:
  auto echo(TcpConnection *tcp_connection, Buffer &buffer) -> void;

  TcpServerBase m_tcp_server_base;
  EventLoop *mp_event_loop;
  EchoService m_echo_service;

  void *m_timer;
  int m_index;
};

#endif