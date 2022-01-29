#include "tcp_server_base.hh"

#include "acceptor.hh"
#include "def.hh"
#include "event_loop.hh"
#include "tcp_connection.hh"

TcpServerBase::TcpServerBase(EventLoop *event_loop)
    : mp_event_loop(event_loop), mp_acceptor(new Acceptor(event_loop)) {}

TcpServerBase::~TcpServerBase() {
  delete mp_acceptor;
  for (auto entry : m_connections) {
    delete entry.second;
  }
};

auto TcpServerBase::on_connection(int connfd) -> void {
  auto tcp_connection = new TcpConnection(mp_event_loop, connfd);
  m_connections[connfd] = tcp_connection;
  tcp_connection->regester_in(mp_server);
  tcp_connection->establish();
}

auto TcpServerBase::run() -> void {
  mp_acceptor->set_callback(this);
  mp_acceptor->run();
}

auto TcpServerBase::set_callback(Server *server) -> void { mp_server = server; }