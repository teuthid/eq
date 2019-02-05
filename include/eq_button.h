/*
   eq - transcendental fan controller ;)
   Copyright (c) 2018-2019 Mariusz Przygodzki
*/

// Based on the code from EasyButton
// https://github.com/evert-arias/EasyButton
// Author: Evert Arias

#ifndef __EQ_BUTTON_H__
#define __EQ_BUTTON_H__

#include "eq_config.h"
#include "eq_dpin.h"

template <uint8_t Pin> class EqButton;
using EqButtonBacklight = EqButton<EqConfig::buttonBacklightPin>;
using EqButtonOverdrive = EqButton<EqConfig::buttonOverdrivePin>;

template <uint8_t Pin> class EqButton {
  friend EqButtonBacklight &eqButtonBacklight();
  friend EqButtonOverdrive &eqButtonOverdrive();

public:
  // Initialize a button object and the pin it's connected to.
  void init();

  // Returns the current debounced button state, true for pressed, false for
  // released.
  bool update();

  // Call a callback function when the button has been pressed and released.
  void onPressed(callback_t callback);

  // Call a callback function when the button has been held for at least the
  // given number of milliseconds.
  void onPressedFor(const uint32_t &duration, callback_t callback);

  // Call a callback function when the // given sequence has matched.
  void onSequence(const uint8_t &sequences, const uint32_t &duration,
                  callback_t callback);

  // Returns true if the button state was pressed at the last read.
  constexpr bool isPressed() const;

  // Returns true if the button state was released at the last read.
  constexpr bool isReleased() const;

  // Returns true if the button state at the last read was pressed.
  constexpr bool wasPressed() const;

  // Returns true if the button state at the last read was released.
  constexpr bool wasReleased() const;

  // Returns true if the button state at the last read was pressed, and has been
  // in that state for at least the given number of milliseconds.
  constexpr bool pressedFor(const uint32_t &duration) const;

  // Returns true if the button state at the last read was released, and has
  // been in that state for at least the given number of milliseconds.
  constexpr bool releasedFor(const uint32_t &duration) const;

  void read(); // needs specialization

  EqButton(const EqButton &) = delete;
  EqButton(EqButton &&) = delete;
  void operator=(const EqButton &) = delete;

private:
  uint32_t shortPressCount_; // short press counter.
  uint32_t firstPressTime_;  // time when button was pressed for first time.
  volatile uint8_t pressSequences_;         // number of sequences to count.
  volatile uint32_t pressSequenceDuration_; // time limit of the sequence.
  volatile uint32_t heldThreshold_;         // held threshold.
  bool wasHeld_;                            // indicate if button was held.
  bool heldCallbackCalled_; // indicate if button long press has been notified.
  volatile bool puEnabled_; // internal pullup resistor enabled.
  volatile bool invert_;    // inverts button logic, ff true, low = pressed else
                            // high = pressed.
  volatile bool currentState_;   // current button state, true = pressed.
  volatile bool lastState_;      // previous button state, true = pressed.
  volatile bool changed_;        // has the state change since last read.
  volatile uint32_t time_;       // time of current state.
  volatile uint32_t lastChange_; // time of last state change.

  // callbacks
  callback_t pressedCallback_;    // callback function for pressed events.
  callback_t pressedForCallback_; // callback function for pressedFor events.
  callback_t
      pressedSequenceCallback_; // callback function for pressedSequence events.

  constexpr EqButton(bool puEnabled, bool invert)
      : puEnabled_(puEnabled), invert_(invert) {}

  void connect_(); // needs specializations

  static constexpr uint32_t dbTime_ = EqConfig::buttonDebounceTime;
  static EqButton instance_;
};

inline EqButtonBacklight &eqButtonBacklight() {
  return EqButtonBacklight::instance_;
}
inline EqButtonOverdrive &eqButtonOverdrive() {
  return EqButtonOverdrive::instance_;
}

template <uint8_t Pin> void EqButton<Pin>::init() {
  if (puEnabled_)
    EqDPin<Pin>::setInputPulledUp();
  else
    EqDPin<Pin>::setInput();
  currentState_ =
      invert_ ? EqDPin<Pin>::isInputLow() : EqDPin<Pin>::isInputHigh();
  time_ = millis();
  lastState_ = currentState_;
  changed_ = false;
  lastChange_ = time_;
  connect_();
}

template <uint8_t Pin> void EqButton<Pin>::onPressed(callback_t callback) {
  pressedCallback_ = callback;
}

template <uint8_t Pin>
void EqButton<Pin>::onPressedFor(const uint32_t &duration,
                                 callback_t callback) {
  heldThreshold_ = duration;
  pressedForCallback_ = callback;
}

template <uint8_t Pin>
void EqButton<Pin>::onSequence(const uint8_t &sequences,
                               const uint32_t &duration, callback_t callback) {
  pressSequences_ = sequences;
  pressSequenceDuration_ = duration;
  pressedSequenceCallback_ = callback;
}

template <uint8_t Pin> constexpr bool EqButton<Pin>::isPressed() const {
  return currentState_;
}

template <uint8_t Pin> constexpr bool EqButton<Pin>::isReleased() const {
  return !currentState_;
}

template <uint8_t Pin> constexpr bool EqButton<Pin>::wasPressed() const {
  return currentState_ && changed_;
}

template <uint8_t Pin> constexpr bool EqButton<Pin>::wasReleased() const {
  return !currentState_ && changed_;
}

template <uint8_t Pin>
constexpr bool EqButton<Pin>::pressedFor(const uint32_t &duration) const {
  return currentState_ && time_ - lastChange_ >= duration;
}

template <uint8_t Pin>
constexpr bool EqButton<Pin>::releasedFor(const uint32_t &duration) const {
  return !currentState_ && time_ - lastChange_ >= duration;
}

template <uint8_t Pin> bool EqButton<Pin>::update() {
  uint32_t __readStartedMs = millis();
  bool pinVal =
      invert_ ? EqDPin<Pin>::isInputLow() : EqDPin<Pin>::isInputHigh();
  if (__readStartedMs - lastChange_ < dbTime_) {
    changed_ = false;
  } else { // button's state has changed.
    lastState_ = currentState_;
    currentState_ = pinVal;
    changed_ = (currentState_ != lastState_);
    if (changed_)
      lastChange_ = __readStartedMs;
  }

  // call the callback functions when conditions met.
  if (!currentState_ && changed_) {
    if (!wasHeld_) {
      if (shortPressCount_ == 0)
        firstPressTime_ = __readStartedMs;
      shortPressCount_++;
      if (pressedCallback_)
        pressedCallback_();
      if (shortPressCount_ == pressSequences_ &&
          pressSequenceDuration_ >= (__readStartedMs - firstPressTime_)) {
        if (pressedSequenceCallback_)
          pressedSequenceCallback_();
        shortPressCount_ = 0;
        firstPressTime_ = 0;
      }
      // if secuence timeout, reset short presses counters.
      else if (pressSequenceDuration_ <= (__readStartedMs - firstPressTime_)) {
        shortPressCount_ = 0;
        firstPressTime_ = 0;
      }
    } else // button was not held.
      wasHeld_ = false;
    // since button released, reset pressedForCallback_Called value.
    heldCallbackCalled_ = false;
  }

  // button is not released.
  else if (currentState_ && time_ - lastChange_ >= heldThreshold_ &&
           pressedForCallback_) {
    wasHeld_ = true; // button has been pressed for at least the given time
    shortPressCount_ = 0;
    firstPressTime_ = 0;
    if (pressedForCallback_ && !heldCallbackCalled_) {
      heldCallbackCalled_ = true; // set as called.
      pressedForCallback_();
    }
  }

  time_ = __readStartedMs;
  return currentState_;
}

#endif // __EQ_BUTTON_H__
