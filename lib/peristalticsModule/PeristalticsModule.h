#ifndef PeristalticsModule_h
#define PeristalticsModule_h

#include <Arduino.h>

extern const int bionovaA;
extern const int bionovaB;
extern const int phDown;


class PeristalticsModule {
    public:
    int estado;
    void acciones(String accion, bool estado);
    void configInit();
    void DownPeristaltic(int pin);
    void UpPeristaltic(int pin);
    
};



#endif