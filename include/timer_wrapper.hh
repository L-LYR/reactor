#ifndef MINI_MUDUO_TIMER_WRAPPER
#define MINI_MUDUO_TIMER_WRAPPER

#include "decl.hh"
#include "runnable.hh"
#include "timer_queue.hh"

class AddTimerWrapper : public Runnable {
public:
  AddTimerWrapper(TimerQueue *p_queue) : mp_queue(p_queue) {}

  virtual auto run(void *param) -> void { mp_queue->add_timer(param); }

private:
  TimerQueue *mp_queue;
};

class RemoveTimerWrapper : public Runnable {
public:
  RemoveTimerWrapper(TimerQueue *p_queue) : mp_queue(p_queue) {}

  virtual auto run(void *param) -> void { mp_queue->remove_timer(param); }

private:
  TimerQueue *mp_queue;
};

#endif