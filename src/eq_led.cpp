/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
*/

#include "eq_led.h"

template <> EqLedAlert EqLedAlert::instance_{};
template <> EqLedHeartbeat EqLedHeartbeat::instance_{};
#if (EQ_LED_STATUS_ENABLED)
template <> EqLedStatus EqLedStatus::instance_{};
#endif
