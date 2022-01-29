#include "echo_server.hh"

#include "logger.hh"

EchoServer::EchoServer() : m_tcp_server_base(&m_event_loop), m_qps(5.0) {
  m_tcp_server_base.set_callback(this);
  m_event_loop.run_every(m_qps.interval, this);
}

auto EchoServer::run() -> void {
  m_tcp_server_base.run();
  m_event_loop.loop();
}

auto EchoServer::on_connection(TcpConnection *_) -> void {
  info("Connection Established...\n");
}

auto EchoServer::on_message(TcpConnection *tcp_connection, Buffer &buffer)
    -> void {
  info("Message Received...\n");
  echo(tcp_connection, buffer);
  m_qps.inc();
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
  if (m_echo_service.get_kill_sig()) {
    m_event_loop.stop();
  }
  m_echo_service.reset();
}

auto EchoServer::run(void *param) -> void {
  info("QPS: %lf\n", m_qps.result());
  m_qps.reset();
}