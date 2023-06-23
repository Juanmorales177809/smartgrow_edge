#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c 
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1  
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
float value1 = 123.45;
float value2 = 67.89;
float value3 = 10.11;

void setup() {
  Serial.begin(115200); // Initialize I2C communication
  display.begin(i2c_Address, true); // Initialize OLED display
//
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(5);
  display.setTextWrap(false);
//  display.setCursor(40, 0);
//  display.print("pH: ");
//  display.print(value1);
//  display.setCursor(40, 10);
//  display.print("EC: ");
//  display.print(value2);
//  display.setCursor(40, 20);
//  display.print("T: ");
//  display.print(value3);
//  display.display();


  while (1) {
    scrollText("pH: " + String(value1, 2)); // Convertir y mostrar el valor flotante 1
    scrollText("EC: " + String(value2, 2)); // Convertir y mostrar el valor flotante 2
    scrollText("Temp: " + String(value3, 2)); // Convertir y mostrar el valor flotante 3
  }
}

void loop() {
  // Tu código aquí
}

void scrollText(String text) {
  int textWidth = text.length() * 30;
  int xPos = display.width();

  while (xPos >= -textWidth) {
    display.clearDisplay();
    display.setCursor(xPos, 0);
    display.print(text);
    display.display();
    delay(10);
    xPos--;
  }
}
