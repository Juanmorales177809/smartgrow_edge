#include "ReleToggle.h"

ReleToggle::ReleToggle(int pin) {
    _pin = pin;
    _state = false;
}

void ReleToggle::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void ReleToggle::update() {
    _state = digitalRead(_pin);
    _state = !_state;
    digitalWrite(_pin, _state);
}
