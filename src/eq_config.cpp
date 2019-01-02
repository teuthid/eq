
#include "eq_config.h"
#include "eq_button.h"
#include "eq_display.h"
#include "eq_dpin.h"
#include "eq_eeprom.h"
#include "eq_fan_pwm.h"
#include "eq_ht_sensor.h"
#include "eq_led.h"
#include "eq_light_sensor.h"

#include <Wire.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#ifdef EQ_DEBUG
#define __ASSERT_USE_STDERR
#include <assert.h>
void __assert(const char *__func, const char *__file, int __lineno,
              const char *__sexp) {
  Serial.println();
  Serial.println(F("- Assertion -"));
  Serial.println(__func);
  Serial.println(__file);
  Serial.println(__lineno, DEC);
  Serial.println(__sexp);
  Serial.flush();
  abort();
}
#endif // EQ_DEBUG

EqAlertType EqConfig::alert_ = EqAlertType::None;
uint16_t EqConfig::overdriveTime_ = 0;
uint16_t EqConfig::backlightTimeCounter_ = 0;

bool EqConfig::init() {
  EqEeprom::init();
  cancelOverheating();
  EqDPin<fanTachometerControlPin>::setInputPulledUp();
  backlightTimeCounter_ = backlightTime();
  Wire.begin();
  if (!eqDisplay.init())
    return false;
  eqLedHeartbeat.test(200, 3);
  eqLedAlert.test(200, 3);
  if (!EqLightSensor::instance().init())
    return false;
  if (!eqItSensor.init())
    return false;
  if (!eqHtSensor.init())
    return false;
  eqButtonOverdrive.init();
  eqButtonBacklight.init();
  if (!eqFanPwm.init())
    return false;
  return true;
}

void EqConfig::reset(const bool &cleanEeprom) {
  EqEeprom::init(cleanEeprom);
  wdt_enable(WDTO_15MS);
  while (true) {
  }
}

void EqConfig::sleep() {
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

void EqConfig::show() {
  Serial.println(F("Configuration:"));
  EqEeprom::show();
  Serial.print(EqConfig::alertAsString(EqAlertType::HtSensor));
  printValue(F("I="), EqConfig::htSensorInterval());
  printValue(F(" H="), EqConfig::htSensorHumidityThreshold());
  printValue(F(" T="), EqConfig::htSensorTemperatureThreshold());
  printValue(F(" IDX="), static_cast<uint8_t>(EqConfig::htIndexType()));
  Serial.println();
  Serial.print(EqConfig::alertAsString(EqAlertType::LightSensor));
  Serial.print(EqConfig::lightSensorThreshold());
  printValue(F("  Overdrive "), EqConfig::overdriveStep());
  printValue(F("  Tacho "),
             EqConfig::isFanTachometerEnabled() ? F("On") : F("Off"));
  Serial.println();
}

void EqConfig::setAlert(const EqAlertType &value) {
  setBacklight(value != alert_);
  alert_ = value;
}

void EqConfig::resetAlert(const EqAlertType &value) {
  alert_ = (alert_ == value) ? EqAlertType::None : alert_;
}

bool EqConfig::isAlertOnZeroSpeed() {
  return EqEeprom::readValue<bool>(EqEeprom::AlertOnZeroSpeed,
                                   alertOnZeroSpeedDefault);
}

void EqConfig::setAlertOnZeroSpeed(const bool &enabled) {
  EqEeprom::writeValue<bool>(EqEeprom::AlertOnZeroSpeed, enabled);
}

namespace {
const char __eqStrAlertNone[] PROGMEM = "None ";
const char __eqStrAlertDisplay[] PROGMEM = "Display ";
const char __eqStrAlertFan[] PROGMEM = "Fan Controller ";
const char __eqStrAlertHtSensor[] PROGMEM = "HT Sensor ";
const char __eqStrAlertLightSensor[] PROGMEM = "Light Sensor ";
const char __eqStrAlertTempSensor[] PROGMEM = "Temp Sensor ";
const char __eqStrAlertItSensor[] PROGMEM = "Internal Sensor ";
const char __eqStrAlertOverheating[] PROGMEM = "Overheating ";
const char *const __eqStrAlerts[] PROGMEM = {
    __eqStrAlertNone,     __eqStrAlertDisplay,     __eqStrAlertFan,
    __eqStrAlertHtSensor, __eqStrAlertLightSensor, __eqStrAlertTempSensor,
    __eqStrAlertItSensor, __eqStrAlertOverheating};
char __eqStrAlertBuffer[17];
} // namespace

const char *EqConfig::alertAsString(const EqAlertType &alert) {
  return strncpy_P(__eqStrAlertBuffer,
                   (char *)pgm_read_word(&(__eqStrAlerts[(uint8_t)alert])),
                   sizeof(__eqStrAlertBuffer));
}

void EqConfig::showAlert(const EqAlertType &alert) {
  eqLedAlert.setState(true);
  if (alert_ != EqAlertType::Display)
    eqDisplay.showAlert(alert);
#ifdef EQ_DEBUG
  Serial.println();
  Serial.print(F("ALERT: "));
  Serial.println(EqConfig::alertAsString(alert));
#endif
}

bool EqConfig::overheating() {
  return (EqEeprom::readValue<uint8_t>(EqEeprom::OverheatingCounter, 0) >
          maxCountOverheating);
}

void EqConfig::registerOverheating() {
  EqEeprom::writeValue<uint8_t>(
      EqEeprom::OverheatingCounter,
      EqEeprom::readValue<uint8_t>(EqEeprom::OverheatingCounter, 0) + 1);
}

void EqConfig::cancelOverheating() {
  EqEeprom::writeValue<uint8_t>(EqEeprom::OverheatingCounter, 0);
}

uint8_t EqConfig::lightSensorThreshold() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::LightSensorThreshold,
                                      lightSensorThresholdDefault);
}

void EqConfig::setLightSensorThreshold(const uint8_t &value) {
  EqEeprom::writeValue<uint8_t>(
      EqEeprom::LightSensorThreshold,
      constrain(value, lightSensorThresholdMin, lightSensorThresholdMax));
}

uint8_t EqConfig::htSensorInterval() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::HtSensorInterval,
                                      htSensorIntervalDefault);
}

void EqConfig::setHtSensorInterval(const uint8_t &value) {
  EqEeprom::writeValue<uint8_t>(
      EqEeprom::HtSensorInterval,
      constrain(value, htSensorIntervalMin, htSensorIntervalMax));
}

uint8_t EqConfig::htSensorHumidityThreshold() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::HtSensorHumidityThreshold,
                                      htSensorHumidityThresholdDefault);
}

void EqConfig::setHtSensorHumidityThreshold(const uint8_t &value) {
  EqEeprom::writeValue<uint8_t>(EqEeprom::HtSensorHumidityThreshold,
                                constrain(value, htSensorHumidityThresholdMin,
                                          htSensorHumidityThresholdMax));
}

uint8_t EqConfig::htSensorTemperatureThreshold() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::HtSensorTemperatureThreshold,
                                      htSensorTemperatureThresholdDefault);
}

void EqConfig::setHtSensorTemperatureThreshold(const uint8_t &value) {
  EqEeprom::writeValue<uint8_t>(EqEeprom::HtSensorTemperatureThreshold,
                                constrain(value,
                                          htSensorTemperatureThresholdMin,
                                          htSensorTemperatureThresholdMax));
}

EqHtIndexType EqConfig::htIndexType() {
  return EqEeprom::readValue<EqHtIndexType>(EqEeprom::HtIndexType,
                                            EqHtIndexType::Default);
}

void EqConfig::setHtIndexType(const EqHtIndexType &value) {
  EqEeprom::writeValue<EqHtIndexType>(EqEeprom::HtIndexType, value);
}

void EqConfig::setOverdriveTime(const uint16_t &value) {
  EqConfig::overdriveTime_ =
      constrain(value, overdriveStepMin, overdriveMaxTime);
}

void EqConfig::decreaseOverdriveTime(const uint16_t &value) {
  overdriveTime_ = (overdriveTime_ > value) ? (overdriveTime_ - value) : 0;
}

void EqConfig::increaseOverdriveTime(const uint16_t &value) {
  if (overdriveTime_ < overdriveMaxTime) {
    overdriveTime_ = min(overdriveTime_ + value, overdriveMaxTime);
    setBacklight();
  }
}

uint16_t EqConfig::overdriveStep() {
  return EqEeprom::readValue<uint16_t>(EqEeprom::OverdriveStep,
                                       overdriveStepDefault);
}

void EqConfig::setOverdriveStep(const uint16_t &value) {
  EqEeprom::writeValue<uint16_t>(
      EqEeprom::OverdriveStep,
      constrain(value, overdriveStepMin, overdriveStepMax));
}

uint8_t EqConfig::fanPwmInterval() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::FanPwmInterval,
                                      fanPwmIntervalDefault);
}

void EqConfig::setFanPwmInterval(const uint8_t &value) {
  EqEeprom::writeValue<uint8_t>(
      EqEeprom::FanPwmInterval,
      constrain(value, fanPwmIntervalMin, fanPwmIntervalMax));
}

uint8_t EqConfig::fanPwmMin() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::FanPwmMin, fanPwmMinDefault);
}

void EqConfig::setFanPwmMin(const uint8_t &value) {
  uint8_t __v = constrain(value, fanPwmMinDefault, fanPwmMaxDefault - 1);
  if (__v < EqConfig::fanPwmMax())
    EqEeprom::writeValue<uint8_t>(EqEeprom::FanPwmMin, __v);
}

uint8_t EqConfig::fanPwmMax() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::FanPwmMax, fanPwmMaxDefault);
}

void EqConfig::setFanPwmMax(const uint8_t &value) {
  uint8_t __v = constrain(value, fanPwmMinDefault + 1, fanPwmMaxDefault);
  if (__v > EqConfig::fanPwmMin())
    EqEeprom::writeValue<uint8_t>(EqEeprom::FanPwmMax, __v);
}

uint8_t EqConfig::fanPwmOverdrive() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::FanPwmOverdrive,
                                      fanPwmOverdriveDefault);
}

void EqConfig::setFanPwmOverdrive(const uint8_t &value) {
  EqEeprom::writeValue<uint8_t>(EqEeprom::FanPwmOverdrive,
                                constrain(value, EqConfig::fanPwmMin(), 100));
}

bool EqConfig::isFanPwmStepModeEnabled() {
  return EqEeprom::readValue<bool>(EqEeprom::FanPwmStepMode,
                                   fanPwmStepModeDefault);
}

void EqConfig::setFanPwmStepMode(const bool &enabled) {
  EqEeprom::writeValue<bool>(EqEeprom::FanPwmStepMode, enabled);
}

bool EqConfig::isFanTachometerEnabled() {
  return EqDPin<EqConfig::fanTachometerControlPin>::isInputHigh();
}

uint16_t EqConfig::backlightTime() {
  return EqEeprom::readValue<uint16_t>(EqEeprom::BacklighTime,
                                       backlightTimeDefault);
}

void EqConfig::setBacklighTime(const uint16_t &value) {
  EqEeprom::writeValue<uint16_t>(
      EqEeprom::BacklighTime,
      constrain(value, backlightTimeMin, backlightTimeMax));
}

void EqConfig::setBacklight(bool enabled) {
  backlightTimeCounter_ = enabled ? backlightTime() : 0;
}

void EqConfig::decreaseBacklightTimeCounter() {
  if (backlightTimeCounter_ > 0)
    backlightTimeCounter_--;
}
