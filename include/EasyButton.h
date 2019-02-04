/**
 * EasyButton.h
 * @author Evert Arias
 * @version 1.0.0
 * @license MIT
 */

#ifndef _EasyButton_h
#define _EasyButton_h

#include "eq_config.h"
#include "eq_dpin.h"

template <uint8_t Pin> class EasyButton {
public:
  EasyButton(uint32_t dbTime = 35, bool puEnable = true, bool invert = true)
      : _db_time(dbTime), _pu_enabled(puEnable), _invert(invert) {}

  ~EasyButton() {}

  // Initialize a button object and the pin it's connected to.
  void init();

  // Returns the current debounced button state, true for pressed, false for
  // released.
  bool read();

  // Call a callback function when the button has been pressed and released.
  void onPressed(callback_t callback);

  // Call a callback function when the button has been held for at least the
  // given number of milliseconds.
  void onPressedFor(const uint32_t &duration, callback_t callback);

  // Call a callback function when the // given sequence has matched.
  void onSequence(const uint8_t &sequences, const uint32_t &duration,
                  callback_t callback);

  // Returns true if the button state was pressed at the last read.
  bool isPressed() const;

  // Returns true if the button state was released at the last read.
  bool isReleased() const;

  // Returns true if the button state at the last read was pressed.
  bool wasPressed() const;

  // Returns true if the button state at the last read was released.
  bool wasReleased() const;

  // Returns true if the button state at the last read was pressed, and has been
  // in that state for at least the given number of milliseconds.
  bool pressedFor(const uint32_t &duration) const;

  // Returns true if the button state at the last read was released, and has
  // been in that state for at least the given number of milliseconds.
  bool releasedFor(const uint32_t &duration) const;

private:
  uint32_t _short_press_count; // short press counter.
  uint32_t _first_press_time;  // time when button was pressed for first time.
  volatile uint8_t _press_sequences;          // number of sequences to count.
  volatile uint32_t _press_sequence_duration; // time limit of the sequence.
  volatile uint32_t _held_threshold;          // held threshold.
  bool _was_btn_held;                         // indicate if button was held.
  bool
      _held_callback_called; // indicate if button long press has been notified.
  // uint8_t _pin;      // Arduino pin number where the Button is connected to.
  uint32_t _db_time; // Debounce time (ms).
  bool _pu_enabled;  // Internal pullup resistor enabled.
  bool _invert;      // Inverts button logic. If true, low = pressed else high =
                     // pressed.
  volatile bool _current_state;   // current button state, true = pressed.
  volatile bool _last_state;      // previous button state, true = pressed.
  volatile bool _changed;         // has the state change since last read.
  volatile uint32_t _time;        // time of current state.
  volatile uint32_t _last_change; // time of last state change.

  // callbacks
  callback_t mPressedCallback;    // callback function for pressed events.
  callback_t mPressedForCallback; // callback function for pressedFor events.
  callback_t
      mPressedSequenceCallback; // callback function for pressedSequence events.
};

void template <uint8_t Pin> EasyButton::init() {
  // pinMode(_pin, _pu_enabled ? INPUT_PULLUP : INPUT);
  if (_pu_enabled)
    EqDPin<Pin>::setInputPulledUp();
  else
    EqDPin<Pin>::setInput();
  //_current_state = digitalRead(_pin);
  // if (_invert)
  //  _current_state = !_current_state;
  _current_state =
      _invert ? EqDPin<Pin>::isInputLow() : EqDPin<Pin>::isInputHigh();
  _time = millis();
  _last_state = _current_state;
  _changed = false;
  _last_change = _time;
}

void template <uint8_t Pin> EasyButton::onPressed(callback_t callback) {
  mPressedCallback = callback;
}

void template <uint8_t Pin>
EasyButton::onPressedFor(const uint32_t &duration, callback_t callback) {
  _held_threshold = duration;
  mPressedForCallback = callback;
}

void template <uint8_t Pin>
EasyButton::onSequence(const uint8_t &sequences, const uint32_t &duration,
                       callback_t callback) {
  _press_sequences = sequences;
  _press_sequence_duration = duration;
  mPressedSequenceCallback = callback;
}

bool template <uint8_t Pin> EasyButton::isPressed() const {
  return _current_state;
}

bool template <uint8_t Pin> EasyButton::isReleased() const {
  return !_current_state;
}

bool template <uint8_t Pin> EasyButton::wasPressed() const {
  return _current_state && _changed;
}

bool template <uint8_t Pin> EasyButton::wasReleased() const {
  return !_current_state && _changed;
}

bool template <uint8_t Pin>
EasyButton::pressedFor(const uint32_t &duration) const {
  return _current_state && _time - _last_change >= duration;
}

bool template <uint8_t Pin>
EasyButton::releasedFor(const uint32_t &duration) const {
  return !_current_state && _time - _last_change >= duration;
}

#endif // _EasyButton_h
