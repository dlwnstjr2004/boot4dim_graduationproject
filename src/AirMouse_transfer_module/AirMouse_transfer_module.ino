#include<Wire.h>
#include<Math.h>

//mpu 6050 함수 정의============================
float dt;
const int MPU_addr = 0x68; // mpu address
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
float VX,VY,VZ; // angle velocity

//variable for snap (swipe)
bool up_sign, down_sign, left_sign, right_sign;
bool up_sign_past, down_sign_past, left_sign_past, right_sign_past;
int snap_timer;
int snap_terminal;

//variable for reset
int  reset_gyro_timer;

//varialbe for mouse point, type casting
int  IX;
int  IY;
int  IC=0; // for sending rate

bool  change_dir;
int  change_dir_timer;
int  change_dir_terminal;

//mpu 설정===================================
void initMPU6050(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
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

void initDT(){
  t_prev = micros();
}

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

void HandAction(){ // right hand

// ================ tilted angle need at least 20 degree. From here, the code will check which hand action you do. ================
// VX, VY save mouse point moving (delta)
// two types of snap flag ( ex : up_snap, up_snap_past ) can figure out your current hand snap action to keyboard::Arrows

if(filtered_angle_y > 20) // when tilted down 
{
  down_sign = true;
  if( filtered_angle_x > 10 )//|| (filtered_angle_z > 20) ) // left down on computer monitor
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

else if(filtered_angle_y < -20) // when tilted up
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

else
{
  up_sign = false;
  down_sign = false;
  VY = 0;
}

if ( filtered_angle_z > 100 ) // move to left
{
  VX = -sqrt( pow(filtered_angle_x , 2) + 4*pow( 90-abs(filtered_angle_z) , 2) );
  VY = filtered_angle_y;
}
else if ( (filtered_angle_z < 80) && (filtered_angle_z > 50) ) // move to right
{
  VX = sqrt( pow(filtered_angle_x , 2) + 4*pow( 90-abs(filtered_angle_z) , 2) );
  VY = filtered_angle_y;
}
else { }//VX = 0; }

if( (filtered_angle_y < 50) || (filtered_angle_y > -50) )
{
  if( abs(VX) < 80 ) VX = 0;
}

// when tilted side. codes in below only check hand swing
if ( (filtered_angle_x > 37) )
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
else
{
  right_sign = false;
  left_sign = false;
}

//1. flag update set;
  up_sign_past=up_sign;
  down_sign_past=down_sign;
  left_sign_past=left_sign;
  right_sign_past=right_sign;   

//2. sending Mouse point
  IX = ((int)VX)/30;
  IY = ((int)VY)/10;
  //if(NowState == 1)
  //{
      //String newValue2 = "MT."+ (String)IX + "/" + (String)IY +".\n";  // sending mouse pointer!
      //pRemoteCharacteristic->writeValue(newValue2.c_str(), newValue2.length());
      Serial.println("MT."+(String)IX+"/"+(String)IY+".\n");
  //}

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
  VX=0;
  VY=0;
  VZ=0;
  snap_timer = 0;
  snap_terminal = 50; // 50ms. to prevent double check, waiting for 0.05 second.
  reset_gyro_timer = 0;
  IC=0;
  change_dir = false;
  change_dir_timer = 0;
  change_dir_terminal = 50;
  filtered_angle_z = 90;
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
  snap_timer = millis();
  snap_terminal = 50; // 50ms. to prevent double check, waiting for 0.05 second.
  reset_gyro_timer = 0;
  IC=0;
  change_dir_timer = millis();
  change_dir_terminal = 50; // 50ms. to prevent double check, waiting for 0.05 second.
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

  accel_xy = sqrt(pow(accel_x , 2) + pow(accel_y,2));
  accel_angle_z = atan(accel_z/ accel_xy) * RADIANS_TO_DEGREES;
 //advisable code. but it cause infinity. We can`t use this method.

 // accel_angle_z = 0;
  
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

float temp_base;
bool temp_flag;
float new_filtered_z;

void calcFilteredYPR(){
  const float ALPHA = 0.96;
  float tmp_angle_x, tmp_angle_y, tmp_angle_z;

  tmp_angle_x = filtered_angle_x + gyro_x * dt;
  tmp_angle_y = filtered_angle_y + gyro_y * dt;
  tmp_angle_z = filtered_angle_z + gyro_z * dt;

  filtered_angle_x = ALPHA * tmp_angle_x + (1.0-ALPHA) * accel_angle_x;
  filtered_angle_y = ALPHA * tmp_angle_y + (1.0-ALPHA) * accel_angle_y;
  filtered_angle_z = ALPHA * tmp_angle_z + (1.0-ALPHA) * accel_angle_z; // advisable code but it dosen`t work
  //filtered_angle_z = tmp_angle_z; // z is floating in here

  if((temp_flag == false)) // 1000ms 동안 세팅되도록 놔둔다.
  {
    for(int i = 0; i<10; i++)
    {
      temp_base += filtered_angle_z;
      delay(100);
    }
    temp_flag = true;
    temp_base = temp_base/10;
  }

  else if(temp_flag == true)
  {
    filtered_angle_z = filtered_angle_z;//+0.01;
    new_filtered_z += filtered_angle_z - temp_base;
  }
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

//setup=======================================
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Analog IN Test");
  //pinMode(2,INPUT_PULLUP);
  //put your setup code here, to run once:
  initMPU6050();
  Serial.begin(115200);
  calibAccelGyro();
  initDT();
  //accelNoiseTest();

  InitHandAction();
  temp_flag = false;
}


void loop() {
  
    readAccelGyro();
    calcDT();

    calcAccelYPR();
    calcGyroYPR();
    calcFilteredYPR();

    HandAction();

    //Serial.println(new_filtered_z);
    
    //Serial.println(filtered_angle_z);
  delay(20);
}
