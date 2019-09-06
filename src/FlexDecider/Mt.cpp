#include "Mt.h"

Mt::Mt(){
    angle_x = 0;
    angle_y = 0;
    delta_x = 0;
    delta_y = 0;
}

void Mt::getValue(float filtered_angle_x, float filtered_angle_y){
    angle_x = filtered_angle_x;
    angle_y = filtered_angle_y;
}

void Mt::calcDelta(){
    float tmp_x;
    float tmp_y;

    tmp_x = (float)tan(angle_x) * (-1) * TirWieght;
    tmp_y = (float)tan(angle_y) * TirWieght;

    if(tmp_x < 5 && tmp_x > -5 && tmp_y < 5 && tmp_y > -5){
        tmp_x = 0;
        tmp_y = 0;
    }

    delta_x = tmp_x * DisWieght;
    delta_y = tmp_y * DisWieght;
}