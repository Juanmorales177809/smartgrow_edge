#include "ActuadoresModule.h"
#include "MqttModule.h"

const int ENTRADA_HIDROPONICO = 14;
const int DESAGUE_HIDROPONICO = 27;
const int RECIRCULACION_HIDROPONICO = 12;
const int MOTO_BOMBA = 13;

void ActuadoresModule::acciones(String mensaje)
{
    if (mensaje == "entrada_de_agua_hidroponico")
    {
        ToggleActuador(ENTRADA_HIDROPONICO);
    }
    else if (mensaje == "desague_hidroponico")
    {
        if (digitalRead(RECIRCULACION_HIDROPONICO) == HIGH)
        {
            ToggleActuador(DESAGUE_HIDROPONICO);
            ToggleActuador(MOTO_BOMBA);
        } else {
            Serial.println("No se puede desaguar si esta activa la recirculacion");
        }
    }
    else if (mensaje == "recirculacion_hidroponico")
    {
        if (digitalRead(DESAGUE_HIDROPONICO) == LOW)
        {
            ToggleActuador(DESAGUE_HIDROPONICO);
            ToggleActuador(MOTO_BOMBA);
            delay(1000);
        }
        ToggleActuador(RECIRCULACION_HIDROPONICO);
        ToggleActuador(MOTO_BOMBA);
    }
    else {
        Serial.println("Mensaje no reconocido");
    }
    String msj = "Entrada de agua: " + String(digitalRead(ENTRADA_HIDROPONICO)) + "\n" +
                 "Desague: " + String(digitalRead(DESAGUE_HIDROPONICO)) + "\n" +
                 "Recirculacion: " + String(digitalRead(RECIRCULACION_HIDROPONICO)) + "\n" +
                 "Moto bomba: " + String(digitalRead(MOTO_BOMBA)) + "\n";
    Serial.println(msj);
}

void ToggleActuador(int actuador)
{
    if (digitalRead(actuador) == HIGH)
    {
        digitalWrite(actuador, LOW);
    }
    else
    {
        digitalWrite(actuador, HIGH);
    }
}

void ActuadoresModule::configInit()
{
    digitalWrite(ENTRADA_HIDROPONICO, HIGH);
    digitalWrite(DESAGUE_HIDROPONICO, HIGH);
    digitalWrite(RECIRCULACION_HIDROPONICO, HIGH);
    digitalWrite(MOTO_BOMBA, HIGH);
}
