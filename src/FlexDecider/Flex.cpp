#include "Flex.h"

FlexSensor::FlexSensor(){
    Mid = 0;
    Idx = 0;
    Thumb = 0;
    MidMin = 9999;
    MidMax = 0;
    IdxMin = 9999;
    IdxMax = 0;
    ThumbMin = 9999;
    ThumbMax = 0;
}

void FlexSensor::getValue(){
    Mid = analogRead(flex_pin1);
    Idx = analogRead(flex_pin2);
    Thumb = analogRead(flex_pin3);
    
    #ifdef PRINT_VALUE
    
    Serial.print("Mid: ");
    Serial.println(Mid);
    Serial.print("Idx: ");
    Serial.println(Idx);
    Serial.print("Thumb: ");
    Serial.println(Thumb);
    
    #endif

    delay(5);
}

int FlexSensor::changeTrigger(){
    return (Mid < DEADLINE_LONG) || (Idx < DEADLINE_LONG);
}

int FlexSensor::cmpInit(int flag){
    switch(flag){
        case 1:
            return Mid > INIT_LONG;
            break;
        case 2:
            return Idx > INIT_LONG;
            break;
        case 3:
            return Thumb > INIT_SHORT;
            break;
    }
}

int FlexSensor::isInitTrigger() {
	return (Mid > INIT_LONG) && (Idx > INIT_LONG);
}

int FlexSensor::ThumbClicked(){
    return Thumb < DEADLINE_SHORT ? 1 : 0;
}

int FlexSensor::MidClicked(){
    return Mid < DEADLINE_SHORT ? 1 : 0;
}

int FlexSensor::IdxClicked(){
    return Idx < DEADLINE_LONG ? 1 : 0;
}

void FlexSensor::initMinMax(){
    int time = millis();
    while(millis()- time < 5000){
        
        getValue();
        
        MidMin = MidMin < Mid ? MidMin : Mid;
        MidMax = MidMax > Mid ? MidMax : Mid;
        
        IdxMin = IdxMin < Idx ? IdxMin : Mid;
        IdxMax = IdxMax > Idx ? IdxMax : Mid;

        ThumbMin = ThumbMin < Thumb ? ThumbMin : Mid;
        ThumbMax = ThumbMax > Thumb ? ThumbMax : Mid;
    }
}

void FlexSensor::printReg(){
    #ifdef PRINT_MIN_MAX
    Serial.print("Mid Max: ");
    Serial.println(MidMax);
    Serial.print("Mid Min: ");
    Serial.println(MidMin);
    
    Serial.print("Idx Max: ");
    Serial.println(IdxMax);
    Serial.print("Mid Min: ");
    Serial.println(IdxMin);

    Serial.print("Thumb Max: ");
    Serial.println(ThumbMax);
    Serial.print("Thumb Min: ");
    Serial.println(ThumbMin);
    
    #else

    Serial.print("Mid: ");
    Serial.println(Mid);
    Serial.print("Idx: ");
    Serial.println(Idx);
    Serial.print("Thumb: ");
    Serial.println(Thumb);
    
    #endif
}