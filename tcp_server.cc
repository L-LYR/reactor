#include "./def.hh"
#include "./acceptor.hh"
#include "./event_loop.hh"
#include "./tcp_server.hh"
#include "./tcp_connection_handler.hh"

TcpServer::TcpServer()
    : mp_acceptor(nullptr), mp_event_loop(new EventLoop) {}

auto TcpServer::on_connection(int connfd) -> void {
    m_connections[connfd] = new TcpConnectionHandler(mp_event_loop, connfd);
}

auto TcpServer::run() -> int {
    mp_acceptor = new Acceptor(mp_event_loop);
    mp_acceptor->set_callback(this);
    mp_acceptor->run();

    mp_event_loop->run();

    return 0;
}