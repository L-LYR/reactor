#ifndef MINI_MUDUO_RUNNABLE
#define MINI_MUDUO_RUNNABLE

class Runnable {
  public:
    virtual auto run(void* param) -> void = 0;
};

#endif