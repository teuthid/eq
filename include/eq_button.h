
#ifndef __EQ_BUTTON_H__
#define __EQ_BUTTON_H__

#include "eq_config.h"
#include <EasyButton.h>

template <uint8_t ButtonPin> class EqButton {
public:
  static EqButton &instance() {
    static EqButton instance;
    return instance;
  }
  EqButton(const EqButton &) = delete;
  EqButton(EqButton &&) = delete;
  void operator=(const EqButton &) = delete;

  // needs specialization:
  void init();
  void read();

private:
  EqButton() : button_(ButtonPin, EqConfig::buttonDebounceTime) {}
  void setOnPressed_();
  EasyButton button_;
};

template <uint8_t ButtonPin> void EqButton<ButtonPin>::init() {
  button_.begin();
  setOnPressed_();
}

using EqButtonBacklight = EqButton<EqConfig::buttonBacklightPin>;
using EqButtonOverdrive = EqButton<EqConfig::buttonOverdrivePin>;

#endif // __EQ_BUTTON_H__
