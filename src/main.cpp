#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(14, 12); // using pin 7 for the CE pin, and pin 8 for the CSN pin

// Let these addresses be used for the pair
uint8_t address[][6] = {"1Node", "2Node"};

bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit

// Used to control whether this node is sending or receiving
bool role = true;  // true = TX role, false = RX role

String payload ; 

unsigned long timming ; 
unsigned long time1 ; 

String Qd, Qc ; 

void initializeRadio() {
      radio.begin();
      role = true; 
      radioNumber = 1; 
      radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
      radio.setRetries(1, 6); 
      radio.setPayloadSize(sizeof(payload));
      radio.openWritingPipe(address[radioNumber]);
      radio.openReadingPipe(1, address[!radioNumber]); 
        if (role) {
         radio.stopListening();  // put radio in TX mode
        } else {
          radio.startListening(); // put radio in RX mode
        }
      timming = micros(); 
}

void sendAndRecieveRF(String *Qc, String *Qd){
  if ((micros() - timming) > 10000){

  if (role) {
    time1 = micros(); 
    payload = *Qc ; //current Quaternion

    bool report = radio.write(&payload, sizeof(payload));      // transmit & save the report
                   // end the timer

    if (report) {
      role = !role  ;
      radio.startListening();
      timming = micros(); 
    } else {
      // Sí no puede transmitir reiniciar el radio
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
      initializeRadio();
    }

    } else {
      // Sí el tiempo se ha excedido escuchando reiniciar el radio
      initializeRadio();
      Serial.println("radio inicializado nuevamente");
    }

  } else {
    // This device is a RX node

    uint8_t pipe;
    if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
      radio.read(&payload, bytes);            // fetch payload from FIFO
      *Qd = payload; 
      role = !role  ;
      radio.stopListening();
      Serial.print(F("time taken for send and recieve: "));
      Serial.println(micros() - time1);                 // print the timer result
    }
  } // role
}

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  initializeRadio() ; 
} // setup

void loop() {

  /* if ((micros() - timming) > 10000){

  if (role) {
    time1 = micros(); 
    payload = "2, 2, 2, 2" ;
    bool report = radio.write(&payload, sizeof(payload));      // transmit & save the report
                   // end the timer

    if (report) {
      //Serial.print(F("Transmission successful! "));          // payload was delivered
      //Serial.print(F(" us. Sent: "));
      //Serial.println(payload);                               // print payload sent
      role = !role  ;
      radio.startListening();
      timming = micros(); 
    } else {
      // Sí no puede transmitir reiniciar el radio
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
      initializeRadio();
    }

    } else {
      // Sí el tiempo se ha excedido escuchando reiniciar el radio
      initializeRadio();
      Serial.println("radio inicializado nuevamente");
    }

  } else {
    // This device is a RX node

    uint8_t pipe;
    if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
      radio.read(&payload, bytes);            // fetch payload from FIFO
      //Serial.print(F("Received "));
      //Serial.print(bytes);                    // print the size of the payload
      //Serial.print(F(" bytes on pipe "));
      //Serial.print(pipe);                     // print the pipe number
      //Serial.print(F(": "));
      //Serial.println(payload);                // print the payload's value
      role = !role  ;
      radio.stopListening();
      Serial.print(F("time taken for send and recieve: "));
      Serial.println(micros() - time1);                 // print the timer result
    }
  } // role*/

  sendAndRecieveRF(&Qc, &Qd) ; 

} // loop
