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
    bool ec_bad = false;
    bool ph_bad = false;
    

}
void ControlModule::control_ec() {
        peristalticsModule.configInit();
        Serial.println(ec);
        if (ec < setpoint_ec-800 && ec!=0) {
            Serial.println("EC bajo");
            Serial.print("EC: ");
            Serial.println(ec);
            Serial.print("Setpoint: ");
            Serial.println(setpoint_ec);
            peristalticsModule.acciones("bionovaA", true);
            peristalticsModule.acciones("bionovaB", true);
            delay(8000); //tiempo de espera para 2.5 litros con agitador de 3w (4000) ec 800
            peristalticsModule.acciones("bionovaA", false);
            peristalticsModule.acciones("bionovaB", false);
            Serial.println("Bionova A y B activos");
        }else if(ec < setpoint_ec-100 && ec!=0){
            Serial.println("EC bajo");
            Serial.print("EC: ");
            Serial.println(ec);
            peristalticsModule.acciones("bionovaA",true);
            peristalticsModule.acciones("bionovaB", true);
            Serial.println("Bionova A y B activos");
            delay(3000); //tiempo de espera para 2.5 litros con agitador de 3w (500)
            peristalticsModule.acciones("bionovaA", false);
            peristalticsModule.acciones("bionovaB", false);
            Serial.println("Bionova A y B apagados");
        }else if(ec > setpoint_ec+500 && ec!=0){
            Serial.println("EC muy alto");
            Serial.println("EC: ");
            Serial.println(ec);
            ec_bad = true;
            Serial.println("Agregar agua...");
        }else if(ec > setpoint_ec-40 && ec < setpoint_ec+ 500 && ec!=0){
            Serial.println("EC estable");
            state_ec = true;
        }else if(ec==0 ){
            Serial.println("No hay lectura de EC");
        }
}

    

void ControlModule::control_ph() {
        
        if (ec_bad == true){
            Serial.println("No se puede controlar el PH, EC muy alto");
            
        }else{
        if (ph > setpoint_ph+0.1) {
            peristalticsModule.acciones("phDown", true);
            Serial.println("Regulando pH...");
            delay(500);
            peristalticsModule.acciones("phDown", false);
            Serial.println("phDown apagado");
        }else if (ph < setpoint_ph-0.2) {
            //peristalticsModule.acciones("phUp", true);
            Serial.println("pH acido");
            delay(25);
            //peristalticsModule.acciones("phUp", false);
            Serial.println("reiniciar solucion");
        }else if(ph >= setpoint_ph-0.05 && ph <= setpoint_ph+0.05) {
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



    
