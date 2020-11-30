//UWAGA: 
//Dla uniknięcia błędów w przypadku osobliwych sytuacji dla regulatora aktualny yaw jest sprowadzany do 180 stopni

//biblioteki do obsługi IMU
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#define DRIVE_ENG_PIN1 5 //pin1 PWM silnika lewego
#define DRIVE_ENG_PIN2 9 //pin2 PWM silnika lewego
#define TURN_ENG_PIN1 6 //pin1 PWM silnika prawego
#define TURN_ENG_PIN2 10 //pin2 PWM silnika prawego
#define SAMPLE_TIME_YAW_PD 1000 //czas próbkowania w ms regulatora PD do skręcania
#define TURN_REG_P 1.0 //wzmocnienie P regulatora PD regulującego orientację 
#define TURN_REG_D 0.01 //czas różniczkowania regulatora PD regulującego orientację 

MPU6050 IMU;  //deklaracja obiektu reprezentującego układ MPU6050


float yaw=0,target_yaw=0;
void setup() {
  //deklaracja trybów pinów
  pinMode(DRIVE_ENG_PIN1, OUTPUT);
  pinMode(DRIVE_ENG_PIN2, OUTPUT);
  pinMode(TURN_ENG_PIN1, OUTPUT);
  pinMode(TURN_ENG_PIN2, OUTPUT);
  IMU_setup(); //inicjalizacja IMU
  
}

void loop() {
  
  
}

void prosto(int spd){  //funkcja do jazdy prosto
  //jazda do przodu
  //ustawienie prędkości silnika do jazdy przód/tył
  set_motor_spd(DRIVE_ENG_PIN1,DRIVE_ENG_PIN2, spd);  
}
void tyl(int spd){
  //jazda do tyłu
  //ustawienie prędkości silnika do jazdy przód/tył
  set_motor_spd(DRIVE_ENG_PIN2,DRIVE_ENG_PIN1, spd);  
}
void lewo(){  //funkcja do skrętu w lewo
  bool dir=false;
  yaw=get_yaw();  //odczyt aktualnego kąta yaw
  target_yaw=yaw+90; //ustawienie odpowiedniej orientacji
  rotate(target_yaw); //ustawienie zadanej orientacji
  target_yaw=limit_yaw(target_yaw+180); //przeliczenie (teoretycznego)yaw do odpowiedniego zakresu
}
void prawo(){
  bool dir=true;
  yaw=get_yaw();  //odczyt aktualnego kąta yaw
  target_yaw=yaw-90; //ustawienie odpowiedniej orientacji
  rotate(target_yaw); //ustawienie zadanej orientacji
  target_yaw=limit_yaw(target_yaw); //przeliczenie (teoretycznego)yaw do odpowiedniego zakresu

}
float limit_yaw(float yaw_val){ //funkcja ograniczająca (teoretyczny)yaw do zakresu 0 do 360 stopni
  if(yaw_val<0){ 
    yaw_val=yaw_val+360;
  }
  else if(yaw_val>360){
    yaw_val=yaw_val-360;
  }
  return yaw_val;
}

void rotate(float target_yaw){    //funkcja służąca ustawiania zadanej orientacji robota
  unsigned long current_time, prev_yaw_reg_time=0;
  float prev_yaw_error=0;
  float yaw_offset;
  yaw=get_yaw();  //odczyt aktualnego kąta yaw
  yaw_offset=180-yaw;  //zapis offsetu od 180 stopni
  do{
  yaw=get_yaw();  //odczyt aktualnego kąta yaw
  current_time=millis();  //odczyt aktualnego czasu od uruchomienia arduino
  while(current_time-prev_yaw_reg_time<SAMPLE_TIME_YAW_PD);  //oczekiwanie upłynięcie czasu do końca cyklu(stały czas próbkowania regulatora)

  prev_yaw_reg_time=current_time; //zapis ostatniego czasu przeliczenia sygnału sterującego
  int turn_spd = (int)PD_reg(TURN_REG_P, TURN_REG_D, yaw+yaw_offset, target_yaw + yaw_offset, prev_yaw_error); //wyznaczenie sygnału sterującego
  turn(turn_spd); //skręcanie robotem
  }while(abs(target_yaw-yaw)>=1);   //skrecanie dopóki uchyb  >=1
}
void turn(int turn_spd){  //funkcja służąca do skręcania robotem
  //dodatnia prędkość skręcania to skręt w prawo, ujemna w lewo
  if(turn_spd>=0)
    set_motor_spd(TURN_ENG_PIN1, TURN_ENG_PIN2, turn_spd);
  else if(turn_spd<0)
    set_motor_spd(TURN_ENG_PIN2, TURN_ENG_PIN1, turn_spd);
}

float PD_reg(float P, float D, float current_val, float target_val, float prev_error){  //funkcja wyznaczająca sygnał sterujący (regulator PD)
  float error= target_val-current_val;
  float sig_out= P*error+D*(error-prev_error);
  prev_error=error;
  return sig_out;
}

void set_motor_spd(int pin1, int pin2, int spd){  //funkcja sterująca silnikiem
  spd = constrain(spd, 0, 255); 
  digitalWrite(pin1, LOW);
  analogWrite(pin2, abs(spd)); 
}

void IMU_setup(){   //funkcja do inicjalizacji IMU}
  uint8_t devStatus;      // status/wynik wykonywanych operacji
  //podłączenie do i2c
  Wire.begin();

  IMU.initialize(); //inicjalizacja IMU

  devStatus = IMU.dmpInitialize(); //inicjalizacja DMP(wbudowanego procesora IMU)

  //ustawienie offsetów dla poszczególnych osi Żyroskopu
  IMU.setXGyroOffset(22);
  IMU.setYGyroOffset(3);
  IMU.setZGyroOffset(2);

  //sprawdzenie czy operacja sie powiodła
  if (devStatus == 0) {
    IMU.setDMPEnabled(true); //włączenie DMP(wbudowanego procesora IMU)
  }
  else
    error("IMU_init_error");
}

float get_yaw() {  //funkcja odczytu danych z IMU(zwraca wartość yaw w zakresie od -180 do 180 stopni)
  Quaternion q;           // [w, x, y, z]         obiekt reprezentujący kwaternion
  VectorFloat gravity;    // [x, y, z]            obiekt reprezentujący wektor grawitacji
  uint8_t fifoBuffer[64]; // bufor kolejki FIFO
  float ypr[3];           // [yaw, pitch, roll]   tablica przechowujaca katy RPY
  if (IMU.dmpGetCurrentFIFOPacket(fifoBuffer)){
    IMU.dmpGetQuaternion(&q, fifoBuffer);
    IMU.dmpGetGravity(&gravity, &q);
    IMU.dmpGetYawPitchRoll(ypr, &q, &gravity);
    return (ypr[0]*180/M_PI+180);
    }
    return 400;
}

void error(char* msg){
  //kod do obsługi błedów tutaj
}
