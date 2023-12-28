#include "DisplayModule.h"
#include <EmoticonDisplay.h>
#include <LiquidCrystal_I2C.h> 


#define lcd_addr 0x27
#define lcd_cols 16
#define lcd_rows 2

LiquidCrystal_I2C lcd(lcd_addr, lcd_cols, lcd_rows);
EmoticonDisplay bytes;

byte Celsius[8] = {
0b00110,
0b01001,
0b01000,
0b01000,
0b01001,
0b00110,
0b00000,
0b00000
};
byte smiley[8] = {
0b00000,
0b00000,
0b01010,
0b00000,
0b00000,
0b10001,
0b01110,
0b00000
};

byte frownie[8] = {
0b00000,
0b00000,
0b01010,
0b00000,
0b00000,
0b00000,
0b01110,
0b10001
};

byte email[8] = { 
                    0b11111,
                    0b00000,
                    0b01110,
                    0b00000,
                    0b00100,
                    0b00000,
                    0b00100,
                    0b00000
};


void DisplayModule::display(){
    LiquidCrystal_I2C lcd(lcd_addr, lcd_cols, lcd_rows);
    EmoticonDisplay bytes;
}
void DisplayModule::init_display(){
    lcd.init();
    lcd.backlight();
}