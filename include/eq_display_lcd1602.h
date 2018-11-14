
// Do not include this file directly

#ifndef __EQ_DISPLAY_H__
#error "Do not include this file directly"
#endif

#ifndef __EQ_DISPLAY_LCD1602_H__
#define __EQ_DISPLAY_LCD1602_H__

#include <LiquidCrystal_PCF8574.h>
#include <PGMWrap.h>
#include <Wire.h>

class __EqLcd1602 {
public:
  __EqLcd1602() : lcd_(0x27) {}
  bool init();
  void turnOff();
  void turnOn() { lcd_.setBacklight(255); }
  void showHT(const float &humidity, const float &temperature);
  void showTrends(const int16_t &trendHumidity,
                  const int16_t &trendTemperature);
  void showOverdriveTime();
  void showFanSpeed(const uint8_t &speed);
  void showAlert();
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
const PROGMEM __EqLcd1602::Bar_ __EqLcd1602::bars_[] = {
    {B11100, B11110, B11110, B11110, B11110, B11110, B11110, B11100},
    {B00111, B01111, B01111, B01111, B01111, B01111, B01111, B00111},
    {B11111, B11111, B00000, B00000, B00000, B00000, B11111, B11111},
    {B11110, B11100, B00000, B00000, B00000, B00000, B11000, B11100},
    {B01111, B00111, B00000, B00000, B00000, B00000, B00011, B00111},
    {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111},
    {B00000, B00000, B00000, B00000, B00000, B00000, B00111, B01111},
    {B11111, B11111, B00000, B00000, B00000, B00000, B00000, B00000}};

const PROGMEM __EqLcd1602::Digit_ __EqLcd1602::digits_[] = {
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

bool __EqLcd1602::init() {
  Wire.beginTransmission(0x27);
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
  lcd_.setCursor(2, 1);
  lcd_.print(F("h"));
  lcd_.print(EqConfig::htSensorHumidityThreshold());
  lcd_.print(F(" t"));
  lcd_.print(EqConfig::htSensorTemperatureThreshold());
  lcd_.print(F(" i"));
  lcd_.print(EqConfig::htSensorInterval());
  lcd_.print(F(" m"));
  lcd_.print(static_cast<uint8_t>(EqConfig::htIndexType()));

  uint8_t __bar[8];
  for (uint8_t __i = 0; __i < 8; __i++) {
    for (uint8_t __j = 0; __j < 8; __j++)
      __bar[__j] = __EqLcd1602::bars_[__i][__j]; // uint8_p => uint8_t
    lcd_.createChar(__i + 1, __bar);
  }
  return true;
}

void __EqLcd1602::turnOff() {
  lcd_.setBacklight(0);
  lcd_.clear();
  lastSpeedDots_ = 0xFF;
}

void __EqLcd1602::showHT(const float &humidity, const float &temperature) {
  printValue_(round(humidity), 0);
  lcd_.setCursor(6, 0);
  lcd_.print(F("% "));
  lcd_.setCursor(10, 0);
  lcd_.print(String(temperature, 1));
  lcd_.write(0xDF);
}

void __EqLcd1602::showTrends(const int16_t &trendHumidity,
                             const int16_t &trendTemperature) {
  lcd_.setCursor(7, 0);
  if (trendHumidity > 0)
    lcd_.write(0x7E);
  else if (trendHumidity < 0)
    lcd_.write(0x7F);
  else
    lcd_.write(0x20);
  lcd_.print(F("  "));
  lcd_.setCursor(15, 0);
  if (trendTemperature > 0)
    lcd_.write(0x7E);
  else if (trendTemperature < 0)
    lcd_.write(0x7F);
  else
    lcd_.write(0x20);
}

void __EqLcd1602::showOverdriveTime() {
  lcd_.clear();
  printValue_(EqConfig::overdriveTime() / 60, 2);
  lcd_.setCursor(8, 0);
  lcd_.write(0xA1);
  lcd_.setCursor(8, 1);
  lcd_.write(0xDF);
  printValue_(EqConfig::overdriveTime() % 60, 9);
  lastSpeedDots_ = 0xFF;
}

void __EqLcd1602::showFanSpeed(const uint8_t &speed) {
  uint8_t __c = min(10, speed / 10);
  if ((speed > 0) && (__c == 0))
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

void __EqLcd1602::showAlert() {
  /*
  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  A  L  E  R  T
                                               0
  */
  lcd_.clear();
  lcd_.print(F("ALERT"));
  String __s = EqConfig::alertAsString();
  lcd_.setCursor(16 - __s.length(), 1);
  lcd_.print(__s);
  lastSpeedDots_ = 0xFF;
}

void __EqLcd1602::showCalibrating(const uint8_t &percents) {
  lcd_.clear();
  lcd_.print(F("Calibrating..."));
  showFanSpeed(percents);
}

/*
  specializations of EqDisplay
*/

template <> struct __EqDisplayObject<EQ_LCD_1602> { typedef __EqLcd1602 Type; };

template <> bool EqDisplay<EQ_LCD_1602>::initDisplay_() {
  return display_.init();
}

template <> void EqDisplay<EQ_LCD_1602>::turnOff_() { display_.turnOff(); }

template <> void EqDisplay<EQ_LCD_1602>::turnOn_() { display_.turnOn(); }

template <>
void EqDisplay<EQ_LCD_1602>::showHT_(const float &humidity,
                                     const float &temperature) {
  display_.showHT(humidity, temperature);
}

template <>
void EqDisplay<EQ_LCD_1602>::showTrends_(const int16_t &trendHumidity,
                                         const int16_t &trendTemperature) {
  display_.showTrends(trendHumidity, trendTemperature);
}

template <> void EqDisplay<EQ_LCD_1602>::showOverdriveTime_() {
  display_.showOverdriveTime();
}

template <> void EqDisplay<EQ_LCD_1602>::showFanSpeed_(const uint8_t &speed) {
  display_.showFanSpeed(speed);
}

template <> void EqDisplay<EQ_LCD_1602>::showAlert() { display_.showAlert(); }

template <> void EqDisplay<EQ_LCD_1602>::showCalibrating(uint8_t percents) {
  display_.showCalibrating(percents);
}

#endif // __EQ_DISPLAY_LCD1602_H__
