#include "ReadModule.h"
#include <Ezo_i2c.h>
#include <Ezo_i2c_util.h> 
#include "ph_grav.h"


Gravity_pH pH = Gravity_pH(32);
Ezo_board EC = Ezo_board(100, "EC");
Ezo_board RTD = Ezo_board(102, "RTD"); 

ReadModule::ReadModule(){
    
    char EC_data[32];
    char *EC_str;                     
    char *TDS;                      
    float EC_float;
    float TDS_float;
    float ph;                 
    float temp_float;                
    float analog_ph;
    float voltage;
    char computerdata[20];
    char RTD_data[20]; 
    float tmp_float; 

}

void ReadModule::tds_activate(){
    EC.send_cmd("o,tds,1"); 
    delay(3000);
}

void ReadModule::send_a_read_ec(){
    EC.send_cmd("r");
    delay(3000);
}

void ReadModule::send_a_read_temp(){
    RTD.send_read_cmd();
}

float ReadModule::receive_and_print_reading_EC(){

    //EC.send_cmd_with_num("T,", 23);
    EC.receive_cmd(EC_data, 32);
    EC_str = strtok(EC_data, ",");       //let's parse the string at each comma.
    TDS = strtok(NULL, ",");
    EC_float=atof(EC_str);               //convert char to float.
    TDS_float=atof(TDS);              //convert char to float.
    return EC_float;

}

float ReadModule::receive_and_print_reading_Temp(){
    RTD.receive_cmd(RTD_data, 20); 
    tmp_float = atof(RTD_data);
    return tmp_float;
}
float ReadModule::receive_and_print_reading_pH(){

    voltage = pH.read_voltage();
    ph = pH.read_ph(voltage);                      
    analog_ph = float(ph);
    return analog_ph;
}
void ReadModule::verifica_ph(){
    if (pH.begin()) {                                     
    Serial.println("Loaded EEPROM");
  }
  else {
    Serial.println("Error loading EEPROM");
  }
}

void ReadModule::parse_cmd(char* string) {
    strupr(string);                                
  if (strcmp(string, "CAL,7") == 0) {       
    pH.cal_mid();                                
    Serial.println("MID CALIBRATED");
  }
  else if (strcmp(string, "CAL,4") == 0) {            
    pH.cal_low();                                
    Serial.println("LOW CALIBRATED");
  }
  else if (strcmp(string, "CAL,10") == 0) {      
    pH.cal_high();                               
    Serial.println("HIGH CALIBRATED");
  }
  else if (strcmp(string, "CAL,CLEAR") == 0) { 
    pH.cal_clear();                              
    Serial.println("CALIBRATION CLEARED");
  }
}



