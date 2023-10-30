#include "PeristalticsModule.h"

const int bionovaA = 33;
const int bionovaB = 34;
const int phDown = 35;
int estado = false;

void PeristalticsModule::acciones(String accion, bool estado) {
    if (accion == "bionovaA" && estado) {
        UpPeristaltic(bionovaA);
        Serial.println("Peristaltica bionovaA activada");
    } else if (accion == "bionovaB" && estado) {
        UpPeristaltic(bionovaB);
        Serial.println("Peristaltica bionovaB activada");
    } else if (accion == "phDown"&& estado) {
        UpPeristaltic(phDown);
        Serial.println("Peristaltica phDown activada");
    }else if (accion == "bionovaA" && !estado) {
        DownPeristaltic(bionovaA);
        Serial.println("Peristaltica bionovaA desactivada");
    }else if (accion == "bionovaB" && !estado) {
        DownPeristaltic(bionovaB);
        Serial.println("Peristaltica bionovaB desactivada");
    }else if (accion == "phDown" && !estado) {
        DownPeristaltic(phDown);
        Serial.println("Peristaltica phDown desactivada");
    }else{
        Serial.println("Error en la accion");
    }
    //String msj = "Peristaltica " + accion + " activada";
    //Serial.println(msj);
}

void UpPeristaltic(int pin) {
    digitalWrite(pin, HIGH);
    estado = true;

    
}
void DownPeristaltic(int pin) {
    digitalWrite(pin, LOW);
    estado = false;
}

void PeristalticsModule::configInit() {
    pinMode(bionovaA, OUTPUT);
    pinMode(bionovaB, OUTPUT);
    pinMode(phDown, OUTPUT);
}