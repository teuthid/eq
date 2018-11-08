
#ifndef __EQ_BUTTON_H__
#define __EQ_BUTTON_H__

#include "eq_config.h"
#include <EasyButton.h>

template <uint8_t ButtonPin> class EqButton {
public:
  constexpr EqButton() : button_(ButtonPin, EqConfig::buttonDebounceTime) {}

  void init() {
    button_.begin();
    setOnPressed_();
  }
  void read() { button_.read(); }

private:
  void setOnPressed_() {}
  EasyButton button_;
};

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
              false) { /* touch button */}
template <> void EqButton<EqConfig::buttonBacklightPin>::read() {
  button_.read();
  if (button_.pressedFor(EqConfig::buttonLongPressedTime))
    EqConfig::reset();
}
template <> void EqButton<EqConfig::buttonBacklightPin>::setOnPressed_() {
  button_.onPressed([]() { EqConfig::setBacklight(!EqConfig::backlight()); });
}

#endif // __EQ_BUTTON_H__
