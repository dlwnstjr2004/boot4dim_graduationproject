#ifndef _FLEX_DECIDER_
#define _FLEX_DECIDER_

#include "Arduino.h"
#include "Flex.h"
#include "BLEDevice.h"
#include "Wire.h"
#include "Mt.h"

// .PR : press
// .RL : release
// Click State : RC, LC, SU, SD
// Mt State : Mouse Tracking

class FlexDecider{
    private:
        BLERemoteCharacteristic* pRemoteAddress;
        FlexSensor *flex;
        Mt *MouseAxis;
        String ClickState;
        String MtState;
        int RCFlag;
        int LCFlag;
        int SCFlag;
    public:
        FlexDecider(BLERemoteCharacteristic* address);
        void StateLogic(float filtered_angle_x, float filtered_angle_y);
        void SendData(String SendValue);
        void SetClickState();
        void SetMtState(float filtered_angle_x, float filtered_angle_y);
};

#endif