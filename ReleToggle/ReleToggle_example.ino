#include "ReleToggle.h"

ReleToggle actuador1(13);

void setup() {
  actuador1.begin();
}

void loop() {
 actuador1.update();
 delay(5000);
}
