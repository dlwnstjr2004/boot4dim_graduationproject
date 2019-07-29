//#define ANALOG_PIN_0 39 // VN -> ADC
//#define ANALOG_PIN_0 36 // VP -> ADC

#ifndef FLEX_H
#define FLEX_H

#include"arduino.h"

#define MIN_22 0
#define MAX_22 4095
#define MIN_45 0
#define MAX_45 4095
#define B22 2000
#define B45 1000

class FlexSensor{
public:
  FlexSensor(int PFlex0, int PFlex1, int PFlex2);
  FlexSensor();
  void setVadcResult();
  void returnValue(int *flex0, int *flex1, int *flex2);
private:
  int _pflex0;
  int _pflex1;
  int _pflex2;

  int _flex0;
  int _flex1;
  int _flex2;
};

#endif
