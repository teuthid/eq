/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#ifndef __EQ_BUTTON_H__
#define __EQ_BUTTON_H__

#include <EasyButton.h>

#include "eq_config.h"

template <uint8_t ButtonPin> class EqButton;
using EqButtonBacklight = EqButton<EqConfig::buttonBacklightPin>;
using EqButtonOverdrive = EqButton<EqConfig::buttonOverdrivePin>;

template <uint8_t ButtonPin> class EqButton {
  friend EqButtonBacklight &eqButtonBacklight();
  friend EqButtonOverdrive &eqButtonOverdrive();

public:
  EqButton(const EqButton &) = delete;
  EqButton(EqButton &&) = delete;
  void operator=(const EqButton &) = delete;

  // needs specialization
  void init();
  void read();

private:
  EqButton();
  void setOnPressed_(); // needs specialization
  EasyButton<ButtonPin> button_;
  static EqButton instance_;
};

inline EqButtonBacklight &eqButtonBacklight() {
  return EqButtonBacklight::instance_;
}
inline EqButtonOverdrive &eqButtonOverdrive() {
  return EqButtonOverdrive::instance_;
}

template <uint8_t ButtonPin> void EqButton<ButtonPin>::init() {
  button_.init();
  setOnPressed_();
}

#endif // __EQ_BUTTON_H__
