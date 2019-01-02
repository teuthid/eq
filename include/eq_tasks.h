
#ifndef __EQ_TASKS_H__
#define __EQ_TASKS_H__

#define _TASK_OO_CALLBACKS
#define _TASK_PRIORITY
#define _TASK_WDT_IDS
#include <TaskSchedulerDeclarations.h>

#include "eq_config.h"

enum class EqTaskId : uint8_t {
  Heartbeat = 10,
  ItSensorControl = 20,
  HtSensorControl = 30,
  FanControl = 40,
  ButtonControl = 50,
  Debug = 255
};

template <EqTaskId Id> class EqTask : public Task {
public:
  void setWdPoint(uint8_t point);
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

template <EqTaskId Id> void EqTask<Id>::setWdPoint(uint8_t point) {
#ifdef EQ_DEBUG
  setControlPoint(static_cast<unsigned int>(Id) + point);
#endif
}

#endif // __EQ_TASKS_H__
