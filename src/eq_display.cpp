
#include "eq_display.h"

#if (EQ_DISPLAY_TYPE == EQ_NO_DISPLAY)

template <> bool EqDisplay<EQ_NO_DISPLAY>::initDisplay_() { return true; }

template <> void EqDisplay<EQ_NO_DISPLAY>::turnOff_() {}

template <> void EqDisplay<EQ_NO_DISPLAY>::turnOn_() {}

template <> void EqDisplay<EQ_NO_DISPLAY>::showHT_() {}

template <> void EqDisplay<EQ_NO_DISPLAY>::showTrends_() {}

template <> void EqDisplay<EQ_NO_DISPLAY>::showOverdriveTime_() {}

template <> void EqDisplay<EQ_NO_DISPLAY>::showFanSpeed_() {}

template <> void EqDisplay<EQ_NO_DISPLAY>::showMessage(const char *message) {}

template <>
void EqDisplay<EQ_NO_DISPLAY>::showAlert(const EqAlertType &alert) {}

template <>
void EqDisplay<EQ_NO_DISPLAY>::showCalibrating(const uint8_t &percents) {}

#endif // (EQ_DISPLAY_TYPE == EQ_NO_DISPLAY)

// instance of EqDisplay
template <> EqDisplay<EQ_DISPLAY_TYPE> EqDisplay<EQ_DISPLAY_TYPE>::instance_{};
