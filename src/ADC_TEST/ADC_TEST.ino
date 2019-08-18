//#define ANALOG_PIN_0 39 // VN -> ADC
//#define ANALOG_PIN_0 36 // VP -> ADC
#define ANALOG_PIN_0 33 // ADC
int LED_BUILTin = 2;
int analog_value = 1;
/* Arduino ADC PIN TEST
 * used esp32 dev module
 * VN,VP PIN can use ADC PIN
 */
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTin, OUTPUT);
  Serial.begin(115200);
  Serial.println("ESP32 Analog IN Test");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTin,HIGH);
  analog_value = analogRead(ANALOG_PIN_0);
  Serial.println(analog_value);
  delay(500);
}
