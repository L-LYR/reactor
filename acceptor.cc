#include "./acceptor.hh"

#include <arpa/inet.h>
#include <errno.h>
#include <sys/fcntl.h>

#include "./channel.hh"
#include "./logger.hh"

Acceptor::Acceptor(EventLoop *event_loop)
    : mp_event_loop(event_loop), m_listenfd(-1), mp_channel(nullptr),
      mp_acceptor_callback(nullptr) {}

auto Acceptor::run() -> void {
  bind_and_listen();
  mp_channel = new Channel(mp_event_loop, m_listenfd);
  mp_channel->set_callback(this);
  mp_channel->enable_read();
}

auto Acceptor::set_callback(AcceptorCallback *p_acceptor_callback) -> void {
  mp_acceptor_callback = p_acceptor_callback;
}

auto Acceptor::handle_read() -> void {
  sockaddr_in client_addr;
  socklen_t client_length = sizeof(sockaddr_in);
  int connfd = ::accept(m_listenfd, (sockaddr *)&client_addr,
                        (socklen_t *)&client_length);
  if (connfd < 0) {
    error("error in accept(), connfd: %d, errno: %d\n", connfd, errno);
    return;
  }

  info("new connection from [%s:%d], new socket fd: %d\n",
       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), connfd);
  fcntl(connfd, F_SETFL, O_NONBLOCK);

  mp_acceptor_callback->on_connection(connfd);
}

auto Acceptor::handle_write() -> void {}

auto Acceptor::bind_and_listen() -> void {
  // listen socket
  m_listenfd = ::socket(AF_INET, SOCK_STREAM, 0); // TCP
  if (m_listenfd < 0) {
    error("error in socket(), errno: %d\n", errno);
  }
  if (-1 == ::fcntl(m_listenfd, F_SETFL, O_NONBLOCK)) {
    error("error in fcntl(), errno: %d\n", errno);
  } // non-block io

  int on = 1;
  if (-1 ==
      ::setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
    error("error in setsockopt(), errno: %d\n", errno);
  } // reusable addr in bind

  // bind
  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
  server_addr.sin_port = ::htons(10086);
  if (-1 == ::bind(m_listenfd, (sockaddr *)&server_addr, sizeof(server_addr))) {
    error("error in bind(), errno: %d\n", errno);
  }
  // listen
  if (-1 == ::listen(m_listenfd, max_listen_fd)) {
    error("error in listen(), errno: %d\n", errno);
  }
  // show info
  info("bind addr: [%s:%d], listening fd: %d\n",
       inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port),
       m_listenfd);
}