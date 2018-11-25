
#ifndef __EQ_BUTTON_H__
#define __EQ_BUTTON_H__

#include "eq_config.h"
#include <EasyButton.h>

template <uint8_t ButtonPin> class EqButton {
public:
  EqButton() : button_(ButtonPin, EqConfig::buttonDebounceTime) {}

  void init();
  void read();

private:
  void setOnPressed_();
  EasyButton button_;
};

template <uint8_t ButtonPin> void EqButton<ButtonPin>::init() {
  button_.begin();
  setOnPressed_();
}

extern EqButton<EqConfig::buttonOverdrivePin> eqButtonOverdrive;
extern EqButton<EqConfig::buttonBacklightPin> eqButtonBacklight;

#endif // __EQ_BUTTON_H__
