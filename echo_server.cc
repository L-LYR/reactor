#include "./echo_server.hh"
#include "./event_loop.hh"

#include <iostream>

EchoServer::EchoServer(EventLoop* event_loop)
    : mp_event_loop(event_loop), m_tcp_server_base(event_loop) {
    m_tcp_server_base.set_callback(this);
}

auto EchoServer::run() -> void {
    m_tcp_server_base.run();
}

auto EchoServer::on_connection(TcpConnection* tcp_connection) -> void {
    std::cout << "Connection Established..." << std::endl;
}

auto EchoServer::on_message(TcpConnection* tcp_connection, const std::string& data) -> void {
    std::cout << "Message Received..." << std::endl;

    echo(tcp_connection, data);
}

auto EchoServer::echo(TcpConnection* tcp_connection, const std::string& data) -> void {
    m_request_parser.parse(data.c_str(), data.length());
    if (m_request_parser.generate_response()) {
        tcp_connection->write(m_request_parser.get_response());
    }
    std::cout << "Echo..." << std::endl;
    m_request_parser.reset();
}