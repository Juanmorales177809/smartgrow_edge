#ifndef ReadModule_h
#define ReadModule_h
#include <Arduino.h>



class ReadModule {
    public:
    ReadModule();
    void tds_activate();
    void send_a_read_ec();
    void send_a_read_temp();
    float receive_and_print_reading_EC();
    float receive_and_print_reading_Temp();
    float receive_and_print_reading_pH();
    void parse_cmd(char *cmd);
    void verifica_ph();

    char EC_data[32];//The data we want to store
    char *EC_str;     //Pointer to the data we want to store                
    char *TDS;         //Pointer to the data we want to store             
    float EC_float;//The float version of our data
    float TDS_float;//The float version of our data


    float ph;                 
    float temp_float;                
    float analog_ph;
    float voltage;

    char computerdata[20];
    char RTD_data[20]; 
    float tmp_float; 

};



#endif




