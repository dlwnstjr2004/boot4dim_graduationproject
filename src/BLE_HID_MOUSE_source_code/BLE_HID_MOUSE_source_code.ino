//BLE MOUSE (CONNECT)
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

BLEHIDDevice* hid;
BLECharacteristic* inputMouse;


bool connected = false;

class MyCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer){
    connected = true;
    BLE2902* desc = (BLE2902*)inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(true);
  }

  void onDisconnect(BLEServer* pServer){
    connected = false;
    BLE2902* desc = (BLE2902*)inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(false);
  }
};



void taskServer(void*){


    BLEDevice::init("Flip-O-Matic");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyCallbacks());

    hid = new BLEHIDDevice(pServer);
    inputMouse = hid->inputReport(0); // <-- input REPORTID from report map

    std::string name = "chegewara";
    hid->manufacturer()->setValue(name);

    hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
    hid->hidInfo(0x00,0x02);

  BLESecurity *pSecurity = new BLESecurity();

  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

// http://www.keil.com/forum/15671/usb-mouse-with-scroll-wheel/
// Wheel Mouse - simplified version - 5 button, vertical and horizontal wheel
//
// Input report - 5 bytes
//
//     Byte | D7      D6      D5      D4      D3      D2      D1      D0
//    ------+---------------------------------------------------------------------
//      0   |  0       0       0    Forward  Back    Middle  Right   Left (Buttons)
//      1   |                             X
//      2   |                             Y
//      3   |                       Vertical Wheel
//      4   |                    Horizontal (Tilt) Wheel
//
// Feature report - 1 byte
//
//     Byte | D7      D6      D5      D4   |  D3      D2  |   D1      D0
//    ------+------------------------------+--------------+----------------
//      0   |  0       0       0       0   |  Horizontal  |    Vertical
//                                             (Resolution multiplier)
//
// Reference
//    Wheel.docx in "Enhanced Wheel Support in Windows Vista" on MS WHDC
//    http://www.microsoft.com/whdc/device/input/wheel.mspx
//


const uint8_t reportMapMouse[] = {

0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
     0x09, 0x02,        // USAGE (Mouse)
     0xa1, 0x01,        // COLLECTION (Application)
     0x09, 0x02,        //   USAGE (Mouse)
     0xa1, 0x02,        //   COLLECTION (Logical)
     0x09, 0x01,        //     USAGE (Pointer)
     0xa1, 0x00,        //     COLLECTION (Physical)
                        // ------------------------------  Buttons
     0x05, 0x09,        //       USAGE_PAGE (Button)      
     0x19, 0x01,        //       USAGE_MINIMUM (Button 1)
     0x29, 0x05,        //       USAGE_MAXIMUM (Button 5)
     0x15, 0x00,        //       LOGICAL_MINIMUM (0)
     0x25, 0x01,        //       LOGICAL_MAXIMUM (1)
     0x75, 0x01,        //       REPORT_SIZE (1)
     0x95, 0x05,        //       REPORT_COUNT (5)
     0x81, 0x02,        //       INPUT (Data,Var,Abs)
                        // ------------------------------  Padding
     0x75, 0x03,        //       REPORT_SIZE (3)
     0x95, 0x01,        //       REPORT_COUNT (1)
     0x81, 0x03,        //       INPUT (Cnst,Var,Abs)
                        // ------------------------------  X,Y position
     0x05, 0x01,        //       USAGE_PAGE (Generic Desktop)
     0x09, 0x30,        //       USAGE (X)
     0x09, 0x31,        //       USAGE (Y)
     0x15, 0x81,        //       LOGICAL_MINIMUM (-127)
     0x25, 0x7f,        //       LOGICAL_MAXIMUM (127)
     0x75, 0x08,        //       REPORT_SIZE (8)
     0x95, 0x02,        //       REPORT_COUNT (2)
     0x81, 0x06,        //       INPUT (Data,Var,Rel)
     0xa1, 0x02,        //       COLLECTION (Logical)
                        // ------------------------------  Vertical wheel res multiplier
     0x09, 0x48,        //         USAGE (Resolution Multiplier)
     0x15, 0x00,        //         LOGICAL_MINIMUM (0)
     0x25, 0x01,        //         LOGICAL_MAXIMUM (1)
     0x35, 0x01,        //         PHYSICAL_MINIMUM (1)
     0x45, 0x04,        //         PHYSICAL_MAXIMUM (4)
     0x75, 0x02,        //         REPORT_SIZE (2)
     0x95, 0x01,        //         REPORT_COUNT (1)
     0xa4,              //         PUSH
     0xb1, 0x02,        //         FEATURE (Data,Var,Abs)
                        // ------------------------------  Vertical wheel
     0x09, 0x38,        //         USAGE (Wheel)
     0x15, 0x81,        //         LOGICAL_MINIMUM (-127)
     0x25, 0x7f,        //         LOGICAL_MAXIMUM (127)
     0x35, 0x00,        //         PHYSICAL_MINIMUM (0)        - reset physical
     0x45, 0x00,        //         PHYSICAL_MAXIMUM (0)
     0x75, 0x08,        //         REPORT_SIZE (8)
     0x81, 0x06,        //         INPUT (Data,Var,Rel)
     0xc0,              //       END_COLLECTION
     0xa1, 0x02,        //       COLLECTION (Logical)
                        // ------------------------------  Horizontal wheel res multiplier
     0x09, 0x48,        //         USAGE (Resolution Multiplier)
     0xb4,              //         POP
     0xb1, 0x02,        //         FEATURE (Data,Var,Abs)
                        // ------------------------------  Padding for Feature report
     0x35, 0x00,        //         PHYSICAL_MINIMUM (0)        - reset physical
     0x45, 0x00,        //         PHYSICAL_MAXIMUM (0)
     0x75, 0x04,        //         REPORT_SIZE (4)
     0xb1, 0x03,        //         FEATURE (Cnst,Var,Abs)
                        // ------------------------------  Horizontal wheel
     0x05, 0x0c,        //         USAGE_PAGE (Consumer Devices)
     0x0a, 0x38, 0x02,  //         USAGE (AC Pan)
     0x15, 0x81,        //         LOGICAL_MINIMUM (-127)
     0x25, 0x7f,        //         LOGICAL_MAXIMUM (127)
     0x75, 0x08,        //         REPORT_SIZE (8)
     0x81, 0x06,        //         INPUT (Data,Var,Rel)
     0xc0,              //       END_COLLECTION
     0xc0,              //     END_COLLECTION
     0xc0,              //   END_COLLECTION
     0xc0               // END_COLLECTION
};

    hid->reportMap((uint8_t*)reportMapMouse, sizeof(reportMapMouse));
    hid->startServices();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(HID_MOUSE);
    pAdvertising->addServiceUUID(hid->hidService()->getUUID());
    pAdvertising->start();
    hid->setBatteryLevel(7);

    ESP_LOGD(LOG_TAG, "Advertising started!");
    delay(portMAX_DELAY);
  
};


const int button1Pin = 12;     // the number of the pushbutton pin
const int button2Pin = 13;     // the number of the pushbutton pin
const int ledPin =     5;      // the number of the LED pin

void setup() {
  
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  xTaskCreate(taskServer, "server", 20000, NULL, 5, NULL);
}

void loop() {
  
  if(connected){

    //vTaskDelay(5000);Serial.println("dormindo");

     while (digitalRead(button2Pin) ==  LOW ){ 

          Serial.println("mouse Scroll DOWN");
          //<button>, <x>, <y>, <wheel>
         
          uint8_t msg[] = {  0x00, 0x00, 0x00, -0x01};
         
          inputMouse->setValue(msg,sizeof(msg));
          inputMouse->notify();
          delay(100);

          
          
     }
    
    while (digitalRead(button1Pin) ==  LOW ){ 

          Serial.println("mouse Scroll UP");
          //<button>, <x>, <y>, <wheel>
         
          uint8_t msg[] = {  0x00, 0x00, 0x00, 0x01};
         
          inputMouse->setValue(msg,sizeof(msg));
          inputMouse->notify();
          delay(100);

     }
    
    
  }
  delay(50);
}
