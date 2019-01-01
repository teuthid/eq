
#ifndef __EQ_TASKS_H__
#define __EQ_TASKS_H__

#define _TASK_OO_CALLBACKS
#define _TASK_PRIORITY
#define _TASK_WDT_IDS
#include <TaskSchedulerDeclarations.h>

#include "eq_config.h"
#include "eq_fixedpoints.h"

enum class EqTaskId : uint8_t {
  Heartbeat = 0x00,
  ItSensorControl = 0x10,
  HtSensorControl = 0x20,
  FanControl = 0x30,
  ButtonControl = 0x40
};

template <EqTaskId Id> class EqTask : public Task {
public:
  static EqTask &instance() {
    static EqTask instance;
    return instance;
  }
  EqTask(const EqTask &) = delete;
  void operator=(const EqTask &) = delete;

private:
  EqTask();
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

template <>
void EqDebugTask::print_(const __FlashStringHelper *description,
                         const fixed_t &value);

#endif // EQ_DEBUG

#endif // __EQ_TASKS_H__
