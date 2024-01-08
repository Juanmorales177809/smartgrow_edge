#include "ActuadoresModule.h"
#include "MqttModule.h"
#include "HttpModule.h"
#include <Arduino.h>
#include <ArduinoJson.h>

const int ENTRADA_HIDROPONICO = 14;
const int DESAGUE_HIDROPONICO = 27;
const int RECIRCULACION_HIDROPONICO = 12;
const int MOTO_BOMBA = 13;

void updateActuadores(const char *id, bool estado)
{
    StaticJsonDocument<200> jsonDocument;
    jsonDocument["estado"] = estado;
    String jsonString;
    serializeJson(jsonDocument, jsonString);
    // 200.122.207.134:8311
    HttpModule::enviarDatosActuadores("200.122.207.134", 8311, jsonString.c_str(), id);
}

void ActuadoresModule::ToggleActuador(int actuador, const char *id)
{
    if (digitalRead(actuador) == HIGH)
    {
        digitalWrite(actuador, LOW);
        if (id != "")
        {
            updateActuadores(id, true);
        }
    }
    else
    {
        digitalWrite(actuador, HIGH);
        if (id != "")
        {
            updateActuadores(id, false);
        }
    }
}

void ActuadoresModule::configInit()
{
    digitalWrite(ENTRADA_HIDROPONICO, HIGH);
    digitalWrite(DESAGUE_HIDROPONICO, HIGH);
    digitalWrite(RECIRCULACION_HIDROPONICO, HIGH);
    digitalWrite(MOTO_BOMBA, HIGH);

    const char *entrada_de_agua_id = "65999eeb0a40e026b2620ffa";
    const char *salida_de_agua_id = "65999ebf0a40e026b2620ff8";
    const char *recirculacion_id = "65999fdb0a40e026b2621002";
    updateActuadores(entrada_de_agua_id, false);
    updateActuadores(salida_de_agua_id, false);
    updateActuadores(recirculacion_id, false);
}
