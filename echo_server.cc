#include "./echo_server.hh"

#include "./def.hh"
#include "./event_loop.hh"
#include "./logger.hh"
#include "./timer.hh"

EchoServer::EchoServer(EventLoop *event_loop)
    : m_tcp_server_base(event_loop), mp_event_loop(event_loop),
      m_timer(nullptr), m_index(0) {
  m_tcp_server_base.set_callback(this);
}

auto EchoServer::start() -> void { m_tcp_server_base.start(); }

auto EchoServer::on_connection(TcpConnection *tcp_connection) -> void {
  info("Connection Established...\n");
}

auto EchoServer::on_message(TcpConnection *tcp_connection, Buffer &buffer)
    -> void {
  info("Message Received...\n");
  echo(tcp_connection, buffer);
  if (m_timer == nullptr) {
    m_timer = mp_event_loop->run_every(0.5, this);
  }
}

auto EchoServer::on_write_done(TcpConnection *tcp_connection) -> void {
  info("Echo done...\n");
}

auto EchoServer::echo(TcpConnection *tcp_connection, Buffer &buffer) -> void {
  info("Echoing...\n");
  while (buffer.readable_bytes() > signal_length) {
    m_request_parser.parse(buffer.readable_begin(), buffer.readable_bytes());
    buffer.retrieve_all();
  }
  if (m_request_parser.generate_response()) {
    tcp_connection->send(m_request_parser.get_response(),
                         m_request_parser.get_response_length());
  } else {
    error("Fail to response...\n");
  }
  m_request_parser.reset();
}

auto EchoServer::run(void *param) -> void {
  info("timer count: %d\n", m_index);
  if (m_index++ == 3) {
    mp_event_loop->cancel_timer(m_timer);
    m_index = 0;
    m_timer = nullptr;
  }
}