#include <BasicLinearAlgebra.h>
using namespace BLA;
#include <RadioPayload2.h>

Matrix<4, 1> qDeseado = {1.0, 1.0, 1.0, 1.0}; 

Matrix<4, 1> qCurrent = {1.0, 1.0, 1.0, 1.0}; 

unsigned long timeLast ; 

void setup() {

  Serial.begin(115200);
  
  InitialiceRadio() ;

  timeLast = micros() ; 
} // setup

void loop() {

  if(micros() - timeLast > 8000){
    timeLast = micros(); 
    sended = send(qCurrent) ; 
  
    recieved = recieve(qDeseado);  

  } //if time

} // loop
