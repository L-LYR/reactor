#ifndef MINI_MUDUO_ECHO_SERVER
#define MINI_MUDUO_ECHO_SERVER

#include <string>

#include "echo_service.hh"
#include "server.hh"
#include "tcp_server_base.hh"
#include "timer_queue.hh"
#include "event_loop.hh"

class QPS {
  friend class EchoServer;
  int count;
  double interval; // second

  explicit QPS(double interval) : count(0), interval(interval) {}

  auto result() -> double { return count / interval; }
  auto reset() -> void { count = 0; }
  auto inc() -> void { count++; }
};

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
  EchoService m_echo_service;

  QPS qps;
};

#endif