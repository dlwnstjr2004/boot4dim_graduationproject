#ifndef FLEX_H
#define FLEX_H

#include "Arduino.h"

//#define PRINT_MIN_MAX
#define PRINT_VALUE
#define flex_pin1 34 // 중지
#define flex_pin2 33 // 검지
#define flex_pin3 32 // 엄지


#define INIT_LONG 2400
#define INIT_SHORT 1900
#define DEADLINE_LONG 2000
#define DEADLINE_SHORT 1800

class FlexSensor{
private:
    int Mid;
    int Idx;
    int Thumb;
    int MidMin;
    int MidMax;
    int IdxMin;
    int IdxMax;
    int ThumbMin;
    int ThumbMax;

public:
    FlexSensor();
    void getValue();
    void printReg();
    void initMinMax();
    int cmpInit(int flag);
    int changeTrigger();
    int isInitTrigger();
    int MidClicked();
    int IdxClicked();
    int ThumbClicked();

};
#endif