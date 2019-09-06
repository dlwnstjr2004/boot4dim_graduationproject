#ifndef _MT_
#define _MT_
#include "Arduino.h"
#include "math.h"

#define TirWieght 20
#define Error 5
#define DisWieght 3

class Mt{

    public:
        float angle_x;
        float angle_y;
        float delta_x;
        float delta_y;

        Mt();
        void getValue(float filtered_angle_x, float filtered_angle_y);  
        void calcDelta();
};

#endif