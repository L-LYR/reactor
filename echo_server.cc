#include "./echo_server.hh"

#include <iostream>

#include "./def.hh"
#include "./event_loop.hh"
#include "./timer.hh"

EchoServer::EchoServer(EventLoop* event_loop)
    : m_tcp_server_base(event_loop),
      mp_event_loop(event_loop),
      m_timer(nullptr),
      m_index(0) {
    m_tcp_server_base.set_callback(this);
}

auto EchoServer::start() -> void {
    m_tcp_server_base.start();
}

auto EchoServer::on_connection(TcpConnection* tcp_connection) -> void {
    std::cout << "Connection Established..." << std::endl;
}

auto EchoServer::on_message(TcpConnection* tcp_connection, Buffer& buffer) -> void {
    std::cout << "Message Received..." << std::endl;
    echo(tcp_connection, buffer);
    if (m_timer == nullptr) {
        m_timer = mp_event_loop->run_every(0.5, this);
    }
}

auto EchoServer::on_write_done(TcpConnection* tcp_connection) -> void {
    std::cout << "Echo done..." << std::endl;
}

auto EchoServer::echo(TcpConnection* tcp_connection, Buffer& buffer) -> void {
    std::cout << "Echoing..." << std::endl;
    while (buffer.readable_bytes() > signal_length) {
        m_request_parser.parse(buffer.readable_begin(), buffer.readable_bytes());
        buffer.retrieve_all();
    }
    if (m_request_parser.generate_response()) {
        tcp_connection->send(m_request_parser.get_response(),
                             m_request_parser.get_response_length());
    } else {
        std::cout << "Fail to response..." << std::endl;
    }
    m_request_parser.reset();
}

auto EchoServer::run(void* param) -> void {
    std::cout << m_index << std::endl;
    if (m_index++ == 3) {
        mp_event_loop->cancel_timer(m_timer);
        m_index = 0;
        m_timer = nullptr;
    }
}