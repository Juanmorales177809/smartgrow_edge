#include "ControlModule.h"
#include "PeristalticsModule.h"
#include "ReadModule.h"
#include "MqttModule.h"

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
    bool ec_bad = false;
    peristalticsModule.configInit();
    

}
void ControlModule::control_ec() {
        Serial.println(ec);
        if (ec < setpoint_ec && ec!=0) {
            Serial.println("EC bajo");
            Serial.println(ec);
            Serial.println(setpoint_ec);
            if (ec-setpoint_ec > setpoint_ec-200){
            peristalticsModule.acciones("bionovaA",false);
            peristalticsModule.acciones("bionovaB", false);
            Serial.print("Bionova A y B encendidos por 30 segundos");
            delay(5000);
            peristalticsModule.acciones("bionovaA", true);
            peristalticsModule.acciones("bionovaB", true);
            Serial.print("Bionova A y B apagados");
            delay(1000);
        }else if(ec-setpoint_ec < setpoint_ec/2 && ec!=0){
            peristalticsModule.acciones("bionovaA", true);
            peristalticsModule.acciones("bionovaB", true);
            Serial.print("Bionova A y B encendidos por 15 segundos");
            delay(2500);
            peristalticsModule.acciones("bionovaA", false);
            peristalticsModule.acciones("bionovaB", false);
            Serial.print("Bionova A y B apagados");
            delay(1000);
        }
        }else if(ec > setpoint_ec && ec < setpoint_ec+ 500 && ec!=0){
            Serial.println("EC estable");
            state_ec = true;
        }else if(ec==0 ){
            Serial.println("No hay lectura de EC");
        }else if(ec > setpoint_ec+500){
            Serial.println("EC muy alto, activar desague");
            state_ec = true;
            ec_bad = true;
        }

}
    

void ControlModule::control_ph() {
        
        if (ec_bad == true){
            Serial.println("No se puede controlar el PH, EC muy alto");
            
        }else{
        if (ph > setpoint_ph+0.4) {
            peristalticsModule.acciones("phDown", true);
            Serial.print("phDown encendido por 5 segundos");
            delay(600);
            peristalticsModule.acciones("phDown", false);
            Serial.print("phDown apagado");
        }else if (ph < setpoint_ph-0.2) {
            peristalticsModule.acciones("phUp", true);
            Serial.print("phUp encendido por 5 segundos");
            delay(300);
            peristalticsModule.acciones("phUp", false);
            Serial.print("phUp apagado");
        }else {
            Serial.println("PH estable");
            cOn = true;
            state_ph = true;
        }

        }
    }

void ControlModule::set_ph(float ph_set){
    ph = ph_set;
}
void ControlModule::set_ec(float ec_set){
    ec = ec_set;
}
void ControlModule::set_tmp(float tmp_set){
    tmp = tmp_set;
}
void ControlModule::set_setpoint_ph(float setpoint_ph){
    setpoint_ph = setpoint_ph;
}
void ControlModule::set_setpoint_ec(float setpoint_ec){
    setpoint_ec = setpoint_ec;
}



    
