#include "AS7265xModule.h"
#include "SparkFun_AS7265X.h"


//hacer un constructor con todos los canales del sensor AS7265X

AS7265xModule::AS7265xModule(const char* id)
{   
    AS7265X sensor;
    this->id = id;
    float A = 0.0f; //410nm
    float B = 0.0f; //435nm
    float C = 0.0f; //460nm
    float D = 0.0f; //485nm
    float E = 0.0f; //510nm
    float F = 0.0f; //535nm
    float G = 0.0f; //560nm
    float H = 0.0f; //585nm
    float R = 0.0f; //610nm
    float I = 0.0f; //645nm
    float S = 0.0f; //680nm
    float J = 0.0f; //705nm
    float T = 0.0f; //730nm
    float U = 0.0f; //760nm
    float V = 0.0f; //810nm
    float W = 0.0f; //860nm
    float K = 0.0f; //900nm
    float L = 0.0f; //
}

void AS7265xModule::begin()
{
    if (sensor.begin() == false)
    {
        Serial.println("Intentando encontrar AS7265X");
        delay(10000);
        if (sensor.begin() == false)
        {
            Serial.println("No se pudo encontrar el sensor AS7265X");
        
        }
        else
        {
            Serial.println("Sensor AS7265X encontrado");
        }
    }else{
        Serial.println("Sensor AS7265X encontrado");
    }
}
void AS7265xModule::setGain(const char* gain, const int integration){
    if (gain == "1X")
    {
        sensor.setGain(AS7265X_GAIN_1X);
    }
    else if (gain == "37X")
    {
        sensor.setGain(AS7265X_GAIN_37X);
    }
    else if (gain == "16X")
    {
        sensor.setGain(AS7265X_GAIN_16X);
    }
    else if (gain == "64X")
    {
        sensor.setGain(AS7265X_GAIN_64X);
    }
    else
    {
        Serial.println("Default gain 1X");
    }
    //Integration cycles is from 0 (2.78ms) to 255 (711ms) in steps of 2.78ms
    sensor.setIntegrationCycles(integration); //Default 50*2.8ms = 140ms per reading

}



void AS7265xModule::Read()
{
    

    sensor.takeMeasurements();

    this->A = sensor.getCalibratedA();
    this->B = sensor.getCalibratedB();
    this->C = sensor.getCalibratedC();
    this->D = sensor.getCalibratedD();
    this->E = sensor.getCalibratedE();
    this->F = sensor.getCalibratedF();
    this->G = sensor.getCalibratedG();
    this->H = sensor.getCalibratedH();
    this->R = sensor.getCalibratedR();
    this->I = sensor.getCalibratedI();
    this->S = sensor.getCalibratedS();
    this->J = sensor.getCalibratedJ();
    this->T = sensor.getCalibratedT();
    this->U = sensor.getCalibratedU();
    this->V = sensor.getCalibratedV();
    this->W = sensor.getCalibratedW();
    this->K = sensor.getCalibratedK();
    this->L = sensor.getCalibratedL();

    Serial.print("A: ");
    Serial.println(this->A);
    Serial.print("B: ");
    Serial.println(this->B);
    Serial.print("C: ");
    Serial.println(this->C);
    Serial.print("D: ");
    Serial.println(this->D);
    Serial.print("E: ");
    Serial.println(this->E);
    Serial.print("F: ");
    Serial.println(this->F);
    Serial.print("G: ");
    Serial.println(this->G);
    Serial.print("H: ");
    Serial.println(this->H);
    Serial.print("R: ");
    Serial.println(this->R);
    Serial.print("I: ");
    Serial.println(this->I);
    Serial.print("S: ");
    Serial.println(this->S);
    Serial.print("J: ");
    Serial.println(this->J);
    Serial.print("T: ");
    Serial.println(this->T);
    Serial.print("U: ");
    Serial.println(this->U);
    Serial.print("V: ");
    Serial.println(this->V);
    Serial.print("W: ");
    Serial.println(this->W);
    Serial.print("K: ");
    Serial.println(this->K);
    Serial.print("L: ");
    Serial.println(this->L);
    Serial.println("=====================================");







}     

const String AS7265xModule::buildJson()
{
    jsonDocument.clear();
    jsonDocument["A"] = this->A;
    jsonDocument["B"] = this->B;
    jsonDocument["C"] = this->C;
    jsonDocument["D"] = this->D;
    jsonDocument["E"] = this->E;
    jsonDocument["F"] = this->F;
    jsonDocument["G"] = this->G;
    jsonDocument["H"] = this->H;
    jsonDocument["R"] = this->R;
    jsonDocument["I"] = this->I;
    jsonDocument["S"] = this->S;
    jsonDocument["J"] = this->J;
    jsonDocument["T"] = this->T;
    jsonDocument["U"] = this->U;
    jsonDocument["V"] = this->V;
    jsonDocument["W"] = this->W;
    jsonDocument["K"] = this->K;
    jsonDocument["L"] = this->L;
    jsonDocument["sensor"] = this->id;
    String jsonString;
    serializeJson(jsonDocument, jsonString);
    return jsonString;
}




    

    
