
#ifndef __EQ_BUTTON_H__
#define __EQ_BUTTON_H__

#include "eq_config.h"
#include <EasyButton.h>

template <uint8_t ButtonPin> class EqButton {
public:
  EqButton() : button_(ButtonPin, EqConfig::buttonDebounceTime) {}

  void init() {
    button_.begin();
    setOnPressed_();
  }
  void read() { button_.read(); }

private:
  void setOnPressed_() {}
  EasyButton button_;
};

extern EqButton<EqConfig::buttonOverdrivePin> eqButtonOverdrive;
extern EqButton<EqConfig::buttonBacklightPin> eqButtonBacklight;

#endif // __EQ_BUTTON_H__
