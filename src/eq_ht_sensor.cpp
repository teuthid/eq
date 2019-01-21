/*
   eq - transcendental fan controller ;)
   Copyright (c) 2017-2018 Mariusz Przygodzki
*/

#include "eq_ht_sensor.h"

// instance of HT sensor
template <>
EqHtSensor<EQ_HT_SENSOR_TYPE> EqHtSensor<EQ_HT_SENSOR_TYPE>::instance_{};

// instance of IT sensor
template <> EqItSensor EqItSensor::instance_{};
