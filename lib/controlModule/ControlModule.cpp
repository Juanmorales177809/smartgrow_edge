#include "ControlModule.h"
#include "PeristalticsModule.h"
#include "ReadModule.h"

PeristalticsModule peristalticsModule;


ControlModule::ControlModule(){
    float setpoint_ph;
    float setpoint_ec;
    float tanque;
    float ph;
    float ec;
    float tmp;
    bool cOn = false;
    bool state_ec = false;
    bool state_ph = false;
    
    

}
void ControlModule::control_ec() {
        peristalticsModule.configInit();
        if (ec < setpoint_ec) {
            if (ec-setpoint_ec > setpoint_ec-200){
            peristalticsModule.acciones("bionovaA",false);
            peristalticsModule.acciones("bionovaB", false);
            Serial.print("Bionova A y B encendidos por 30 segundos");
            delay(30000);
            peristalticsModule.acciones("bionovaA", true);
            peristalticsModule.acciones("bionovaB", true);
            Serial.print("Bionova A y B apagados");
            delay(1000);
        }else if(ec-setpoint_ec < setpoint_ec/2){
            peristalticsModule.acciones("bionovaA", true);
            peristalticsModule.acciones("bionovaB", true);
            Serial.print("Bionova A y B encendidos por 15 segundos");
            delay(15000);
            peristalticsModule.acciones("bionovaA", false);
            peristalticsModule.acciones("bionovaB", false);
            Serial.print("Bionova A y B apagados");
            delay(1000);
        }
        }else if(ec > setpoint_ec+200){
            delay(1000);
        }else{
            Serial.println("EC estable");
            state_ec = true;
        }

        
    }

void ControlModule::control_ph() {
        if (ph > setpoint_ph+0.4) {
            peristalticsModule.acciones("phDown", true);
            Serial.print("phDown encendido por 5 segundos");
            delay(5000);
            peristalticsModule.acciones("phDown", false);
            Serial.print("phDown apagado");
        }else if (ph < setpoint_ph-0.2) {
            peristalticsModule.acciones("phUp", true);
            Serial.print("phUp encendido por 5 segundos");
            delay(5000);
            peristalticsModule.acciones("phUp", false);
            Serial.print("phUp apagado");
        }else {
            Serial.println("PH estable");
            cOn = true;
            state_ph = true;
        }

        
    }

void ControlModule::set_ph(float ph){
    ph = ph;
}
void ControlModule::set_ec(float ec){
    ec = ec;
}
void ControlModule::set_tmp(float tmp){
    tmp = tmp;
}

    
