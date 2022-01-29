#ifndef MINI_MUDUO_RUNNER
#define MINI_MUDUO_RUNNER

#include "runnable.hh"

class Runner {
public:
  Runner(Runnable *p_runnable, void *p_param)
      : mp_runnable(p_runnable), mp_param(p_param) {}
  ~Runner() = default;

  auto run() -> void { mp_runnable->run(mp_param); }

private:
  Runnable *mp_runnable;
  void *mp_param;
};

#endif