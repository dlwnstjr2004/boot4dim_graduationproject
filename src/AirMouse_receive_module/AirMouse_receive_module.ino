#include <SoftwareSerial.h>
#include "Mouse.h"
#include "Keyboard.h"

SoftwareSerial mySerial(10, 11); // RX, TX

int Keyboard_Mouse_mode = 1;
//0: Off
//1: Mouse
//2: Keyboard
void Logic_start(String read_data_frist){
  String inString = read_data_frist;
  String inString_back = inString;
  String inString_front;
  int index_x ;  
  int index_y ; 
  do{
    index_x = inString.indexOf('.');     
    index_y = inString.length(); 
    inString_front = inString.substring(0, index_x);  
    inString_back = inString.substring(index_x+1,index_y); 
    inString = inString_back;
    if(inString_front == "TF"){
      switch(Keyboard_Mouse_mode){
        case 0:Keyboard_Mouse_mode=1;break;
        case 1:Keyboard_Mouse_mode=2;break;
        case 2:Keyboard_Mouse_mode=0;break;
        default :Keyboard_Mouse_mode = 0; break;
      }
//      Serial.println("TF");
    }
    else if(Keyboard_Mouse_mode == 1){
      if(inString_front == "RC" ){
        Mouse.press(MOUSE_RIGHT);
        delay(1);
        Mouse.release(MOUSE_RIGHT);
//        Serial.println("RC");
      }
      else if(inString_front == "LC"){
        Mouse.press(MOUSE_LEFT);
        delay(1);
        Mouse.release(MOUSE_LEFT);
//        Serial.println("LC");
      }
      else if(inString_front == "SU"){
        Mouse.move(0,0,1);
//        Serial.println("SU");
      }
      else if(inString_front == "SD"){
        Mouse.move(0,0,-1);
//        Serial.println("SD");
      }
      else if(inString_front == "MT"){
        int number_x = inString.indexOf('/');
        int number_y = inString.indexOf('.');
        int move_x = inString.substring(0, number_x).toInt();
        int move_y = inString.substring(number_x+1, number_y).toInt();
        Mouse.move(move_x,move_y);
        inString_back = inString.substring(number_y+1,inString.indexOf('\n')); 
        inString = inString_back;
//        Serial.println("MT");
      }
    }
    else if(Keyboard_Mouse_mode == 2){
      if(inString_front == "SB" ){
        Keyboard.write(' ');
//        Serial.println("SB");
      }
      else if(inString_front == "ZI"){
        Keyboard.press(KEY_LEFT_CTRL);
        delay(1);
        Mouse.move(0,0,5);
        delay(1);
        Keyboard.releaseAll();
//        Serial.println("ZI");
      }
      else if(inString_front == "ZO"){
        Keyboard.press(KEY_LEFT_CTRL);
        delay(1);
        Mouse.move(0,0,-5);
        delay(1);
        Keyboard.releaseAll();
//        Serial.println("ZO");
      }
      else if(inString_front == "AU"){
        Keyboard.press(KEY_UP_ARROW);
        delay(1);
        Keyboard.releaseAll();
//        Serial.println("AU");
      }
      else if(inString_front == "AD"){
        Keyboard.press(KEY_DOWN_ARROW);
        delay(1);
        Keyboard.releaseAll();
//        Serial.println("AD");
      }
      else if(inString_front == "AL"){
        Keyboard.press(KEY_LEFT_ARROW);
        delay(1);
        Keyboard.releaseAll();
//        Serial.println("AL");
      }
      else if(inString_front == "AR"){
        Keyboard.press(KEY_RIGHT_ARROW);
        delay(1);
        Keyboard.releaseAll();
//        Serial.println("AR");
      }
    }
  }while(inString.length() >= 3);
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Mouse.begin();
  Keyboard.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Start!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(115200);
}

void loop() { // run over and over
  if(mySerial.available()){
    String read_data = mySerial.readStringUntil('\n');
    Serial.println("read_data : " + read_data);
    Logic_start(read_data);

  }
}
