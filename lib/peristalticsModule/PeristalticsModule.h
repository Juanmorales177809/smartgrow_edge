#ifndef PeristalticsModule_h
#define PeristalticsModule_h

#include <Arduino.h>

extern const int bionovaA;
extern const int bionovaB;
extern const int phDown;

class PeristalticsModule {
    public:
    static void acciones(String accion);
    static void configInit();
};

void TogglePeristaltic(int pin);

#endif