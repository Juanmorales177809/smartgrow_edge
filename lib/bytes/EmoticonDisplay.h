#ifndef emoticon_h
#define emoticon_h

#include <Arduino.h>

class EmoticonDisplay{
    public:
        EmoticonDisplay();
        const byte* getCelsius();
        const byte* getSmiley();
        const byte* getFrownie();
        const byte* getEmail();
    private:
        
        const byte Celsius[8] = { 
                        0b00110,
                        0b01001,
                        0b01000,
                        0b01000,
                        0b01001,
                        0b00110,
                        0b00000,
                        0b00000
                        };
    
    const byte smiley[8] = {
                        0b00000,
                        0b00000,
                        0b01010,
                        0b00000,
                        0b00000,
                        0b10001,
                        0b01110,
                        0b00000
                        };

    const byte frownie[8] = { 
                        0b00000,
                        0b00000,
                        0b01010,
                        0b00000,
                        0b00000,
                        0b00000,
                        0b01110,
                        0b10001
                        };
    const byte email[8] = { 
                        0b00000,
                        0b00000,
                        0b01010,
                        0b00000,
                        0b00000,
                        0b00000,
                        0b01110,
                        0b10001
                        };
};


#endif

