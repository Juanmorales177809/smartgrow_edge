#ifndef ControlModule_h
#define ControlModule_h

#include "PeristalticsModule.h"

#include <Arduino.h>



class ControlModule {
    public:
    ControlModule();
    void control_ec();
    void control_ph();
    void set_ph(float ph);
    void set_ec(float ec);
    void set_tmp(float tmp);
    void set_setpoint_ec(float setpoint_ec);
    void set_setpoint_ph(float setpoint_ph);
    float setpoint_ph;
    float setpoint_ec;
    float tanque;
    float ph;
    float ec;
    float tmp;
    bool cOn;
    bool state_ec;
    bool state_ph;
    bool ec_bad;
    bool control_state;

};

#endif