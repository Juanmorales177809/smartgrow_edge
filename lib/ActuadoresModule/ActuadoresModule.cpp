#include "ActuadoresModule.h"
#include "MqttModule.h"
#include "HttpModule.h"
#include <Arduino.h>
#include <ArduinoJson.h>

const int ENTRADA_HIDROPONICO = 14;
const int DESAGUE_HIDROPONICO = 27;
const int RECIRCULACION_HIDROPONICO = 12;
const int MOTO_BOMBA = 13;

void ActuadoresModule::acciones(String mensaje)
{
    if (mensaje == "entrada_de_agua_hidroponico")
    {
        ToggleActuador(ENTRADA_HIDROPONICO, "65999eeb0a40e026b2620ffa");
    }
    else if (mensaje == "desague_hidroponico")
    {
        if (digitalRead(RECIRCULACION_HIDROPONICO) == HIGH)
        {
            ToggleActuador(DESAGUE_HIDROPONICO, "65999ebf0a40e026b2620ff9");
            ToggleActuador(MOTO_BOMBA, "");
        }
        else
        {
            Serial.println("No se puede desaguar si esta activa la recirculacion");
        }
    }
    else if (mensaje == "recirculacion_hidroponico")
    {
        if (digitalRead(DESAGUE_HIDROPONICO) == LOW)
        {
            ToggleActuador(DESAGUE_HIDROPONICO, "65999ebf0a40e026b2620ff9");
            ToggleActuador(MOTO_BOMBA, "");
            delay(1000);
        }
        ToggleActuador(RECIRCULACION_HIDROPONICO, "65999fdb0a40e026b2621002");
        ToggleActuador(MOTO_BOMBA, "");
    }
    else
    {
        Serial.println("Mensaje no reconocido");
    }
    String msj = "Entrada de agua: " + String(digitalRead(ENTRADA_HIDROPONICO)) + "\n" +
                 "Desague: " + String(digitalRead(DESAGUE_HIDROPONICO)) + "\n" +
                 "Recirculacion: " + String(digitalRead(RECIRCULACION_HIDROPONICO)) + "\n" +
                 "Moto bomba: " + String(digitalRead(MOTO_BOMBA)) + "\n";
    Serial.println(msj);
}

void updateActuadores(const char *id, bool estado)
{
    StaticJsonDocument<200> jsonDocument;
    jsonDocument["estado"] = estado;
    String jsonString;
    serializeJson(jsonDocument, jsonString);
    // 200.122.207.134:8311
    HttpModule::enviarDatosActuadores("192.168.1.103", 3000, jsonString.c_str(), id);
}

void ToggleActuador(int actuador, const char *id)
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
    const char *salida_de_agua_id = "65999ebf0a40e026b2620ff9";
    const char *recirculacion_id = "65999fdb0a40e026b2621002";
    updateActuadores(entrada_de_agua_id, false);
    updateActuadores(salida_de_agua_id, false);
    updateActuadores(recirculacion_id, false);
}
