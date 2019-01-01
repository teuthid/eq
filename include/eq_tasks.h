
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
  ButtonControl = 0x40,
  Debug = 0xFF
};

template <EqTaskId Id> class EqTask : public Task {
public:
  static EqTask &instance() {
    static EqTask instance;
    return instance;
  }
  EqTask(const EqTask &) = delete;
  EqTask(EqTask &&) = delete;
  void operator=(const EqTask &) = delete;

private:
  EqTask();
  bool Callback();
};

#endif // __EQ_TASKS_H__
