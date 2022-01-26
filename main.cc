#include "./decl.hh"
#include "./echo_server.hh"
#include "./event_loop.hh"

auto main() -> int {
  EventLoop event_loop;
  EchoServer echo_server(&event_loop);
  echo_server.start();
  event_loop.loop();
  return 0;
}
