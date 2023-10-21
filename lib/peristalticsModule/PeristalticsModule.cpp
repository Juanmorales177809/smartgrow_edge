#include "PeristalticsModule.h"

const int bionovaA = 25;
const int bionovaB = 26;
const int phDown = 27;

void PeristalticsModule::acciones(String accion) {
    if (accion == "bionovaA") {
        TogglePeristaltic(bionovaA);
    } else if (accion == "bionovaB") {
        TogglePeristaltic(bionovaB);
    } else if (accion == "phDown") {
        TogglePeristaltic(phDown);
    }else{
        Serial.println("Accion no encontrada");
    }
    String msj = "Peristaltica " + accion + " activada";
    Serial.println(msj);
}

void TogglePeristaltic(int pin) {
    digitalWrite(pin, LOW);
    delay(1000);
    digitalWrite(pin, HIGH);
}

void PeristalticsModule::configInit() {
    pinMode(bionovaA, HIGH);
    pinMode(bionovaB, HIGH);
    pinMode(phDown, HIGH);
}