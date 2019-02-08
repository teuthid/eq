/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_DISPLAY_H__
#define __EQ_DISPLAY_H__

#include "eq_config.h"

template <uint8_t Model> class EqDisplay {
  friend EqDisplay<EQ_DISPLAY_TYPE> &eqDisplay();

public:
  void show();
  bool init();

  // needs specialization:
  void showMessage(const char *message);
  void showAlert(const EqAlertType &alert);
  void showCalibrating(const uint8_t &percents);

  EqDisplay(const EqDisplay &) = delete;
  EqDisplay(EqDisplay &&) = delete;
  void operator=(const EqDisplay &) = delete;

private:
  constexpr EqDisplay() {}
  void backlight_(const bool &on = true);

  // needs specialization:
  bool initDisplay_();
  void turnOff_();
  void turnOn_();
  void showHT_();
  void showOverdriveTime_();
  void showFanSpeed_();

private:
  bool isOn_ = false;
  static EqDisplay instance_;
};

inline EqDisplay<EQ_DISPLAY_TYPE> &eqDisplay() {
  return EqDisplay<EQ_DISPLAY_TYPE>::instance_;
}

template <uint8_t Model> bool EqDisplay<Model>::init() {
#ifdef EQ_DEBUG
  Serial.print(EqConfig::alertAsString(EqAlertType::Display));
#endif
  return initDisplay_();
}

template <uint8_t Model> void EqDisplay<Model>::show() {
  if (EqConfig::backlight()) {
    backlight_();
    if (!EqConfig::anyAlert())
      if (EqConfig::overdriveTime() == 0) {
        showHT_();
        showFanSpeed_();
      } else // show overdrive time
        showOverdriveTime_();
    else // show alert
      showAlert(EqConfig::alert());
  } else // turn off display
    backlight_(false);
}

template <uint8_t Model> void EqDisplay<Model>::backlight_(const bool &on) {
  if (on)
    if (!isOn_) {
      turnOn_();
      isOn_ = true;
    } else
      ;
  else if (isOn_) {
    turnOff_();
    isOn_ = false;
  }
}

#endif // __EQ_DISPLAY_H__
