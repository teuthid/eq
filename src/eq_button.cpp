
#include "eq_button.h"

// specializations:

// overdrive & hard reset (long press)
template <> void EqButton<EqConfig::buttonOverdrivePin>::read() {
  button_.read();
  if (button_.pressedFor(EqConfig::buttonLongPressedTime))
    EqConfig::reset(true);
}

template <> void EqButton<EqConfig::buttonOverdrivePin>::setOnPressed_() {
  button_.onPressed([]() {
    if (!EqConfig::anyAlert())
      EqConfig::increaseOverdriveTime(EqConfig::overdriveStep());
  });
}

// backlight on/off & reset (long press)
template <>
EqButton<EqConfig::buttonBacklightPin>::EqButton()
    : button_(EqConfig::buttonBacklightPin, EqConfig::buttonDebounceTime, false,
              false) {} /* touch button */

template <> void EqButton<EqConfig::buttonBacklightPin>::read() {
  button_.read();
  if (button_.pressedFor(EqConfig::buttonLongPressedTime))
    EqConfig::reset();
}

template <> void EqButton<EqConfig::buttonBacklightPin>::setOnPressed_() {
  button_.onPressed([]() { EqConfig::setBacklight(!EqConfig::backlight()); });
}
