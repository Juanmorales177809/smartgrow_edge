#ifndef ControlModule_h
#define ControlModule_h

#include <Arduino.h>

class ControlModule {
    public:
    static void read_parameters();
    static void control();
};