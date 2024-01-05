#ifndef DisplayModule_h
#define DisplayModule_h

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define lcd_addr 0x27
#define lcd_cols 16
#define lcd_rows 2

class DisplayModule {
    public:
    void display();
    void init_display();
    byte Celsius[8];
    byte smiley[8];
    byte frownie[8];
    byte email[8];
    byte wifi[8];
    void clear();
    void print(String text, int row, int col);
    



    
};
#endif