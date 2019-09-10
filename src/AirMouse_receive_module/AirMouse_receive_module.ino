#include <SoftwareSerial.h>
#include "Mouse.h"
#include "Keyboard.h"

SoftwareSerial mySerial(10, 11); // RX, TX
int xp = 0, xm = 0, yp = 0, ym = 0;
int move_x;
int move_y;

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
    
    if(index_x != 2){Serial.println("break");break;}
    
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
        int number_click1 = inString.indexOf('.');
        String click_order = inString.substring(0, number_click1);
        String click_order_back = inString.substring(number_click1+1, inString.indexOf('\n'));
        inString = click_order_back;
        //Right Click Press
        if(click_order == "PR"){    
          Mouse.press(MOUSE_RIGHT);
        }
        //Right Click Release
        else if(click_order == "RL"){ 
          Mouse.release(MOUSE_RIGHT);
        }
//        Serial.println("RC");
      }
      else if(inString_front == "LC"){
        int number_click1 = inString.indexOf('.');
        String click_order = inString.substring(0, number_click1);
        String click_order_back = inString.substring(number_click1+1, inString.indexOf('\n'));
        inString = click_order_back;
        //Left Click Release
        if(click_order == "PR"){
          Mouse.press(MOUSE_LEFT);
        }
        //Left Click Release
        else if(click_order == "RL"){
          Mouse.release(MOUSE_LEFT);
        }
//        Serial.println("LC");
      }
      //Scroll Up
      else if(inString_front == "SU"){
        Mouse.move(0,0,1);
//        Serial.println("SU");
      }
      //Scroll Down
      else if(inString_front == "SD"){
        Mouse.move(0,0,-1);
//        Serial.println("SD");
      }
      //Mouse Tracking
      else if(inString_front == "MT"){
        int number_x = inString.indexOf('/');
        int number_y = inString.indexOf('.');
        move_x = inString.substring(0, number_x).toInt();
        move_y = inString.substring(number_x+1, number_y).toInt();
       // Mouse.move(move_x,move_y);
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
  Serial.begin(9600);
  Mouse.begin();
  Keyboard.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Start!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(115200);
}
bool start = false;
void loop() { // run over and over
  if(mySerial.available()){
    String read_data = mySerial.readStringUntil('\n');
    Serial.println("read_data : " + read_data);
    if(start == false){
      start = true;
    }
    else {
      Logic_start(read_data);
    }
  }
  else{
    xp = move_x >0 && move_x < 4 ? xp += move_x : xp;
    xm = move_x > -4 && move_x < 0 ? xm += move_x: xm;
    yp = move_y >0 && move_y < 4 ? yp += move_y : yp;
    ym = move_y > -4 && move_y < 0 ? ym += move_y: ym;

    if(xp > 2){
      xp-=2;
      Mouse.move(1,0);   
    }
    if(xm < -2){
      xm +=2;
      Mouse.move(-1,0); 
    }
    
    if(yp > 2){
      yp-=2;
      Mouse.move(0,1);   
    }
    if(ym < -2){
      ym +=2;
      Mouse.move(0,-1); 
    }

    for(int i =0; i<10;i++)
    {
        Mouse.move(move_x/4,move_y/4);
    }
    
  }
}
