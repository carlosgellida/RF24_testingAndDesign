#include <BasicLinearAlgebra.h>
using namespace BLA;
#include <RadioPayload.h>

Matrix<4, 1> qDeseado = {1.0, 1.0, 1.0, 1.0}; 

Matrix<4, 1> qCurrent = {1.0, 1.0, 1.0, 1.0}; 

void setup() {

  Serial.begin(115200);
  
  InitialiceRadio() ;
} // setup

void loop() {

  sended = send(qCurrent) ; 
  
  recieved = recieve(qDeseado);  

} // loop
