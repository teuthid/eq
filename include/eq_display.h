
#ifndef __EQ_DISPLAY_H__
#define __EQ_DISPLAY_H__

#include "eq_config.h"

template <uint8_t Model> class EqDisplay {
public:
  EqDisplay() {}
  void show(const float &humidity, const float &temperature,
            const int16_t &trendHumidity, const int16_t &trendTemperature,
            const uint8_t &fanSpeed);
  bool init();

  // needs specialization:
  void showAlert();
  void showCalibrating(uint8_t percents);

private:
  void backlight_(const bool &on = true);

  // needs specialization:
  bool initDisplay_();
  void turnOff_();
  void turnOn_();
  void showHT_(const float &humidity, const float &temperature);
  void showTrends_(const int16_t &trendHumidity,
                   const int16_t &trendTemperature);
  void showOverdriveTime_();
  void showFanSpeed_(const uint8_t &speed /* 0..100% */);

private:
  bool isOn_ = false;
};

template <uint8_t Model> bool EqDisplay<Model>::init() {
#ifdef EQ_DEBUG
  Serial.print(F("[Display] "));
#endif
  return initDisplay_();
}

extern EqDisplay<EQ_DISPLAY_TYPE> eqDisplay;

template <uint8_t Model>
void EqDisplay<Model>::show(const float &humidity, const float &temperature,
                            const int16_t &trendHumidity,
                            const int16_t &trendTemperature,
                            const uint8_t &fanSpeed) {
  if (EqConfig::backlight()) {
    backlight_();
    if (!EqConfig::anyAlert())
      if (EqConfig::overdriveTime() == 0) {
        showHT_(humidity, temperature);
        showTrends_(trendHumidity, trendTemperature);
        showFanSpeed_(fanSpeed);
      } else
        showOverdriveTime_();
    else
      showAlert();
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
