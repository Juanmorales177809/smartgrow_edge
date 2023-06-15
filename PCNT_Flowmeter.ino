const int     pinInterrupt  = 2;  //Pin de interrupción externa
volatile int  pulseCount = 0;     //Variable para almacenar el contador de pulsos

void handleInterrupt() 
  {
  pulseCount++;                   //Incrementa el contador de pulsos en cada interrupción
  }

void setup() 
  {
  pinMode(pinInterrupt, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinInterrupt), handleInterrupt, RISING);
  }

void loop() 
  {
  Serial.println(pulseCount);
  delay(1000);  // Espera 1 segundo antes de leer el contador nuevamente
  }