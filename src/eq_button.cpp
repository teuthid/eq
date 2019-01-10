/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
*/

#include "eq_button.h"

// specializations:

// backlight on/off & reset (long press)
template <>
EqButtonBacklight::EqButton()
    : button_(EqConfig::buttonBacklightPin, EqConfig::buttonDebounceTime, false,
              false) {}

template <> void EqButtonBacklight::read() {
  button_.read();
  if (button_.pressedFor(EqConfig::buttonLongPressedTime))
    EqConfig::reset();
}

template <> void EqButtonBacklight::setOnPressed_() {
  button_.onPressed([]() { EqConfig::setBacklight(!EqConfig::backlight()); });
}

// overdrive & hard reset (long press)
template <>
EqButtonOverdrive::EqButton()
    : button_(EqConfig::buttonOverdrivePin, EqConfig::buttonDebounceTime) {}

template <> void EqButtonOverdrive::read() {
  button_.read();
  if (button_.pressedFor(EqConfig::buttonLongPressedTime))
    EqConfig::reset(true);
}

template <> void EqButtonOverdrive::setOnPressed_() {
  button_.onPressed([]() {
    if (!EqConfig::anyAlert())
      EqConfig::increaseOverdriveTime(EqConfig::overdriveStep());
  });
}

// instances of EqButton
template <> EqButtonBacklight EqButtonBacklight::instance_{};
template <> EqButtonOverdrive EqButtonOverdrive::instance_{};
