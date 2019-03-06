/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include <Wire.h>

#include "eq_button.h"
#include "eq_display.h"
#include "eq_dpin.h"
#include "eq_eeprom.h"
#include "eq_fan_pwm.h"
#include "eq_ht_sensor.h"
#include "eq_led.h"
#include "eq_light_sensor.h"
#include "eq_pwm_timer.h"
#include "eq_tasks.h"

bool EqConfig::watchdogEnabled_{false};
volatile bool EqConfig::saveWatchdogPoint_{true};
EqAlertType EqConfig::alert_{EqAlertType::None};
uint16_t EqConfig::overdriveTimeCounter_{0};
uint16_t EqConfig::backlightTimeCounter_{0};

uint8_t EqConfig::readWatchdogPoint() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::LastWatchdogPoint, 0);
}

void EqConfig::clearWatchdogPoint() {
  EqEeprom::writeValue<uint8_t>(EqEeprom::LastWatchdogPoint, 0);
}

bool EqConfig::init() {
  EqEeprom::init();
  EqDPin<fanTachometerControlPin>::setInputPulledUp();
  backlightTimeCounter_ = backlightTime();
  // analogReadResolution(10); // TODO: for non-AVR architecture
  Wire.begin(); // TO FIX: for ESP32
#if defined(EQ_UNIT_TEST)
  return false;
#endif
  if (!eqDisplay().init())
    return false;
  eqLedHeartbeat().test();
  eqLedAlert().test();
  if (ledStatusEnabled)
    if (isFanTachometerEnabled())
      eqLedStatus().test(); // indicates that the tachometer is active
  if (!eqLightSensor().init())
    return false;
  if (!eqItSensor().init())
    return false;
  if (!eqHtSensor().init())
    return false;
  if (!eqFanPwm().init())
    return false;
  eqButtonBacklight().init();
  eqButtonOverdrive().init();
  eqPwmTimer().attachCallback([]() {
    eqButtonBacklight().read();
    eqButtonOverdrive().read();
  });
  eqDisplay().clear();
  clearOverheating();
  clearWatchdogPoint();
  saveWatchdogPoint_ = true;
  enableWatchdog();
  alert_ = EqAlertType::None;
  overdriveTimeCounter_ = 0;
  setBacklight(); // force backlight at start-up
  return true;
}

void EqConfig::reset(bool cleanEeprom) {
  EqEeprom::init(cleanEeprom);
  saveWatchdogPoint_ = false;
  enableWatchdog();
  while (true) { // waiting for watchdog
  }
}

void EqConfig::showSettings() {
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

bool EqConfig::isAlertOnZeroSpeed() {
  return EqEeprom::readValue<bool>(EqEeprom::AlertOnZeroSpeed,
                                   alertOnZeroSpeedDefault);
}

void EqConfig::setAlertOnZeroSpeed(bool enabled) {
  EqEeprom::writeValue<bool>(EqEeprom::AlertOnZeroSpeed, enabled);
}

namespace {
static const PROGMEM char __eqStrAlertDisplay[] = "Display ";
static const PROGMEM char __eqStrAlertFan[] = "Fan Controller ";
static const PROGMEM char __eqStrAlertHtSensor[] = "HT Sensor ";
static const PROGMEM char __eqStrAlertLightSensor[] = "Light Sensor ";
static const PROGMEM char __eqStrAlertTempSensor[] = "Temp Sensor ";
static const PROGMEM char __eqStrAlertItSensor[] = "Internal Sensor ";
static const PROGMEM char __eqStrAlertOverheating[] = "Overheating ";
static const PROGMEM char *const __eqStrAlerts[] = {
    __eqStrAlertDisplay,     __eqStrAlertFan,        __eqStrAlertHtSensor,
    __eqStrAlertLightSensor, __eqStrAlertTempSensor, __eqStrAlertItSensor,
    __eqStrAlertOverheating};
char __eqStrAlertBuffer[17];
} // namespace

const char *EqConfig::alertAsString(EqAlertType alert) {
  return strncpy_P(__eqStrAlertBuffer,
                   (PGM_P)pgm_read_word(&(__eqStrAlerts[(uint8_t)alert - 1])),
                   sizeof(__eqStrAlertBuffer));
}

void EqConfig::showAlert(EqAlertType alert) {
  eqLedAlert().setState(true);
  if (alert_ != EqAlertType::Display)
    eqDisplay().showAlert();
#if defined(EQ_DEBUG)
  Serial.println();
  Serial.print(F("ALERT: "));
  Serial.println(EqConfig::alertAsString());
  Serial.flush();
#endif
}

EqLedStatusMode EqConfig::ledStatusMode() {
  return EqEeprom::readValue<EqLedStatusMode>(EqEeprom::LedStatusMode,
                                              ledStatusModeDefault);
}

void EqConfig::setLedStatusMode(EqLedStatusMode mode) {
  EqEeprom::writeValue<EqLedStatusMode>(EqEeprom::LedStatusMode, mode);
}

void EqConfig::setLedStatus() {
  if (!EqConfig::anyAlert()) {
    bool __s = false;
    switch (ledStatusMode()) {
    case EqLedStatusMode::LowTemperature:
      __s = (eqHtSensor().lastTemperatureAsLong() <
             htSensorTemperatureThreshold());
      break;
    case EqLedStatusMode::HighTemperatue:
      __s = (eqHtSensor().lastTemperatureAsLong() >
             htSensorTemperatureThreshold());
      break;
    case EqLedStatusMode::LowHumidity:
      __s = (eqHtSensor().lastHumidityAsLong() < htSensorHumidityThreshold());
      break;
    case EqLedStatusMode::HighHumidity:
      __s = (eqHtSensor().lastHumidityAsLong() > htSensorHumidityThreshold());
      break;
    default:
      break;
    }
    eqLedStatus().setState(__s);
  }
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

void EqConfig::clearOverheating() {
  EqEeprom::writeValue<uint8_t>(EqEeprom::OverheatingCounter, 0);
}

uint8_t EqConfig::lightSensorThreshold() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::LightSensorThreshold,
                                      lightSensorThresholdDefault);
}

void EqConfig::setLightSensorThreshold(uint8_t value) {
  EqEeprom::writeValue<uint8_t>(
      EqEeprom::LightSensorThreshold,
      constrain(value, lightSensorThresholdMin, lightSensorThresholdMax));
}

uint8_t EqConfig::htSensorInterval() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::HtSensorInterval,
                                      htSensorIntervalDefault);
}

void EqConfig::setHtSensorInterval(uint8_t value) {
  uint8_t __i = constrain(value, htSensorIntervalMin, htSensorIntervalMax);
  EqEeprom::writeValue<uint8_t>(EqEeprom::HtSensorInterval, __i);
  eqTaskHtSensorControl().setInterval(__i);
}

uint8_t EqConfig::htSensorHumidityThreshold() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::HtSensorHumidityThreshold,
                                      htSensorHumidityThresholdDefault);
}

void EqConfig::setHtSensorHumidityThreshold(uint8_t value) {
  EqEeprom::writeValue<uint8_t>(EqEeprom::HtSensorHumidityThreshold,
                                constrain(value, htSensorHumidityThresholdMin,
                                          htSensorHumidityThresholdMax));
  eqTaskHtSensorControl().forceNextIteration();
}

fixed_t EqConfig::htSensorHumidityCorrection() {
  return fixed_t(
             EqEeprom::readValue<int8_t>(EqEeprom::HtSensorHumidityCorrection,
                                         htSensorHumidityCorrectionDefault)) /
         10;
}

void EqConfig::setHtSensorHumidityCorrection(const fixed_t &value) {
  EqEeprom::writeValue<int8_t>(EqEeprom::HtSensorHumidityCorrection,
                               constrain(fixed_to_int(value * 10),
                                         -htSensorHumidityCorrectionMax,
                                         htSensorHumidityCorrectionMax));
  eqTaskHtSensorControl().forceNextIteration();
}

uint8_t EqConfig::htSensorTemperatureThreshold() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::HtSensorTemperatureThreshold,
                                      htSensorTemperatureThresholdDefault);
}

void EqConfig::setHtSensorTemperatureThreshold(uint8_t value) {
  EqEeprom::writeValue<uint8_t>(EqEeprom::HtSensorTemperatureThreshold,
                                constrain(value,
                                          htSensorTemperatureThresholdMin,
                                          htSensorTemperatureThresholdMax));
  eqTaskHtSensorControl().forceNextIteration();
}

fixed_t EqConfig::htSensorTemperatureCorrection() {
  return fixed_t(EqEeprom::readValue<int8_t>(
             EqEeprom::HtSensorTemperatureCorrection,
             htSensorTemperatureCorrectionDefault)) /
         10;
}

void EqConfig::setHtSensorTemperatureCorrection(const fixed_t &value) {
  EqEeprom::writeValue<int8_t>(EqEeprom::HtSensorTemperatureCorrection,
                               constrain(fixed_to_int(value * 10),
                                         -htSensorTemperatureCorrectionMax,
                                         htSensorTemperatureCorrectionMax));
  eqTaskHtSensorControl().forceNextIteration();
}

EqHtIndexType EqConfig::htIndexType() {
  return EqEeprom::readValue<EqHtIndexType>(EqEeprom::HtIndexType,
                                            EqHtIndexType::Default);
}

void EqConfig::setHtIndexType(EqHtIndexType value) {
  EqEeprom::writeValue<EqHtIndexType>(EqEeprom::HtIndexType, value);
  eqTaskHtSensorControl().forceNextIteration();
}

void EqConfig::setOverdriveTime(uint16_t value) {
  EqConfig::overdriveTimeCounter_ =
      constrain(value, overdriveStepMin, overdriveMaxTime);
}

void EqConfig::decreaseOverdriveTime(uint16_t value) {
  if (value > 0)
    overdriveTimeCounter_ =
        (overdriveTimeCounter_ > value) ? (overdriveTimeCounter_ - value) : 0;
}

void EqConfig::increaseOverdriveTime(uint16_t value, bool backlight) {
  if (value > 0) {
    if (overdriveTimeCounter_ == 0)
      // force overdrive mode for execution immediately
      eqTaskFanControl().forceNextIteration();
    if (overdriveTimeCounter_ < overdriveMaxTime) {
      overdriveTimeCounter_ =
          min(overdriveTimeCounter_ + value, overdriveMaxTime);
      if (backlight)
        setBacklight();
    }
  }
}

uint16_t EqConfig::overdriveStep() {
  return EqEeprom::readValue<uint16_t>(EqEeprom::OverdriveStep,
                                       overdriveStepDefault);
}

void EqConfig::setOverdriveStep(uint16_t value) {
  EqEeprom::writeValue<uint16_t>(
      EqEeprom::OverdriveStep,
      constrain(value, overdriveStepMin, overdriveStepMax));
}

uint8_t EqConfig::fanPwmInterval() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::FanPwmInterval,
                                      fanPwmIntervalDefault);
}

void EqConfig::setFanPwmInterval(uint8_t value) {
  uint8_t __i = constrain(value, fanPwmIntervalMin, fanPwmIntervalMax);
  EqEeprom::writeValue<uint8_t>(EqEeprom::FanPwmInterval, __i);
  eqTaskFanControl().setInterval(__i);
}

uint8_t EqConfig::fanPwmMin() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::FanPwmMin, fanPwmMinDefault);
}

void EqConfig::setFanPwmMin(uint8_t value) {
  uint8_t __v = constrain(value, fanPwmMinDefault, fanPwmMaxDefault - 1);
  if (__v < EqConfig::fanPwmMax()) {
    EqEeprom::writeValue<uint8_t>(EqEeprom::FanPwmMin, __v);
    eqTaskFanControl().forceNextIteration();
  }
}

uint8_t EqConfig::fanPwmMax() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::FanPwmMax, fanPwmMaxDefault);
}

void EqConfig::setFanPwmMax(uint8_t value) {
  uint8_t __v = constrain(value, fanPwmMinDefault + 1, fanPwmMaxDefault);
  if (__v > EqConfig::fanPwmMin()) {
    EqEeprom::writeValue<uint8_t>(EqEeprom::FanPwmMax, __v);
    eqTaskFanControl().forceNextIteration();
  }
}

uint8_t EqConfig::fanPwmOverdrive() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::FanPwmOverdrive,
                                      fanPwmOverdriveDefault);
}

void EqConfig::setFanPwmOverdrive(uint8_t value) {
  EqEeprom::writeValue<uint8_t>(EqEeprom::FanPwmOverdrive,
                                constrain(value, EqConfig::fanPwmMin(), 100));
}

bool EqConfig::isFanPwmStepModeEnabled() {
  return EqEeprom::readValue<bool>(EqEeprom::FanPwmStepMode,
                                   fanPwmStepModeDefault);
}

void EqConfig::setFanPwmStepMode(bool enabled) {
  EqEeprom::writeValue<bool>(EqEeprom::FanPwmStepMode, enabled);
  eqTaskFanControl().forceNextIteration();
}

bool EqConfig::isFanTachometerEnabled() {
  return EqDPin<EqConfig::fanTachometerControlPin>::isInputHigh();
}

bool EqConfig::isBlowingEnabled() {
  return EqEeprom::readValue<bool>(EqEeprom::BlowingEnabled,
                                   blowingEnabledDefault);
}

void EqConfig::setBlowingEnabled(bool enabled) {
  EqEeprom::writeValue<bool>(EqEeprom::BlowingEnabled, enabled);
}

uint8_t EqConfig::blowingInterval() {
  return EqEeprom::readValue<uint8_t>(EqEeprom::BlowingInterval,
                                      blowingIntervalDefault);
}

void EqConfig::setBlowingInterval(uint8_t value) {
  EqEeprom::writeValue<uint8_t>(
      EqEeprom::BlowingInterval,
      constrain(value, blowingIntervalMin, blowingIntervalMax));
}

uint16_t EqConfig::blowingTime() {
  return EqEeprom::readValue<uint16_t>(EqEeprom::BlowingTime,
                                       blowingTimeDefault);
}

void EqConfig::setBlowingTime(uint16_t value) {
  EqEeprom::writeValue<uint16_t>(
      EqEeprom::BlowingTime, constrain(value, blowingTimeMin, blowingTimeMax));
}

uint16_t EqConfig::backlightTime() {
  return EqEeprom::readValue<uint16_t>(EqEeprom::BacklighTime,
                                       backlightTimeDefault);
}

void EqConfig::setBacklighTime(uint16_t value) {
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
