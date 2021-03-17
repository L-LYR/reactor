#ifndef MINI_MUDUO_CALLBACK
#define MINI_MUDUO_CALLBACK

#include "./decl.hh"

class AcceptorCallback {
  public:
    virtual auto on_connection(int sockfd) -> void = 0;
};

class ChannelCallback {
  public:
    virtual auto on_in(int sockfd) -> void = 0;
};
#endif