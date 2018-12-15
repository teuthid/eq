
#ifndef __EQ_DISPLAY_H__
#define __EQ_DISPLAY_H__

#include "eq_config.h"

template <uint8_t Model> class EqDisplay {
public:
  EqDisplay() {}
  void show();
  bool init();

  // needs specialization:
  void showMessage(const char *message);
  void showAlert(const EqAlertType &alert);
  void showCalibrating(uint8_t percents);

private:
  void backlight_(const bool &on = true);

  // needs specialization:
  bool initDisplay_();
  void turnOff_();
  void turnOn_();
  void showHT_();
  void showTrends_();
  void showOverdriveTime_();
  void showFanSpeed_();

private:
  bool isOn_ = false;
};

template <uint8_t Model> bool EqDisplay<Model>::init() {
#ifdef EQ_DEBUG
  Serial.print(EqConfig::alertAsString(EqAlertType::Display));
#endif
  return initDisplay_();
}

extern EqDisplay<EQ_DISPLAY_TYPE> eqDisplay;

template <uint8_t Model> void EqDisplay<Model>::show() {
  if (EqConfig::backlight()) {
    backlight_();
    if (!EqConfig::anyAlert())
      if (EqConfig::overdriveTime() == 0) {
        showHT_();
        showTrends_();
        showFanSpeed_();
      } else
        showOverdriveTime_();
    else
      showAlert(EqConfig::alert());
  } else
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
