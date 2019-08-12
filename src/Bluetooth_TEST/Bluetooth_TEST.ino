//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

int LED_BUILTIN_BLUETOOTH = 2;

BluetoothSerial SerialBT;
char OnCondition = 's';
char OffCondition = 'a';
char input;
void setup() {
  pinMode(LED_BUILTIN_BLUETOOTH,OUTPUT);
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}


void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(input = SerialBT.read());
    if (input == 's')
        digitalWrite(LED_BUILTIN_BLUETOOTH, HIGH);  
    else if(input == 'a')
        digitalWrite(LED_BUILTIN_BLUETOOTH, LOW);
  }
  delay(20);
}
