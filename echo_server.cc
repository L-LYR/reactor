#include "./echo_server.hh"

#include "./def.hh"
#include "./event_loop.hh"
#include "./logger.hh"
#include "./timer.hh"

EchoServer::EchoServer(EventLoop *event_loop)
    : m_tcp_server_base(event_loop), qps(5.0) {
  m_tcp_server_base.set_callback(this);
  event_loop->run_every(qps.interval, this);
}

auto EchoServer::start() -> void { m_tcp_server_base.start(); }

auto EchoServer::on_connection(TcpConnection *_) -> void {
  info("Connection Established...\n");
}

auto EchoServer::on_message(TcpConnection *tcp_connection, Buffer &buffer)
    -> void {
  info("Message Received...\n");
  echo(tcp_connection, buffer);
  qps.inc();
}

auto EchoServer::on_write_done(TcpConnection *tcp_connection) -> void {
  info("Done...\n");
}

auto EchoServer::echo(TcpConnection *tcp_connection, Buffer &buffer) -> void {
  info("Echo...\n");
  while (buffer.readable_bytes() > signal_length) {
    m_echo_service.parse(buffer.readable_begin(), buffer.readable_bytes());
    buffer.retrieve_all();
  }
  if (m_echo_service.generate_response()) {
    tcp_connection->send(m_echo_service.get_response(),
                         m_echo_service.get_response_length());
  } else {
    error("Fail to response...\n");
  }
  m_echo_service.reset();
}

auto EchoServer::run(void *param) -> void {
  info("QPS: %lf\n", qps.result());
  qps.reset();
}