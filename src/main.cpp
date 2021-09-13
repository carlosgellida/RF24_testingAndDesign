#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(14, 12); // using pin 7 for the CE pin, and pin 8 for the CSN pin

// Let these addresses be used for the pair
uint8_t address[][6] = {"1Node", "2Node"};

bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit

bool role = false;  // true = TX role, false = RX role

float payload = 0.0;

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop
  }

  radioNumber = 0 ; 

  radio.setPALevel(RF24_PA_HIGH);  // RF24_PA_MAX is default.

  radio.setRetries(1, 5); // Number of tries to send the message

  radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[radioNumber]);     // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1

  // additional setup specific to the node's role
  if (role) {
    radio.stopListening();  // put radio in TX mode
  } else {
    radio.startListening(); // put radio in RX mode
  }

  unsigned long time0 = micros(); 
} // setup

void loop() {

    // additional setup specific to the node's role
  if (role) {
    radio.stopListening();  // put radio in TX mode
  } else {
    radio.startListening(); // put radio in RX mode
  }

  if (role) {
    // This device is a TX node

    payload = 20 ;                       // start the timer
    bool report = radio.write(&payload, sizeof(float));      // transmit & save the report

    if (report) {

      Serial.println(micros()/1000);

      role = !role ; // Cambiar de rol
    } else {
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
      role = !role; 
    }

    // to make this example readable in the serial monitor

  } else {
    // This device is a RX node

    uint8_t pipe;
    if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
      radio.read(&payload, bytes);            // fetch payload from FIFO
      
      role = !role ; // Cambiar de rol
    }
  } // role

} // loop
