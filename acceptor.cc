#include "./acceptor.hh"
#include "./channel.hh"
#include "./callback.hh"

#include <sys/fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <iostream>

Acceptor::Acceptor(EventLoop* event_loop) : mp_event_loop(event_loop),
                                            m_listenfd(-1),
                                            mp_channel(nullptr),
                                            mp_acceptor_callback(nullptr) {}

auto Acceptor::run() -> void {
    bind_and_listen();
    mp_channel = new Channel(mp_event_loop, m_listenfd);
    mp_channel->set_callback(this);
    mp_channel->enable_read();
}

auto Acceptor::set_callback(AcceptorCallback* p_acceptor_callback) -> void {
    mp_acceptor_callback = p_acceptor_callback;
}

auto Acceptor::on_in(int sockfd) -> void {
    sockaddr_in client_addr;
    socklen_t client_length = sizeof(sockaddr_in);
    int connfd = accept(m_listenfd, (sockaddr*)&client_addr, (socklen_t*)&client_length);
    if (connfd < 0) {
        std::cout << "error in accept(), connfd: " << connfd << " errno: " << errno << std::endl;
        return;
    }
    std::cout << "new connection from [" << inet_ntoa(client_addr.sin_addr)
              << ":" << ntohs(client_addr.sin_port) << "]"
              << " new socket fd: " << connfd << std::endl;

    fcntl(connfd, F_SETFL, O_NONBLOCK);

    mp_acceptor_callback->on_connection(connfd);
}

auto Acceptor::bind_and_listen() -> void {
    // listen socket
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);  // TCP
    if (m_listenfd < 0) {
        std::cout << "error in socket(), errno: " << errno << std::endl;
    }
    if (-1 == fcntl(m_listenfd, F_SETFL, O_NONBLOCK)) {
        std::cout << "error in fcntl()" << std::endl;
    }  // non-block io

    int on = 1;
    if (-1 == setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
        std::cout << "error in setsockopt(), errno: " << errno << std::endl;
    }  // reusable addr in bind

    // bind
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(10086);
    if (-1 == bind(m_listenfd, (sockaddr*)&server_addr, sizeof(server_addr))) {
        std::cout << "error in bind(), errno: " << errno << std::endl;
    }
    // listen
    if (-1 == listen(m_listenfd, max_listen_fd)) {
        std::cout << "error in listen(), errno: " << errno << std::endl;
    }
    // show info
    std::cout << "bind result: "
              << "[" << inet_ntoa(server_addr.sin_addr) << ":"
              << ntohs(server_addr.sin_port) << "]\n"
              << "listen fd: " << m_listenfd
              << std::endl;
}