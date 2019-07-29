#include"arduino.h"
#include"Flex.h"

FlexSensor::FlexSensor(int PFlex0, int PFlex1, int PFlex2){

  _pflex0 = PFlex0;
  _pflex1 = PFlex1;
  _pflex2 = PFlex2;

  _flex0 = -1;
  _flex1 = -1;
  _flex2 = -1;
 }

 FlexSensor::FlexSensor(){

  _pflex0 = 33;
  _pflex1 = 34;
  _pflex2 = 35;

  _flex0 = -1;
  _flex1 = -1;
  _flex2 = -1;
 }

void FlexSensor::setVadcResult(){
  int analogValue = -1;
  
  analogValue = analogRead(_pflex0);
//  Serial.println(PINNUM);
  _flex0 = analogValue >= B22 ? 1 : 0;

  analogValue = analogRead(_pflex1);
  _flex1 = analogValue >= B45 ? 1 : 0;

  analogValue = analogRead(_pflex2);
  _flex2 = analogValue >= B45 ? 1 : 0;
  
//  Serial.println(*result);
}

void FlexSensor::returnValue(int *flex0, int *flex1, int *flex2){
  *flex0 = _flex0;
  *flex1 = _flex1;
  *flex2 = _flex2;
}