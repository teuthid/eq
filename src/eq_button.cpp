/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_button.h"

// specializations:

// backlight on/off & reset (long press)
template <> void EqButtonBacklight::read() {
  update();
  if (pressedFor(EqConfig::buttonLongPressedTime))
    EqConfig::reset();
}

template <> void EqButtonBacklight::connect_() {
  onPressed([]() { EqConfig::setBacklight(!EqConfig::backlight()); });
}

// overdrive & hard reset (long press)
template <> void EqButtonOverdrive::read() {
  update();
  if (pressedFor(EqConfig::buttonLongPressedTime))
    EqConfig::reset(true);
}

template <> void EqButtonOverdrive::connect_() {
  onPressed(
      []() { EqConfig::increaseOverdriveTime(EqConfig::overdriveStep()); });
}

// instances of EqButton
template <> EqButtonBacklight EqButtonBacklight::instance_{};
template <> EqButtonOverdrive EqButtonOverdrive::instance_{};
