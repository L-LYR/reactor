#ifndef MINI_MODUO_CHANNEL_CALLBACK
#define MINI_MODUO_CHANNEL_CALLBACK

#include "./decl.hh"

class ChannelCallback {
  public:
    virtual auto on_in(int sockfd) -> void{};
};

#endif