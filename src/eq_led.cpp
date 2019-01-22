/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_led.h"

template <> EqLedAlert EqLedAlert::instance_{};
template <> EqLedHeartbeat EqLedHeartbeat::instance_{};
#if (EQ_LED_STATUS_ENABLED)
template <> EqLedStatus EqLedStatus::instance_{};
#endif
