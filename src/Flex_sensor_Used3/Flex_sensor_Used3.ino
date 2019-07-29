//flex 3개

#define ANALOG_PIN_0 36
#define flex_pin1 34
#define flex_pin2 33
#define flex_pin3 32

int analog_value1 = 0;
int analog_value2 = 0;
int analog_value3 = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Analog IN Test");
}



void loop() {
  analog_value1 = analogRead(flex_pin1);
  analog_value2 = analogRead(flex_pin2);
  analog_value3 = analogRead(flex_pin3);
  Serial.print("value1 = ");Serial.print(analog_value1);
  Serial.print(" | value2 = ");Serial.print(analog_value2);
  Serial.print(" | value3 = ");Serial.println(analog_value3);
  
  delay(500);

}
