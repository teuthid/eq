
#include "eq_config.h"

#if (EQ_DISPLAY_TYPE == EQ_NO_DISPLAY)

#include "eq_display.h"

EqDisplay<EQ_NO_DISPLAY> eqDisplay; // preinstatiate

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

template <> void EqDisplay<EQ_NO_DISPLAY>::showCalibrating(uint8_t percents) {}

#endif // (EQ_DISPLAY_TYPE == EQ_NO_DISPLAY)
