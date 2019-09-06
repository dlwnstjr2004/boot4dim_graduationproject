#include "FlexDecider.h"
#include "Arduino.h"

FlexDecider::FlexDecider(BLERemoteCharacteristic* address){
    pRemoteAddress = address;
    MouseAxis = new Mt;
    flex = new FlexSensor;
    ClickState = "";
    MtState = "";
    RCFlag = 0;
    LCFlag = 0;
    SCFlag = 0;
}

void FlexDecider::SendData(String sendData){
    pRemoteAddress->writeValue(sendData.c_str(), sendData.length());
}

void FlexDecider::SetClickState(){
    ClickState = "";
    if(RCFlag | LCFlag != 0){
        if(RCFlag == 1 && flex->cmpInit(1)){
            ClickState += "RC.RL.";
            RCFlag = 0;
        }
        else if(RCFlag == 1 && flex->cmpInit(2)){
            ClickState += "LC.RL.";
            LCFlag = 0;
        }
    }

    if(flex->IdxClicked() || flex->MidClicked() || flex->ThumbClicked()){
            if(flex->ThumbClicked()){// RC & LC
                if(flex->cmpInit(1)){
                    ClickState += "LC.PR.";
                    LCFlag = 1;
                }
                else if(flex->cmpInit(2)){
                    ClickState += "RC.PR.";
                    RCFlag = 1;
                }
            }

            else{
                if(flex->cmpInit(1)){
                    ClickState += "SU";
                }
                else if(flex->cmpInit(2)){
                    ClickState += "SD";
                }
            }
        }

}

void FlexDecider::SetMtState(float filtered_angle_x, float filtered_angle_y){
    MouseAxis->getValue(filtered_angle_x, filtered_angle_y);
    MouseAxis->calcDelta();
    MtState = "Mt." + (String)MouseAxis->delta_x + "/" + (String)MouseAxis->delta_y + "\n";

}

void FlexDecider::StateLogic(float filtered_angle_x, float filtered_angle_y){
    SetClickState();
    SetMtState(filtered_angle_x, filtered_angle_y);
    SendData(ClickState + MtState);
}