
#ifndef __EQ_TASKS_H__
#define __EQ_TASKS_H__

#define _TASK_OO_CALLBACKS
#include <TaskSchedulerDeclarations.h>

#include "eq_config.h"

class EqHeartbeat : public Task {
public:
  EqHeartbeat(Scheduler *scheduler);
  bool Callback();
};

class EqButtonControl : public Task {
public:
  EqButtonControl(Scheduler *scheduler);
  bool Callback();
};

class EqHtSensorControl : public Task {
public:
  EqHtSensorControl(Scheduler *scheduler);
  bool Callback();
};

class EqItSensorControl : public Task {
public:
  EqItSensorControl(Scheduler *scheduler);
  bool Callback();

private:
  Scheduler *runner_;
};

class EqFanControl : public Task {
public:
  EqFanControl(Scheduler *scheduler);
  bool Callback();
};

#ifdef EQ_DEBUG
class EqDebugTask : public Task {
public:
  EqDebugTask(Scheduler *scheduler);
  bool Callback();

private:
  template <typename Value>
  void print_(const __FlashStringHelper *description, const Value &value);
};

template <typename Value>
void EqDebugTask::print_(const __FlashStringHelper *description,
                         const Value &value) {
  Serial.print(description);
  Serial.print(value);
}
#endif // EQ_DEBUG

#endif // __EQ_TASKS_H__
