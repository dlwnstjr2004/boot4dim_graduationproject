#include <Flex.h>
#include <FlexDecider.h>
#include <Mt.h>

FlexDecider *pRemote;

#define pAddress 0x00
#define filtered_angle_x 10
#define filtered_angle_y 10

void setup() {
  // put your setup code here, to run once:
  pRemote = new FlexDecider(pAddress);
}

void loop() {
  String NewValue = "ABC";
  // put your main code here, to run repeatedly:
  //pRemote-> SendDate(NewValue);
  // Send new data
  //pRemote-> SetClickState();
  // Set ClickState(RC,LC, SC)
  //pRemote-> SetMtState(filtered_angle_x, filtered_angle_y);
  // Set Mouse Tracking State
  pRemote->StateLogic(filtered_angle_x, filtered_angle_y);
  // 1. Set Clock State
  // 2. Set Mt State
  // 3. Combine
  // 4. Send Data
}
