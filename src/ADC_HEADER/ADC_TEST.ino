#include <Flex.h>

//#define ANALOG_PIN_0 39 // VN -> ADC
//#define ANALOG_PIN_0 36 // VP -> ADC
#define ANALOG_PIN_0 33 // ADC
int LED_BUILTIN = 2;
int v1 = 1;
int v2 = 1;
int v3 = 1;
FlexSensor flex(33,34,35);

void setup() {

}

void loop() {
  // put your main code here, to run repeatedly:
  flex.setVadcResult();
  flex.returnValue(&v1, &v2, &v3);
  
  Serial.println(v1);
  Serial.println(v2);
  Serial.println(v3);
  delay(500);
}
