/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_TASKS_H__
#define __EQ_TASKS_H__

#include "eq_config.h"
#include "eq_interrupt_lock.h"

#define _TASK_OO_CALLBACKS
#define _TASK_PRIORITY
#define _TASK_WDT_IDS
#include <TaskSchedulerDeclarations.h>

enum class EqTaskId : uint8_t {
  Heartbeat = 10,
  ItSensorControl = 20,
  HtSensorControl = 30,
  FanControl = 40,
  ButtonControl = 50,
  Debug = 255
};

template <EqTaskId Id> class EqTask;
using EqTaskHeartbeat = EqTask<EqTaskId::Heartbeat>;
using EqTaskItSensorControl = EqTask<EqTaskId::ItSensorControl>;
using EqTaskHtSensorControl = EqTask<EqTaskId::HtSensorControl>;
using EqTaskFanControl = EqTask<EqTaskId::FanControl>;
using EqTaskButtonControl = EqTask<EqTaskId::ButtonControl>;
#ifdef EQ_DEBUG
using EqTaskDebug = EqTask<EqTaskId::Debug>;
#endif

template <EqTaskId Id> class EqTask : public Task {
  friend EqTaskHeartbeat &eqTaskHeartbeat();
  friend EqTaskItSensorControl &eqTaskItSensorControl();
  friend EqTaskHtSensorControl &eqTaskHtSensorControl();
  friend EqTaskFanControl &eqTaskFanControl();
  friend EqTaskButtonControl &eqTaskButtonControl();
#ifdef EQ_DEBUG
  friend EqTaskDebug &eqTaskDebug();
#endif

public:
  void setWdPoint(uint8_t point);
  EqTask(const EqTask &) = delete;
  EqTask(EqTask &&) = delete;
  void operator=(const EqTask &) = delete;

private:
  EqTask();
  bool Callback();
  static EqTask instance_;
};

inline EqTaskHeartbeat &eqTaskHeartbeat() { return EqTaskHeartbeat::instance_; }
inline EqTaskItSensorControl &eqTaskItSensorControl() {
  return EqTaskItSensorControl::instance_;
}
inline EqTaskHtSensorControl &eqTaskHtSensorControl() {
  return EqTaskHtSensorControl::instance_;
}
inline EqTaskFanControl &eqTaskFanControl() {
  return EqTaskFanControl::instance_;
}
inline EqTaskButtonControl &eqTaskButtonControl() {
  return EqTaskButtonControl::instance_;
}
#ifdef EQ_DEBUG
inline EqTaskDebug &eqTaskDebug() { return EqTaskDebug::instance_; }
#endif

template <EqTaskId Id> void EqTask<Id>::setWdPoint(uint8_t point) {
#ifdef EQ_DEBUG
  EqInterruptLock __lock;
  setControlPoint(static_cast<unsigned int>(Id) + point);
#endif
}

#endif // __EQ_TASKS_H__
