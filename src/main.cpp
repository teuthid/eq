
#include "eq_button.h"
#include "eq_display.h"
#include "eq_eeprom.h"
#include "eq_fan_pwm.h"
#include "eq_ht_sensor.h"
#include "eq_led.h"
#include "eq_light_sensor.h"

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

bool eqInit() {
  EqConfig::init();
  if (!eqDisplay.init())
    return false;
  eqLedHeartbeat.test(200, 3);
  eqLedAlert.test(200, 3);
  if (!eqLightSensor.init())
    return false;
  if (!eqItSensor.init())
    return false;
  if (!eqHtSensor.init())
    return false;
  eqButtonOverdrive.init();
  eqButtonBacklight.init();
  eqFanPwm.init();
  if (EqConfig::isFanTachometerEnabled())
    if (!eqFanPwm.calibrateTachometer())
      return false;
  return true;
}

#ifdef EQ_DEBUG
void printConfig() {
  Serial.println();
  Serial.println(F("Configuration:"));
  EqEeprom::show();
  Serial.print(F(" Light Sensor Threshold = "));
  Serial.println(EqConfig::lightSensorThreshold());
  Serial.print(F(" HT Sensor Interval = "));
  Serial.println(EqConfig::htSensorInterval());
  Serial.print(F(" HT Sensor Temperature Threshold = "));
  Serial.println(EqConfig::htSensorTemperatureThreshold(), 1);
  Serial.print(F(" HT Sensor Humidity Threshold = "));
  Serial.println(EqConfig::htSensorHumidityThreshold());
  Serial.print(F(" HT Index Type = "));
  Serial.println(static_cast<uint8_t>(EqConfig::htIndexType()));
  Serial.print(F(" Overdrive Step = "));
  Serial.println(EqConfig::overdriveStep());
  Serial.print(F(" Fan Tachometer = "));
  Serial.println(EqConfig::isFanTachometerEnabled() ? F("Enabled")
                                                    : F("Disabled"));
  Serial.println(F("Running..."));
}
#endif // EQ_DEBUG

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
#ifdef EQ_DEBUG
  Serial.print(F("Initializing... "));
#endif
  if (eqInit()) {
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
    eqLedAlert.setState(true);
    if (EqConfig::alert() != EqAlertType::Display)
      eqDisplay.showAlert();
#ifdef EQ_DEBUG
    Serial.println();
    Serial.print(F("ALERT: "));
    Serial.println(EqConfig::alertAsString());
    Serial.flush();
#endif
    abort();
  }
#ifdef EQ_DEBUG
  printConfig();
#endif
}

void loop() { eqRunner.execute(); }
