
#include "eq_display.h"

#if (EQ_DISPLAY_TYPE == EQ_LCD_1602)

#include <LiquidCrystal_PCF8574.h>
#include <PGMWrap.h>
#include <Wire.h>

#include "eq_fan_pwm.h"
#include "eq_ht_sensor.h"

namespace {

class EqLcd1602 {
public:
  EqLcd1602() : lcd_(0x27) {}
  bool init();
  void turnOff();
  void turnOn() { lcd_.setBacklight(255); }
  void showHT();
  void showTrends();
  void showOverdriveTime();
  void showFanSpeed(bool detected = true, uint8_t percents = 0);
  void showMessage(const char *message); // second line
  void showAlert(const EqAlertType &alert);
  void showCalibrating(const uint8_t &percents);

private:
  LiquidCrystal_PCF8574 lcd_;
  uint8_t lastSpeedDots_ = 0xFF;

  typedef uint8_p Bar_[8];
  static const PROGMEM Bar_ bars_[];

  typedef uint8_p Digit_[6];
  static const PROGMEM Digit_ digits_[];

  void printDigit_(const uint8_t &digit, const uint8_t &col) {
    lcd_.setCursor(col, 0);
    for (uint8_t __i = 0; __i < 6; __i++) {
      lcd_.write(digits_[digit][__i]);
      if (__i == 2)
        lcd_.setCursor(col, 1);
    }
  }
  void printValue_(const uint8_t &value, const uint8_t &col) {
    printDigit_(value / 10, col);
    printDigit_(value % 10, col + 3);
  }
};

// elements of big digits:
const PROGMEM EqLcd1602::Bar_ EqLcd1602::bars_[] = {
    {B11100, B11110, B11110, B11110, B11110, B11110, B11110, B11100},
    {B00111, B01111, B01111, B01111, B01111, B01111, B01111, B00111},
    {B11111, B11111, B00000, B00000, B00000, B00000, B11111, B11111},
    {B11110, B11100, B00000, B00000, B00000, B00000, B11000, B11100},
    {B01111, B00111, B00000, B00000, B00000, B00000, B00011, B00111},
    {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111},
    {B00000, B00000, B00000, B00000, B00000, B00000, B00111, B01111},
    {B11111, B11111, B00000, B00000, B00000, B00000, B00000, B00000}};

const PROGMEM EqLcd1602::Digit_ EqLcd1602::digits_[] = {
    {2, 8, 1, 2, 6, 1},     // 0
    {32, 32, 1, 32, 32, 1}, // 1
    {5, 3, 1, 2, 6, 6},     // 2
    {5, 3, 1, 7, 6, 1},     // 3
    {2, 6, 1, 32, 32, 1},   // 4
    {2, 3, 4, 7, 6, 1},     // 5
    {2, 3, 4, 2, 6, 1},     // 6
    {2, 8, 1, 32, 32, 1},   // 7
    {2, 3, 1, 2, 6, 1},     // 8
    {2, 3, 1, 7, 6, 1}};    // 9

bool EqLcd1602::init() {
  Wire.beginTransmission(EqConfig::lcdI2CAddress);
  if (Wire.endTransmission() != 0) {
    EqConfig::setAlert(EqAlertType::Display);
    return false;
  }
  lcd_.begin(16, 2);
  lcd_.noBlink();
  lcd_.noCursor();
  lcd_.noAutoscroll();
  lcd_.setBacklight(255);
  lcd_.print(F("Booting..."));
  uint8_t __bar[8];
  for (uint8_t __i = 0; __i < 8; __i++) {
    for (uint8_t __j = 0; __j < 8; __j++)
      __bar[__j] = EqLcd1602::bars_[__i][__j]; // uint8_p => uint8_t
    lcd_.createChar(__i + 1, __bar);
  }
  return true;
}

void EqLcd1602::turnOff() {
  lcd_.setBacklight(0);
  lcd_.clear();
  lastSpeedDots_ = 0xFF;
}

void EqLcd1602::showHT() {
  printValue_(fixed_to_int(eqHtSensor.lastHumidity()), 0);
  lcd_.setCursor(6, 0);
  lcd_.print(F("% "));
  lcd_.setCursor(10, 0);
  char __s[5];
  lcd_.print(dtostrf(fixed_to_float(eqHtSensor.lastTemperature()), 4, 1, __s));
  lcd_.write(0xDF);
}

void EqLcd1602::showTrends() {
  int8_t __tH = eqHtSensor.trendHumidity();
  int8_t __tT = eqHtSensor.trendTemperature();
  lcd_.setCursor(7, 0);
  if (__tH > 0)
    lcd_.write(0x7E);
  else if (__tH < 0)
    lcd_.write(0x7F);
  else
    lcd_.write(0x20);
  lcd_.print(F("  "));
  lcd_.setCursor(15, 0);
  if (__tT > 0)
    lcd_.write(0x7E);
  else if (__tT < 0)
    lcd_.write(0x7F);
  else
    lcd_.write(0x20);
}

void EqLcd1602::showOverdriveTime() {
  lcd_.clear();
  printValue_(EqConfig::overdriveTime() / 60, 2);
  lcd_.setCursor(8, 0);
  lcd_.write(0xA1);
  lcd_.setCursor(8, 1);
  lcd_.write(0xDF);
  printValue_(EqConfig::overdriveTime() % 60, 9);
  lastSpeedDots_ = 0xFF;
}

void EqLcd1602::showFanSpeed(bool detected, uint8_t percents) {
  uint8_t __s = detected ? eqFanPwm().lastSpeed() : percents;
  uint8_t __c = min(10, __s / 10);
  if ((__s > 0) && (__c == 0))
    __c = 1;
  if (lastSpeedDots_ != __c) {
    lcd_.setCursor(6, 1);
    for (uint8_t __i = 0; __i < __c; __i++)
      lcd_.write(0xFF);
    for (uint8_t __i = __c; __i < 10; __i++)
      lcd_.write(0xA5);
    lastSpeedDots_ = __c;
  }
}

void EqLcd1602::showMessage(const char *message) {
  lcd_.setCursor(0, 1);
  for (uint8_t __i = 0; __i < 16; __i++)
    lcd_.write(0x20);
  lcd_.setCursor(17 - min(16, strlen(message)), 1);
  lcd_.print(message);
}

void EqLcd1602::showAlert(const EqAlertType &alert) {
  lcd_.clear();
  lcd_.print(F("ALERT"));
  showMessage(EqConfig::alertAsString(alert));
  lastSpeedDots_ = 0xFF;
}

void EqLcd1602::showCalibrating(const uint8_t &percents) {
  lcd_.clear();
  lcd_.print(F("Calibrating..."));
  showFanSpeed(false, percents);
}

EqLcd1602 __lcd1602;

} // namespace

/*
  specializations of EqDisplay
*/

template <> bool EqDisplay<EQ_LCD_1602>::initDisplay_() {
  return __lcd1602.init();
}

template <> void EqDisplay<EQ_LCD_1602>::turnOff_() { __lcd1602.turnOff(); }

template <> void EqDisplay<EQ_LCD_1602>::turnOn_() { __lcd1602.turnOn(); }

template <> void EqDisplay<EQ_LCD_1602>::showHT_() { __lcd1602.showHT(); }

template <> void EqDisplay<EQ_LCD_1602>::showTrends_() {
  __lcd1602.showTrends();
}

template <> void EqDisplay<EQ_LCD_1602>::showOverdriveTime_() {
  __lcd1602.showOverdriveTime();
}

template <> void EqDisplay<EQ_LCD_1602>::showFanSpeed_() {
  __lcd1602.showFanSpeed();
}

template <> void EqDisplay<EQ_LCD_1602>::showMessage(const char *message) {
  __lcd1602.showMessage(message);
}

template <> void EqDisplay<EQ_LCD_1602>::showAlert(const EqAlertType &alert) {
  __lcd1602.showAlert(alert);
}

template <>
void EqDisplay<EQ_LCD_1602>::showCalibrating(const uint8_t &percents) {
  __lcd1602.showCalibrating(percents);
}

#endif // (EQ_DISPLAY_TYPE == EQ_LCD_1602)
