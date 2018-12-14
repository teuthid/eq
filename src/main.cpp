
#ifdef EQ_DEBUG
#define _TASK_DEBUG
#endif

#include "eq_tasks.h"
#include <TaskScheduler.h>

Scheduler eqRunner;
EqHeartbeat taskHeartbeat(&eqRunner);
EqButtonControl taskButtonControl(&eqRunner);
EqHtSensorControl taskHtSensorControl(&eqRunner);
EqItSensorControl taskItSensorControl(&eqRunner);
EqFanControl taskFanControl(&eqRunner);
#ifdef EQ_DEBUG
EqDebugTask taskDebug(&eqRunner);
#endif

void setup() {
  Serial.begin(115200);
#ifdef EQ_DEBUG
  Serial.print(F("Initializing... "));
#endif
  if (EqConfig::init()) {
    taskItSensorControl.enable();
    taskHeartbeat.enable();
    taskButtonControl.enable();
    taskHtSensorControl.enable();
    taskFanControl.enable();
#ifdef EQ_DEBUG
    taskDebug.enable();
#endif
    eqRunner.startNow();
  } else {
    EqConfig::showAlert(EqConfig::alert());
    abort();
  }
#ifdef EQ_DEBUG
  Serial.println();
  EqConfig::show();
  Serial.println(F("Running..."));
#endif
}

void loop() { eqRunner.execute(); }
