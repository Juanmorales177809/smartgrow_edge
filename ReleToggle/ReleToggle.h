#ifndef ReleToggle_h
#define ReleToggle_h

#include <Arduino.h>

class ReleToggle {
    public:
        ReleToggle(int pin);
        void begin();
        void update();
    private:
        int _pin;
        bool _state;
};

#endif