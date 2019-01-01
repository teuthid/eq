
#ifdef EQ_DEBUG
#define _TASK_DEBUG
#endif

#include "eq_tasks.h"
#include <TaskScheduler.h>

Scheduler eqRunner;
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
    eqRunner.addTask(EqTask<EqTaskId::Heartbeat>::instance());
    // taskItSensorControl.enable();
    // taskButtonControl.enable();
    // taskHtSensorControl.enable();
    // taskFanControl.enable();
#ifdef EQ_DEBUG
    // taskDebug.enable();
#endif
    eqRunner.enableAll();
#ifdef EQ_DEBUG
    Serial.println();
    EqConfig::show();
    Serial.println(F("Running..."));
#endif
    eqRunner.startNow();
  } else {
    EqConfig::showAlert(EqConfig::alert());
    abort();
  }
}

void loop() { eqRunner.execute(); }
