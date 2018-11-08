
//#include "Arduino.h"
#include <avr/sleep.h>

#include "eq_button.h"
#include "eq_display.h"
#include "eq_eeprom.h"
#include "eq_fan_pwm.h"
#include "eq_ht_sensor.h"
#include "eq_led.h"
#include "eq_light_sensor.h"
#include "eq_temp_sensor.h"

#ifdef EQ_DEBUG
#define _TASK_DEBUG
#include "MemoryFree.h"
#endif // EQ_DEBUG

#include <TaskScheduler.h>

EqLed<EqConfig::ledHeartbeatPin> eqLedHeartbeat;
EqLed<EqConfig::ledAlertPin> eqLedAlert;
EqLightSensor eqLightSensor;
EqHtSensor<EQ_HT_SENSOR_TYPE> eqHtSensor;
EqTempSensor eqItSensor(EqConfig::itSensorPin);
EqDisplay<EQ_DISPLAY_TYPE> eqDisplay;
EqButton<EqConfig::buttonOverdrivePin> eqButtonOverdrive;
EqButton<EqConfig::buttonBacklightPin> eqButtonBacklight;
EqFanPwm eqFanPwm;

// tasks calbacks
void eqHeartbeatCallback();
void eqITMeasurementCallback();
void eqHTMeasurementCallback();
void eqFanPwmControlCallback();

Scheduler eqRunner;

// tasks
Task eqHeartbeat(TASK_SECOND, TASK_FOREVER, &eqHeartbeatCallback, &eqRunner,
                 false);
Task eqITMeasurement(TASK_SECOND, TASK_FOREVER, &eqITMeasurementCallback,
                     &eqRunner, false);
Task eqHTMeasurement(EqConfig::htSensorInterval() * TASK_SECOND, TASK_FOREVER,
                     &eqHTMeasurementCallback, &eqRunner, false);
Task eqFanPwmControl(EqConfig::fanPwmInterval() * TASK_SECOND, TASK_FOREVER,
                     &eqFanPwmControlCallback, &eqRunner, false);
Task eqButtonControl(EqConfig::buttonReadInterval *TASK_MILLISECOND,
                     TASK_FOREVER,
                     []() {
                       eqButtonBacklight.read();
                       eqButtonOverdrive.read();
                     },
                     &eqRunner, false);

bool eqInit() {
  EqConfig::init();
  if (!eqDisplay.init())
    return false;
  eqLedHeartbeat.test(200, 3);
  eqLedAlert.test(200, 3);
  if (!eqLightSensor.init())
    return false;
  if (!eqItSensor.init(true))
    return false;
  if (!eqHtSensor.init())
    return false;
  eqButtonOverdrive.init();
  eqButtonBacklight.init();
  eqFanPwm.init();
  if (EqConfig::isFanTachometerEnabled())
    if (!eqFanPwm.calibrateTachometer(
            [](uint8_t percents) { eqDisplay.showCalibrating(percents); }))
      return false;
  //
  return true;
}

#ifdef EQ_DEBUG
void printConfig() {
  Serial.println();
  Serial.println(F("Configuration:"));
  Serial.print(F("- EEPROM: "));
  for (uint16_t __i = EqEeprom::startAddress;
       __i <= EqEeprom::startAddress + 50; __i++) {
    Serial.print(EEPROM.read(__i), HEX);
    Serial.print(F(" "));
  }
  Serial.println();
  Serial.print(F("- Light Sensor Threshold [%] = "));
  Serial.println(EqConfig::lightSensorThreshold());
  Serial.print(F("- HT Sensor Interval [s] = "));
  Serial.println(EqConfig::htSensorInterval());
  Serial.print(F("- HT Sensor Temperature Threshold [*C] = "));
  Serial.println(EqConfig::htSensorTemperatureThreshold(), 1);
  Serial.print(F("- HT Sensor Humidity Threshold [%] = "));
  Serial.println(EqConfig::htSensorHumidityThreshold());
  Serial.print(F("- HT Index Type = "));
  Serial.println(static_cast<uint8_t>(EqConfig::htIndexType()));
  Serial.print(F("- Overdrive Step [s] = "));
  Serial.println(EqConfig::overdriveStep());
  Serial.print(F("- Fan Tachometer = "));
  Serial.println(EqConfig::isFanTachometerEnabled() ? F("Enabled")
                                                    : F("Disabled"));
  // TODO
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
    eqHeartbeat.enable();
    eqButtonControl.enable();
    eqITMeasurement.enable();
    eqHTMeasurement.enable();
    eqFanPwmControl.enable();
    eqRunner.startNow();
  } else {
    eqLedAlert.setState(true);
    if (EqConfig::alert() != EqAlertType::Display)
      eqDisplay.showAlert();
#ifdef EQ_DEBUG
    Serial.println();
    Serial.print(F("- ALERT: "));
    Serial.println(EqConfig::alertAsString());
    Serial.flush();
#endif
    abort();
  }
#ifdef EQ_DEBUG
  //printConfig();
  Serial.println();
#endif
}

void eqHeartbeatCallback() {
  eqLightSensor.read();
  if (EqConfig::anyAlert())
    eqLedAlert.toggle(true); // force blinking led
  else
    eqLedAlert.setState(false);
  eqLedHeartbeat.toggle(true);
  eqDisplay.show(eqHtSensor.lastHumidity(), eqHtSensor.lastTemperature(),
                 eqHtSensor.trendHumidity(), eqHtSensor.trendTemperature(),
                 eqFanPwm.lastSpeed());
  EqConfig::decreaseOverdriveTime();
  EqConfig::decreaseBacklightTimeCounter();
}

void eqSleep() {
  eqRunner.disableAll();
  if (EqConfig::isFanTachometerEnabled())
    detachInterrupt(digitalPinToInterrupt(EqConfig::fanTachometerPin));
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(EqConfig::buttonOverdrivePin), []() {},
                  LOW);
  sei();
  eqLedHeartbeat.setState(false);
  eqLedAlert.setState(true);
  sleep_mode();
  // executed after the interrupt:
  sleep_disable();
  EqConfig::reset();
}

void eqITMeasurementCallback() {
  if (!eqItSensor.read(true))
    EqConfig::setAlert(EqAlertType::ItSensor);
  else {
    EqConfig::resetAlert(EqAlertType::ItSensor);
    if (eqItSensor.temperature() > EqConfig::itSensorMaxTemperature) {
      EqConfig::setAlert(EqAlertType::Overheating);
      EqConfig::registerOverheating();
      if (EqConfig::overheating())
        eqSleep();
    } else
      EqConfig::resetAlert(EqAlertType::Overheating);
  }
}

void eqHTMeasurementCallback() {
  if (!eqHtSensor.read())
    EqConfig::setAlert(EqAlertType::HtSensor);
  else
    EqConfig::resetAlert(EqAlertType::HtSensor);
#ifdef EQ_DEBUG
  Serial.print(F(" L="));
  Serial.print(eqLightSensor.intensity());
/*
Serial.print(F(" H="));
Serial.print(eqHtSensor.humidity());
Serial.print(F(" T="));
Serial.print(eqHtSensor.temperature());
*/
  /*
Serial.print(F(" HTD="));
Serial.print(eqHtSensor.trendHumidity());
Serial.print(F(" HTT="));
Serial.print(eqHtSensor.trendTemperature());
Serial.print(F(" HTI="));
Serial.print(eqHtSensor.index());
Serial.print(F(" OT[s]="));
Serial.print(EqConfig::overdriveTime());
*/
  Serial.print(F(" PWM="));
  Serial.print(eqFanPwm.dutyCycle());
  if (EqConfig::isFanTachometerEnabled()) {
    Serial.print(F(" FS="));
    Serial.print(eqFanPwm.lastSpeed());
  }
  Serial.print(F(" IT="));
  Serial.print(eqItSensor.lastTemperature(), 1);
  Serial.print(F(" FreeMem="));
  Serial.print(freeMemory());
  //
  Serial.println();
#endif
}

void eqFanPwmControlCallback() {
  // setting pwm:
  if (!EqConfig::anyAlert())
    if (EqConfig::overdriveTime() > 0)
      eqFanPwm.setOverdrive();
    else
      eqFanPwm.setDutyCycle(eqHtSensor.index());
  else {
    eqFanPwm.stop();
  }
  // reading fan speed:
  if (!eqFanPwm.readSpeed())
    EqConfig::setAlert(EqAlertType::Fan);
  else
    EqConfig::resetAlert(EqAlertType::Fan);
}

void loop() { eqRunner.execute(); }
