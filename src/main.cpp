#include <SPI.h>
#include "printf.h"
#include <RadioPayload.h>

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  initializeRadio() ; 
} // setup

void loop() {

  Qc = "3, 3, 3, 3";

  bool sended = sendRF() ; 

  bool recieved = recieveRF(); 

} // loop
