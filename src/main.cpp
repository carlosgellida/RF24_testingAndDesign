#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include <BasicLinearAlgebra.h>

using namespace BLA;

// instantiate an object for the nRF24L01 transceiver
RF24 radio(14, 12); // using pin 7 for the CE pin, and pin 8 for the CSN pin

// Let these addresses be used for the pair
uint8_t address[][6] = {"1Node", "2Node"};
bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit


bool role = true;  // true = TX role, false = RX role
float payload = 0.0;

unsigned long timming ; 

bool sended, recieved ; 

Matrix<4, 1> qDeseado = {3.0, 3.0, 3.0, 3.0}; 

Matrix<4, 1> qCurrent = {3.0, 3.0, 3.0, 3.0}; 

/* String arr2str(Matrix<4, 1> arr){
  String str ;
  str = String(arr(0)) + ',' + String(arr(1)) + ',' + String(arr(2)) + ',' + String(arr(3));
  return str; 
}

Matrix<4, 1> str2arr(String str){
  Matrix<4, 1> arr ; 
  int j=0; 
  for(int i=0; i < 4; i++){
    String str2 ; 
    while(true){
      if(str[j] != ',' && (j < 16)){
        //Serial.println(str[j]); 
        str2 = str2 + str[j] ; 
        j++ ; 
      } else {
        j++ ; 
        break ; 
      }
    } //while
    //Serial.println(str2); 
    arr(i) = str2.toFloat() ; 
  } //for
  return arr; 
} */

void InitialiceRadio() {
    if (!radio.begin()) {
      Serial.println(F("radio hardware is not responding!!"));
      while (1) {} // hold in infinite loop
    }
    radioNumber = 1; 

    radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.

    radio.setRetries(1, 5); 

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit a float

    radio.setPayloadSize(sizeof(qDeseado)); // float datatype occupies ?? bytes

    radio.setChannel(2475); 

    radio.setDataRate(RF24_2MBPS); 

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

    timming = micros(); 
}

bool send(Matrix<4, 1> qCurrent){
  bool sended = false; 

  if ((micros() - timming) > 9000){
  //String qCurrentString = arr2str(qCurrent); 

  if (role) {
    // This device is a TX node
    //payload += 0.01;   // increment float payload
    //unsigned long start_timer = micros();                    // start the timer
    radio.stopListening();
    //bool report = radio.write(&qCurrentString, sizeof(qCurrentString));      // transmit & save the report
    bool report = radio.write(&qCurrent, sizeof(qCurrent));  

    if (report) {
      Serial.print(F("Transmission successful! "));          // payload was delivered
      Serial.print(F("current time = "));
      Serial.print((micros() - timming)/1000);                 // print the timer result
      Serial.print(F(" ms. Sent: "));
      //Serial.println(qCurrentString);              // print payload sent
      Serial << qCurrent << '\n' ; 
      role = !role  ;
      radio.startListening();
      timming = micros(); 
      sended = true; 
    } else {
      // Sí no puede transmitir reiniciar el radio
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
      //role = !role  ;
      //radio.startListening();
      //timming = micros(); 
      InitialiceRadio();
    }

    } else {
      // Sí el tiempo se ha excedido escuchando reiniciar el radio
      role = !role  ;
      InitialiceRadio();
    }
  } 
  return sended; 
}

bool recieve(Matrix<4, 1> &qDeseado){
  bool recieved = false; 
  if(!role) {
    String qDeseadoString ; 
    // This device is a RX node

    uint8_t pipe;
    radio.startListening(); 
    //Serial.println("listening"); 
    if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
      //radio.read(&qDeseadoString, bytes);            // fetch payload from FIFO
      radio.read(&qDeseado, bytes) ; 
      //qDeseado = str2arr(qDeseadoString) ; 
      Serial.print(F("Received "));
      Serial.print(bytes);                    // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);                     // print the pipe number
      Serial.print(F(": "));
      Serial << qDeseado << '\n';                // print the payload's value
      //Serial.println(qDeseadoString); 
      role = !role  ;
      recieved = true; 
      radio.stopListening();
    }
  } // role
  return recieved; 
}

void setup() {

  Serial.begin(115200);
  
  InitialiceRadio() ;
} // setup

void loop() {

  sended = send(qCurrent) ; 
  
  recieved = recieve(qDeseado);  

} // loop
