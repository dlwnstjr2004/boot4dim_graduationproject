/*핀 연결
 *flex_sensor : 중지-34(빨) / 검지-33(파) / 엄지-32(주)
 *mpu 6050 : SDA-21(파) / SCL-22(초)
 *button : 2
 */

 /*작동 방식
  * flex_sensor 관련
  * (1)엄지 접힌 상태에서 검지 - left click
  * (2)엄지 접힌 상태에서 중지 _ right click
  * (3)엄지 접힌 상태에서 검지, 중지 - scroll
  * 
  */

#include<Wire.h>
#include<Math.h>
#include "BLEDevice.h"

//Pin number
#define Muscle_pin 39 // 근육센서
#define flex_pin1 34  // 중지
#define flex_pin2 33  // 검지
#define flex_pin3 32  // 엄지

//sensor max number
#define Muscle_limit 60
#define flex_value1 2000
#define flex_value2 2000
#define flex_value3 1700

//timer terminal
#define change_dir_terminal 50
#define muscle_terminal 2000

//flag signal
bool muscle_flag;

// signal (muscle)
int muscle_timer_now;
int muscle_timer_pre;

// signal (gyro)
#define MPU_addr 0x68 // mpu address
float dt;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
float accel_angle_x, accel_angle_y, accel_angle_z;
float gyro_angle_x, gyro_angle_y, gyro_angle_z;
float filtered_angle_x, filtered_angle_y, filtered_angle_z;

  //variable fix value
float baseAcX, baseAcY, baseAcZ;
float baseGyX, baseGyY, baseGyZ;

  //time variable
unsigned long t_now;
unsigned long t_prev;

float gyro_x, gyro_y, gyro_z;

  //variable for hand action
  //angle velocity
float VX=0;
float VY=0;
float VZ=0;

  //variable for snap (swipe)
bool up_sign, down_sign, left_sign, right_sign;
bool up_sign_past, down_sign_past, left_sign_past, right_sign_past;
int snap_timer;
int snap_terminal;

  //variable for reset
int reset_gyro_timer = 0;

  //variable for flexsensor assest (scroll)
bool change_dir;
int change_dir_timer = 0;

int sign_flag_timer = 0;
int down_sign_flag_timer = 0;




// NowState
int NowState = 0;
/*
 * 0 : 정지
 * 1 : 마우스
 * 2 : 키보드
*/

//ble client setting
// The remote service we wish to connect to.
static BLEUUID serviceUUID("0000ffe0-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("0000ffe1-0000-1000-8000-00805f9b34fb");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
      // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

void InitBLE();

//state 구현부================================
void State_Change();

//mpu 6050 함수 정의============================
void initMPU6050();

void calibAccelGyro();

void initDT();

void readAccelGyro();

void calcDT();

void HandAction();

void InitHandAction();

void UpdateHandAction();

void calcAccelYPR();

void calcGyroYPR();

void calcFilteredYPR();

void Restandard(); // to make a new standard for Gyro

// timer
bool timer(int StandardTime, int Terminal_time );


//flex sensor 작동==============================
void SendFlexData();

//Send Data
void Send_Data();

//setup=======================================
void setup() {
  Serial.begin(9600);
  Serial.println("ESP32 Analog IN Test");
  //pinMode(2,INPUT_PULLUP);
  // put your setup code here, to run once:
  initMPU6050();
  calibAccelGyro();
  initDT();
  InitHandAction();
  InitBLE();
}

void loop() {
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }
  
  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    //mpu 계산
    readAccelGyro();
    calcDT();

    calcAccelYPR();
    calcGyroYPR();
    calcFilteredYPR();

    //send data flag설정
    State_Change();
//    SendFlexData();
//    HandAction();

    //send data
//    Send_Data();
//    
//    Restandard();
  
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }
  delay(10);
}



/////함수부분
//****************** 초기화
void initMPU6050(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void initDT(){
  t_prev = micros();
}

void InitHandAction(){ // when it start
  up_sign=false;
  down_sign=false;
  left_sign=false;
  right_sign=false;
  up_sign_past=false;
  down_sign_past=false;
  left_sign_past=false;
  right_sign_past=false;
  change_dir = false;
  muscle_flag = false;
  sign_flag_timer = millis();
}

void UpdateHandAction(){ // when Restandard
  up_sign=false;
  down_sign=false;
  left_sign=false;
  right_sign=false;
  up_sign_past=false;
  down_sign_past=false;
  left_sign_past=false;
  right_sign_past=false;
  VX=0;
  VY=0;
  VZ=0;
  reset_gyro_timer = 0;
  change_dir_timer = millis();
}

void InitBLE(){
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void calibAccelGyro(){
  float sumAcX = 0;
  float sumAcY = 0;
  float sumAcZ = 0;
  float sumGyX = 0;
  float sumGyY = 0;
  float sumGyZ = 0;

  readAccelGyro();

  for(int i = 0; i<10; i++){
    readAccelGyro();

    sumAcX += AcX, sumAcY += AcY, sumAcZ += AcZ;
    sumGyX += GyX, sumGyY += GyY, sumGyZ += GyZ;

    delay(100);
  }

  baseAcX = sumAcX / 10;
  baseAcY = sumAcY / 10;
  baseAcZ = sumAcZ / 10;
  
  baseGyX = sumGyX / 10;
  baseGyY = sumGyY / 10;
  baseGyZ = sumGyZ / 10;
}

//****************** 계산
void readAccelGyro(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);

  AcX = Wire.read() << 8|Wire.read();
  AcY = Wire.read() << 8|Wire.read();
  AcZ = Wire.read() << 8|Wire.read();
  Tmp = Wire.read() << 8|Wire.read();
  GyX = Wire.read() << 8|Wire.read();
  GyY = Wire.read() << 8|Wire.read();
  GyZ = Wire.read() << 8|Wire.read();
}

void calcDT(){
  t_now = micros();
  dt = (t_now - t_prev) / 1000000.0;
  t_prev = t_now;
}

void calcAccelYPR(){
  float accel_x, accel_y, accel_z;
  float accel_xz, accel_yz, accel_xy;
  const float RADIANS_TO_DEGREES = 180/3.14159;

  accel_x = AcX - baseAcX;
  accel_y = AcY - baseAcY;
  accel_z = AcZ + (16384 - baseAcZ);

  accel_yz = sqrt(pow(accel_y , 2) + pow(accel_z,2));
  accel_angle_y = atan(-accel_x / accel_yz) * RADIANS_TO_DEGREES;

  accel_xz = sqrt(pow(accel_x , 2) + pow(accel_z,2));
  accel_angle_x = atan(accel_y / accel_xz) * RADIANS_TO_DEGREES;

/*
  accel_xy = sqrt(pow(accel_x , 2) + pow(accel_y,2));
  accel_angle_z = atan(accel_xy / accel_z) * RADIANS_TO_DEGREES;
// advisable code. but it cause infinity. We can`t use this method.
*/
  accel_angle_z = 0;
}

void calcGyroYPR(){
  const float GYROXYZ_TO_DEGREES_PER_SEC = 131;

  gyro_x = (GyX - baseGyX)/GYROXYZ_TO_DEGREES_PER_SEC;
  gyro_y = (GyY - baseGyY)/GYROXYZ_TO_DEGREES_PER_SEC;
  gyro_z = (GyZ - baseGyZ)/GYROXYZ_TO_DEGREES_PER_SEC;

  gyro_angle_x += gyro_x *dt;
  gyro_angle_y += gyro_y *dt;
  gyro_angle_z += gyro_z *dt;
}

void calcFilteredYPR(){
  const float ALPHA = 0.96;
  float tmp_angle_x, tmp_angle_y, tmp_angle_z;

  tmp_angle_x = filtered_angle_x + gyro_x * dt;
  tmp_angle_y = filtered_angle_y + gyro_y * dt;
  tmp_angle_z = filtered_angle_z + gyro_z * dt;

  filtered_angle_x = ALPHA * tmp_angle_x + (1.0-ALPHA) * accel_angle_x;
  filtered_angle_y = ALPHA * tmp_angle_y + (1.0-ALPHA) * accel_angle_y;
  // filtered_angle_z = ALPHA * tmp_angle_z + (1.0-ALPHA) * accel_angle_z; // advisable code but it dosen`t work
  filtered_angle_z = tmp_angle_z; // z is floating in here

  if( (filtered_angle_x > 40) || (filtered_angle_x < -40 ) ) // init floating z
  {
    filtered_angle_z = 0;
  }
}

//****************** 실행부
void State_Change(){
  int analog_muscle;
  analog_muscle = analogRead(Muscle_pin);
  muscle_timer_now = millis();
  if (analog_muscle < Muscle_limit)
  {
    muscle_timer_pre = muscle_timer_now;
    muscle_flag = false;
  }
  else if( (muscle_timer_now - muscle_timer_pre) == muscle_terminal)
  {
    muscle_flag = true;
    if (NowState == 2)
    {
      NowState == 0;
    }
    else NowState++;
  }
}

void Send_Data(){
  String newValue = "SB.\n";
  pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
}

void SendFlexData(){
  int flex_analog1;
  int flex_analog2;
  int flex_analog3;
  
  flex_analog1 = analogRead(flex_pin1);
  flex_analog2 = analogRead(flex_pin2);
  flex_analog3 = analogRead(flex_pin3);
  if(NowState == 1)
  {
    if(flex_analog3<flex_value3)
    { // 엄지 press
      
    }
  }
  //============== keyboard mode (without analog_value3: 엄지) ==============
  else if(NowState == 2)
  {
    if((flex_analog1<flex_value1)&&(flex_analog2<flex_value2))
    { // 다른건 1000이었는데 얘만 1650이상이었음. 이거 뭘로해야함?
      if( filtered_angle_x > 45 )
      {
        String newValue = "ZI.\n";
        pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
      }
      else if( filtered_angle_x < -45 )
      {
        String newValue = "ZO.\n";
        pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
      }
    }
    else if(flex_analog1<flex_value1)
    { // 다른건 1000이었는데 얘만 1550이상이었음. 이거 뭘로해야함?
      String newValue = "SB.\n";
      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
    }
  }
  
}

void Restandard() // to make a new standard for Gyro
{ 
  if( reset_gyro_timer > 5000 ) // when User don`t move more than 5 second, Restandard start.
  { // check no movement
    if(filtered_angle_y > 50 || filtered_angle_y < -50 || filtered_angle_x > 50 || filtered_angle_x < -50 )
      {
        Serial.println("Alart!");
      } // if current position is not good for Gyro sensor, give "Alart!" feedback so that User can re-initiating.
     calibAccelGyro();
     UpdateHandAction();
     initDT();
     Serial.println("reset clear");
    }
  //else { Serial.println("User is moving now"); }
}

bool timer(int StandardTime, int Terminal_time ){
  int now_time;
  now_time=millis();
  if( (now_time-StandardTime) > Terminal_time ){ // if it reached Terminal time
    return true;
  }
  else{
    return false;
  }
}

void HandAction(){ // right hand

// ================ tilted angle need at least 20 degree. From here, the code will check which hand action you do. ================
// VX, VY save mouse point moving (delta)
// two types of snap flag ( ex : up_snap, up_snap_past ) can figure out your current hand snap action to keyboard::Arrows

//check the two flags to figure out current action.
  if(NowState == 1)
  {
    float radian = 0.0174533;
    int dis_x = tan(filtered_angle_x*radian) * -20;
    int dis_y = tan(filtered_angle_y*radian) * 20;
    if(dis_x < 5 && dis_x > -5 && dis_y < 5 && dis_y > -5){
      dis_x= 0;
      dis_y= 0;
    }
    dis_x = dis_x*3;
    dis_y = dis_y*3;
    String newValue2 = "MT."+ (String)dis_x + "/" + (String)dis_y +".\n";  // sending mouse pointer!
    pRemoteCharacteristic->writeValue(newValue2.c_str(), newValue2.length());
//      Serial.println("MT."+(String)dis_x+"/"+(String)dis_y+".\n");
  }
  else if(NowState == 2)
  {
    if(filtered_angle_y > 40 && abs(filtered_angle_x) < 20) // when tilted down 
    {
      if(timer(sign_flag_timer,500)){
        down_sign = true;  
        sign_flag_timer = millis();
      }
    }
    
    // when tilted up
    else if(filtered_angle_y < -25 && abs(filtered_angle_x) < 20) 
    {
      if(timer(sign_flag_timer,500)){
        up_sign = true;
        sign_flag_timer = millis();
      }
    }
    // when tilted side. codes in below only check hand swing
    else if ( (filtered_angle_x > 35) && abs(filtered_angle_y) < 20 )
    {
      if(timer(sign_flag_timer,500)){
        left_sign = true;
        sign_flag_timer = millis();
      }
    }
    else if (filtered_angle_x < -30 && abs(filtered_angle_y) < 20) // (up_sign == true)
    {
      if(timer(sign_flag_timer,500)){
        right_sign = true;
        sign_flag_timer = millis();
      }
    }
    
    else
    {
      right_sign = false;
      left_sign = false;
      up_sign = false;
      down_sign = false;
    }

    if(timer(snap_timer, snap_terminal)) // If timer is bigger than terminal => new order available
    {
      if( (up_sign == true) && (up_sign_past == false) )
      {
        Serial.println("Arrow Up");
        String newValue = "AU.\n";
        pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());

      }
      else if( (down_sign == true) && (down_sign_past == false) )
      {
        Serial.println("Arrow Down");
        String newValue = "AD.\n";
        pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());

      }
      else if( (left_sign == true) && (left_sign_past == false) )
      {
        Serial.println("Arrow Left");
        String newValue = "AL.\n";
        pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());

      }
      else if( (right_sign == true) && ( right_sign_past == false) )
      {
        Serial.println("Arrow Right");
        String newValue = "AR.\n";
        pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());

      }
    }
    else { reset_gyro_timer = millis() - snap_timer; } // 마지막으로 들어온 신호와의 차이.
  }
  
//1. flag update set;
  up_sign_past=up_sign;
  down_sign_past=down_sign;
  left_sign_past=left_sign;
  right_sign_past=right_sign; 
  
//3. Gyro can give Assestant about Scroll Direction.
  if( timer(change_dir_timer, change_dir_terminal) == true ) // If timer is bigger than terminal => new order available
  {
    if(filtered_angle_x > 60) // if User snap once, Scroll Direction is upside down.
    {
      if (change_dir == true)
      {
        change_dir = false;
      }
      else
      {
        change_dir = true;
      }
      change_dir_timer = millis();
    }
  }
}
