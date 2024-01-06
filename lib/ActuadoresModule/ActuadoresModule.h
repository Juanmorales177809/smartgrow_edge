#ifndef ActuadoresModule_h
#define ActuadoresModule_h

#include <Arduino.h>

extern const int ENTRADA_HIDROPONICO;
extern const int DESAGUE_HIDROPONICO;
extern const int RECIRCULACION_HIDROPONICO;
extern const int MOTO_BOMBA;

class ActuadoresModule
{
public:
    static void acciones(String mensaje);
    static void configInit();
};

void ToggleActuador(int actuador, const char *id);
void updateActuadores(const char *id, bool estado);

#endif