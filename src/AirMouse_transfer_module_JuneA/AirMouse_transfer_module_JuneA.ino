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

#include <Wire.h>
#include <Math.h>
#include <flex.h>
#include "BLEDevice.h"

//Pin number
flex flex_sensor;
#define Muscle_pin 39 // 근육센서

//sensor max number
#define Muscle_limit 60

//timer terminal
#define change_dir_terminal 50
#define muscle_terminal 2000

//flag signal
bool muscle_flag;
bool left_click_flag;
bool right_click_flag;
bool click_flag;
bool zoom_in_flag;
bool zoom_out_flag;
bool spacebar_flag;

// tiemr (muscle)
int muscle_timer_pre = 0;
int click_timer_pre = 0;

// signal (gyro)
#define MPU_addr 0x68 // mpu address
float dt;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int16_t gyroY,gyroZ;
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

int Sensitivity = 600;
int delayi = 3;

uint8_t i2cData[14];
const uint8_t IMUAddress = 0x68; // AD0 is logic low on the PCB
const uint16_t I2C_TIMEOUT = 1000; // Used to check for errors in I2C communication

// NowState
int NowState = 1;
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
void When_Tilted_Down();
void When_Tilted_Up();
void When_Tilted_Side();
void Mouse_move_from_z();
void Compensation_VX_From_Y();
void Sending_Arrow_Data();
uint8_t i2cRead(uint8_t registerAddress, uint8_t* data, uint8_t nbytes);

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
    SendFlexData();
    HandAction();

    //send data
    Send_Data();
//    
    Restandard();
  
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }
  delay(delayi);
}



/////함수부분
//****************************************************************************************** 초기화
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

//****************************************************************************************** 계산
void readAccelGyro(){
  while(i2cRead(0x3B,i2cData,14));
  AcX = ((i2cData[0] << 8) | i2cData[1]);
  AcY = ((i2cData[2] << 8) | i2cData[3]);
  AcZ = ((i2cData[4] << 8) | i2cData[5]);
  GcX = ((i2cData[8] << 8) | i2cData[9]);
  GcY = ((i2cData[10] << 8) | i2cData[11]);
  GcZ = ((i2cData[12] << 8) | i2cData[13]);
 
  gyroY = GcY / Sensitivity / 1.1 ;
  gyroZ = GcZ / Sensitivity  * -1;
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

//****************************************************************************************** 실행부
void State_Change(){
  int analog_muscle;
  analog_muscle = analogRead(Muscle_pin);
  if (analog_muscle < Muscle_limit)
  {
    muscle_timer_pre = millis();
    muscle_flag = false;
  }
  else if(timer(muscle_timer_pre,muscle_terminal))
  {
    muscle_flag = true;
  }
}

void Send_Data(){
//  if(muscle_flag == true){
//    String newValue = "TF.\n";
//    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
//    if (NowState == 2)
//    {
//      NowState = 0;
//    }
//    else NowState++;
//  }
//  else if(NowState == 1){
    if(NowState ==1){
//    if(left_click_flag && right_click_flag){  //SCROLL 미구현
//      String newValue = "TF.\n";
//      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
//    }
//    else if(left_click_flag && !(right_click_flag) && timer(click_timer_pre,50)){
//      String newValue = "LC.PR\n";
//      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
//      click_flag = true;
//    }
//    else if(right_click_flag && !(left_click_flag) && timer(click_timer_pre,50)){
//      String newValue = "RC.PR\n";
//      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
//      click_flag = true;
//    }
//    else if(click_flag && !(left_click_flag) && !(right_click_flag)){
//      String newValue = "LC.RL.RC.RL\n";
//      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
//      click_flag = true;
//    }
//    else {
      String newValue = "MT."+ (String)gyroZ + "/" + (String)gyroY +".\n";
      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());   
//    }
  }
  else if(NowState == 2){
    if(zoom_in_flag){
      String newValue = "ZI.\n";
      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
    }
    else if(zoom_out_flag){
      String newValue = "ZO.\n";
      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
    }
    else if(spacebar_flag){
      String newValue = "SB.\n";
      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
    }
  }
}

void SendFlexData(){
  flex_sensor.setValue();
  
  if(NowState == 1)
  {
    if(flex_sensor.isThumbClicked())
    { // 엄지 press
      if(flex_sensor.isIdxClicked()){
        left_click_flag = true;
        if(!(timer(click_timer_pre,50))){
          click_timer_pre = millis();
        }
      }
      else if(flex_sensor.isIdxReleased()){
        left_click_flag = false;
      }
      if(flex_sensor.isMidClicked()){
        right_click_flag = true;
        if(!(timer(click_timer_pre,50))){
          click_timer_pre = millis();
        }
      }
      else if(flex_sensor.isMidReleased()){
        right_click_flag = false;
      }      
    }
  }
  //============== keyboard mode (without analog_value3: 엄지) ==============
  else if(NowState == 2)
  {
    if(flex_sensor.isThumbReleased() && flex_sensor.isMidClicked() && flex_sensor.isIdxClicked())
    { // 다른건 1000이었는데 얘만 1650이상이었음. 이거 뭘로해야함?
      if( filtered_angle_x > 45 )
      {
        zoom_in_flag = true;
        zoom_out_flag = false;
      }
      else if( filtered_angle_x < -45 )
      {
        zoom_out_flag = true;
        zoom_in_flag = false;
      }
    }
    else if(flex_sensor.isThumbReleased() && flex_sensor.isIdxReleased() && flex_sensor.isMidClicked())
    { // 다른건 1000이었는데 얘만 1550이상이었음. 이거 뭘로해야함?
      spacebar_flag = true;
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
  if( (millis()-StandardTime) > Terminal_time ){ // if it reached Terminal time
    return true;
  }
  else{
    return false;
  }
}

void HandAction(){ //all about Gyro handaction
//============= Data processing =============
  if(filtered_angle_y > 20) // when tilted down. codes in below check hand snap and mouse point both.
  {
    When_Tilted_Down();
  }
  else if(filtered_angle_y < -20) // when tilted up
  {
    When_Tilted_Up();
  }
  else
  {
    up_sign = false;
    down_sign = false;
    VY = 0;
  }

  Mouse_move_from_z(); // move to left or right : it`s not about tilt. just handle pointer
  Compensation_VX_From_Y(); //compensation. Y값에 따른 VX 보정부분

  if ( (filtered_angle_x > 37) ) // when tilted side. codes in below only check hand snap
  {
    When_Tilted_Side();
  }
  else
  {
    right_sign = false;
    left_sign = false;
  }

//============= Transmit output =============
  if ( NowState == 2 ) // output1 : Sending Arrow key [keyboard]
  {
    Sending_Arrow_Data();
  }
  else if( NowState == 1 ) // output2 : Sending Mouse point
  {
      
  } 

//============= Update data =============
// outro1 : flag update set;
  up_sign_past=up_sign;
  down_sign_past=down_sign;
  left_sign_past=left_sign;
  right_sign_past=right_sign;

// outro2 : update scroll direction
  if ( timer(change_dir_timer,change_dir_terminal) )
  {
    if(filtered_angle_x > 60)
    {
      change_dir = ~change_dir;
      change_dir_timer = millis();
    }
  }
}

void When_Tilted_Down() // handaction inner funtion 1
{
  down_sign = true;
  if( filtered_angle_x > 10 )// left down on computer monitor
  {
    VX = -sqrt( pow(filtered_angle_x , 2) + 4*pow( (90-abs(filtered_angle_z)) , 2) );//1
    VY = filtered_angle_y;
  }
  else if ( filtered_angle_x < -10 )//|| (filtered_angle_z < -20) ) // right down on computer monitor
  {
    VX = sqrt( pow(filtered_angle_x , 2) + 4*pow( (90-abs(filtered_angle_z)) , 2) );
    VY = filtered_angle_y;
  }
  else
  {
    VY = filtered_angle_y;
    VX = 0;
  }
}

void When_Tilted_Up() // handaction inner funtion 2
{
  up_sign = true;
  if( filtered_angle_x < -10 ) //|| (filtered_angle_z > 20) ) // left up on computer monitor
  {
    VX = -sqrt( pow(filtered_angle_x , 2) + 4*pow( (90-abs(filtered_angle_z)) , 2) );
    VY = filtered_angle_y;
  }
  else if ( filtered_angle_x > 10 ) //|| (filtered_angle_z < -20) ) // right up on computer monitor
  {
    VX = sqrt( pow(filtered_angle_x , 2) + 4*pow( (90-abs(filtered_angle_z)) , 2) );
    VY = filtered_angle_y;
  }
  else
  {
    VY = filtered_angle_y;
    VX = 0;
  }
}

void When_Tilted_Side() // handaction inner funtion 3
{
  if (filtered_angle_y > 20) // (down_sign == true)
  {
    left_sign = true;
    down_sign = false;
    right_sign = false;
  }
  else if (filtered_angle_y < -20) // (up_sign == true)
  {
    right_sign = true;
    up_sign = false;
    left_sign = false;
  }
  else
  {
    right_sign = false;
    left_sign = false;
  }
}

void Mouse_move_from_z() // handaction inner funtion 4
{
  if ( filtered_angle_z > 100 ) // move to left : it`s not about tilt. just handle pointer
  {
    VX = -sqrt( pow(filtered_angle_x , 2) + 4*pow( 90-abs(filtered_angle_z) , 2) );
    VY = filtered_angle_y;
  }
  else if ( (filtered_angle_z < 80) && (filtered_angle_z > 50) ) // move to right : it`s not about tilt. just handle pointer
  {
    VX = sqrt( pow(filtered_angle_x , 2) + 4*pow( 90-abs(filtered_angle_z) , 2) );
    VY = filtered_angle_y;
  }
  else { }//VX = 0; }
}

void Compensation_VX_From_Y() // handaction inner funtion 5
{
  if( (filtered_angle_y < 50) || (filtered_angle_y > -50) ) // compensation
  {
    if( abs(VX) < 80 ) VX = 0;
  }
}

void Sending_Arrow_Data() // handaction inner funtion 6
{
  if( timer(snap_timer, snap_terminal) == true ) // If timer is bigger than terminal => new order available
  {
    if( (up_sign == true) && (up_sign_past == false) )
    {
      Serial.println("Arrow Up");
      String newValue = "AU.\n";
      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
      snap_timer = millis();
    }
    else if( (down_sign == true) && (down_sign_past == false) )
    {
      Serial.println("Arrow Down");
      String newValue = "AD.\n";
      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
      snap_timer = millis();
    }
    else if( (left_sign == true) && (left_sign_past == false) )
    {
      Serial.println("Arrow Left");
      String newValue = "AL.\n";
      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
      snap_timer = millis();
    }
    else if( (right_sign == true) && ( right_sign_past == false) )
    {
      Serial.println("Arrow Right");
      String newValue = "AR.\n";
      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
      snap_timer = millis();
    }
  }
  else { reset_gyro_timer = millis() - snap_timer; } // 마지막으로 들어온 신호와의 차이.
}

uint8_t i2cRead(uint8_t registerAddress, uint8_t* data, uint8_t nbytes) {
  uint32_t timeOutTimer;
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  if(Wire.endTransmission(false)) // Don't release the bus
    return 1; // Error in communication
  Wire.requestFrom(IMUAddress, nbytes,(uint8_t)true); // Send a repeated start and then release the bus after reading
  for(uint8_t i = 0; i < nbytes; i++) {
    if(Wire.available())
      data[i] = Wire.read();
    else {
      timeOutTimer = micros();
      while(((micros() - timeOutTimer) < I2C_TIMEOUT) && !Wire.available());
      if(Wire.available())
        data[i] = Wire.read();
      else
        return 2; // Error in communication
    }
  }
  return 0; // Success
}
