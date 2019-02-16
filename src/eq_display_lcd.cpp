/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

#include "eq_display.h"

#if (EQ_DISPLAY_TYPE == EQ_LCD_1602) || (EQ_DISPLAY_TYPE == EQ_LCD_2004)

#include <LiquidCrystal_PCF8574.h>
#include <PGMWrap.h>
#include <Wire.h>

#include "eq_fan_pwm.h"
#include "eq_ht_sensor.h"

class EqLcd {
public:
  EqLcd() : lcd_(EqConfig::lcdI2CAddress) {}
  bool init();
  void turnOff();
  void turnOn();
  void showHT();
  void showOverdriveTime();
  void showFanSpeed(bool detected = true, uint8_t percents = 0);
  void showMessage(const char *message); // second line
  void showAlert(EqAlertType alert);
  void showCalibrating(uint8_t percents);

private:
  bool cleared_ = false; // true if screen was already cleared
  uint16_t lastDH_ = 0;  // last displayed humidity
  uint16_t lastDT_ = 0;  // last displayed temperature
  uint8_t lastSpeedDots_ =
      0xFF; // last displayed number of dots (speed)
            // 0xFF means that the dots were not displayed at all
  LiquidCrystal_PCF8574 lcd_;

  typedef uint8_p Bar_[8];
  static const PROGMEM Bar_ bars_[]; // elements of big digits

  typedef uint8_p Digit_[6];
  static const PROGMEM Digit_ digits_[]; // definitons of big digits

#if (EQ_DISPLAY_TYPE == EQ_LCD_1602)
  static constexpr uint8_t maxCols_ = 16;
  static constexpr uint8_t maxRows_ = 2;
  static constexpr uint8_t maxSpeedDots_ = 10;
  static constexpr uint8_t colSpeedBar_ = 6;
  static constexpr uint8_t rowSpeedBar_ = 1;
  static constexpr uint8_t messageRow_ = 1;
#elif (EQ_DISPLAY_TYPE == EQ_LCD_2004)
  static constexpr uint8_t maxCols_ = 20;
  static constexpr uint8_t maxRows_ = 4;
  static constexpr uint8_t maxSpeedDots_ = 20;
  static constexpr uint8_t colSpeedBar_ = 0;
  static constexpr uint8_t rowSpeedBar_ = 2;
  static constexpr uint8_t messageRow_ = 3;
#endif

  void clear_();
  void printBigDigit_(uint8_t digit, uint8_t col);
  void printBigValue_(uint8_t value, uint8_t col);
  void printHumidity_();
  void printTemperature_();
  void printValue_(uint8_t value);
};

const PROGMEM EqLcd::Bar_ EqLcd::bars_[] = {
    {B11100, B11110, B11110, B11110, B11110, B11110, B11110, B11100},
    {B00111, B01111, B01111, B01111, B01111, B01111, B01111, B00111},
    {B11111, B11111, B00000, B00000, B00000, B00000, B11111, B11111},
    {B11110, B11100, B00000, B00000, B00000, B00000, B11000, B11100},
    {B01111, B00111, B00000, B00000, B00000, B00000, B00011, B00111},
    {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111},
    {B00000, B00000, B00000, B00000, B00000, B00000, B00111, B01111},
    {B11111, B11111, B00000, B00000, B00000, B00000, B00000, B00000}};

const PROGMEM EqLcd::Digit_ EqLcd::digits_[] = {{2, 8, 1, 2, 6, 1},     // 0
                                                {32, 32, 1, 32, 32, 1}, // 1
                                                {5, 3, 1, 2, 6, 6},     // 2
                                                {5, 3, 1, 7, 6, 1},     // 3
                                                {2, 6, 1, 32, 32, 1},   // 4
                                                {2, 3, 4, 7, 6, 1},     // 5
                                                {2, 3, 4, 2, 6, 1},     // 6
                                                {2, 8, 1, 32, 32, 1},   // 7
                                                {2, 3, 1, 2, 6, 1},     // 8
                                                {2, 3, 1, 7, 6, 1}};    // 9

void EqLcd::clear_() {
  if (!cleared_) {
    lcd_.clear();
    cleared_ = true;
  }
}

void EqLcd::printBigDigit_(uint8_t digit, uint8_t col) {
  lcd_.setCursor(col, 0);
  for (uint8_t __i = 0; __i < 6; __i++) {
    lcd_.write(digits_[digit][__i]);
    if (__i == 2)
      lcd_.setCursor(col, 1);
  }
}

void EqLcd::printBigValue_(uint8_t value, uint8_t col) {
  printBigDigit_(value / 10, col);
  printBigDigit_(value % 10, col + 3);
}

void EqLcd::printHumidity_() {
#if (EQ_DISPLAY_TYPE == EQ_LCD_1602) || (EQ_DISPLAY_TYPE == EQ_LCD_2004)
  printBigValue_(lastDH_ / 10, 0);
  lcd_.setCursor(6, 0);
  lcd_.write('.');
  lcd_.write((lastDH_ % 10) + 48); // digit to ascii code
  lcd_.write('%');
#else
// TODO
#endif
}

void EqLcd::printTemperature_() {
#if (EQ_DISPLAY_TYPE == EQ_LCD_1602)
  lcd_.setCursor(9, 0);
  lcd_.print(F("  "));
  lcd_.print(lastDT_ / 10);
  lcd_.write('.');
  lcd_.write((lastDT_ % 10) + 48); // digit to ascii code
  lcd_.write(0xDF);
#elif (EQ_DISPLAY_TYPE == EQ_LCD_2004)
  lcd_.setCursor(9, 0);
  lcd_.print(F("  "));
  lcd_.setCursor(6, 1);
  lcd_.print(F("     "));
  printBigValue_(lastDT_ / 10, 11);
  lcd_.setCursor(17, 0);
  lcd_.write('.');
  lcd_.write((lastDT_ % 10) + 48); // digit to ascii code
  lcd_.write(0xDF);
#endif
}

void EqLcd::printValue_(uint8_t value) {
  lcd_.print(value / 10);
  lcd_.print(value % 10);
}

bool EqLcd::init() {
  Wire.beginTransmission(EqConfig::lcdI2CAddress);
  if (Wire.endTransmission() != 0) {
    EqConfig::setAlert(EqAlertType::Display);
    return false;
  }
  lcd_.begin(maxCols_, maxRows_);
  lcd_.noBlink();
  lcd_.noCursor();
  lcd_.noAutoscroll();
  lcd_.setBacklight(0xFF);
  lcd_.print(F("Booting... "));
  lcd_.print(EqConfig::readWatchdogPoint());
  uint8_t __bar[8];
  for (uint8_t __i = 0; __i < 8; __i++) {
    for (uint8_t __j = 0; __j < 8; __j++)
      __bar[__j] = bars_[__i][__j]; // uint8_p => uint8_t
    lcd_.createChar(__i + 1, __bar);
  }
  return true;
}

void EqLcd::turnOff() {
  lcd_.setBacklight(0);
  clear_();
  lastSpeedDots_ = 0xFF;
}

void EqLcd::turnOn() { lcd_.setBacklight(0xFF); }

void EqLcd::showHT() {
  uint16_t __DH = fixed_to_int(eqHtSensor().lastHumidity() * 10);
  uint16_t __DT = fixed_to_int(eqHtSensor().lastTemperature() * 10);
  if (cleared_ || (__DH != lastDH_)) {
    lastDH_ = __DH;
    printHumidity_();
  }
  if (cleared_ || (__DT != lastDT_)) {
    lastDT_ = __DT;
    printTemperature_();
  }
#if (EQ_DISPLAY_TYPE == EQ_LCD_2004)
  showMessage(""); // TODO: display internal temperature?
#endif
  cleared_ = false;
}

void EqLcd::showOverdriveTime() {
#if (EQ_DISPLAY_TYPE == EQ_LCD_1602)
  clear_();
  printBigValue_(EqConfig::overdriveTime() / 60, 2);
  lcd_.setCursor(8, 0);
  lcd_.write(0xA1);
  lcd_.setCursor(8, 1);
  lcd_.write(0xDF);
  printBigValue_(EqConfig::overdriveTime() % 60, 9);
  lastSpeedDots_ = 0xFF;
#elif (EQ_DISPLAY_TYPE == EQ_LCD_2004)
  // there is enough space to display everything:
  showHT();
  showFanSpeed();
  lcd_.setCursor(0, 3);
  lcd_.print(F("Override:"));
  lcd_.setCursor(15, 3);
  printValue_(EqConfig::overdriveTime() / 60);
  lcd_.write(':');
  printValue_(EqConfig::overdriveTime() % 60);
#endif
}

void EqLcd::showFanSpeed(bool detected, uint8_t percents) {
  uint16_t __s = detected ? eqFanPwm().lastSpeed() : percents;
  uint8_t __c = min(maxSpeedDots_, __s * maxSpeedDots_ / 100);
  if ((__s > 0) && (__c == 0))
    __c = 1;
  if (lastSpeedDots_ != __c) {
    lcd_.setCursor(colSpeedBar_, rowSpeedBar_);
    for (uint8_t __i = 0; __i < __c; __i++)
      lcd_.write(0xFF);
    for (uint8_t __i = __c; __i < maxSpeedDots_; __i++)
      lcd_.write(0xA5);
    lastSpeedDots_ = __c;
  }
}

void EqLcd::showMessage(const char *message) {
  lcd_.setCursor(0, messageRow_);
  for (uint8_t __i = 0; __i < maxCols_; __i++)
    lcd_.write(0x20);
  lcd_.setCursor(maxCols_ - min(maxCols_, strlen(message)), messageRow_);
  lcd_.print(message);
}

void EqLcd::showAlert(EqAlertType alert) {
  clear_();
  lcd_.print(F("ALERT"));
  showMessage(EqConfig::alertAsString(alert));
  lastSpeedDots_ = 0xFF;
}

void EqLcd::showCalibrating(uint8_t percents) {
  clear_();
  lcd_.print(F("Calibrating..."));
  showFanSpeed(false, percents);
}

EqLcd __lcd{};

/*
  specializations of EqDisplay
*/

#if (EQ_DISPLAY_TYPE == EQ_LCD_1602)
using EqDisplayLcd = EqDisplay<EQ_LCD_1602>;
#elif (EQ_DISPLAY_TYPE == EQ_LCD_2004)
using EqDisplayLcd = EqDisplay<EQ_LCD_2004>;
#endif

template <> bool EqDisplayLcd::init_() { return __lcd.init(); }

template <> void EqDisplayLcd::turnOff_() { __lcd.turnOff(); }

template <> void EqDisplayLcd::turnOn_() { __lcd.turnOn(); }

template <> void EqDisplayLcd::showHT_() { __lcd.showHT(); }

template <> void EqDisplayLcd::showOverdriveTime_() {
  __lcd.showOverdriveTime();
}

template <> void EqDisplayLcd::showFanSpeed_() { __lcd.showFanSpeed(); }

template <> void EqDisplayLcd::showMessage(const char *message) {
  __lcd.showMessage(message);
}

template <> void EqDisplayLcd::showAlert(EqAlertType alert) {
  __lcd.showAlert(alert);
}

template <> void EqDisplayLcd::showCalibrating(uint8_t percents) {
  __lcd.showCalibrating(percents);
}

#endif // (EQ_DISPLAY_TYPE == EQ_LCD_1602) || (EQ_DISPLAY_TYPE == EQ_LCD_2004)
