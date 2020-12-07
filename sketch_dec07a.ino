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
#define SAMPLE_TIME_PROSTO_PD 1000
#define PROSTO_REG_P 1.0
#define PROSTO_REG_D 0.01
MPU6050 IMU;  //deklaracja obiektu reprezentującego układ MPU6050


float yaw=0,target_yaw=0;

// definiowanie pinow dla czujnikow
const int pinLewyCzujnik = 8;
const int pinPrawyCzujnik  = 13;
const int pinPrzedniCzujnik = 12;
// zmienna okreslajaca odległość od ścianek labiryntu
float jednostkaPola;
// srednica robota w [cm] (odleglosc czujnikow od siebie) 
float grRobota = 15;
// zmienne dla startowej odleglosci od scianek labiryntu 
float odlLewoStart, odlPrawoStart;
// zmienne dla odczytu odleglosci z poprzedniego pola
float odlLewoPop, odlPrawoPop;
int los;
//zmienne okreslajace wielkosc labiryntu
int x=16,y=16;
//zmienna okreslajaca pozycje koncowa
int pozKoncowa=x*y-1;
//zmienna okreslajaca pozycje na mapie
int poleStart=0;
int aktpole=0; 
//tablice pozwalajace okreslic polozenie scian wokol kazdego pola
boolean n[255],e[255],s[255],w[255];
//byte kierunki[255];
//zdefiniowanie tablicy, ktora bedzie zawierac wszystkie pola
boolean pole[255];
//poczatkowe ustawienie robota (orientacja w terenie) to 0 stopni (robot ustawiony w kierunku N labiryntu
int konfiguracja=0;
//kierunek ruchu
int kierunek;
//tablica, do ktorej zostana wpisane pola, po ktorych robot ma sie poruszac podczas algorytmu zoptymalizowanego 
int polaRuchu[255];
//tablica, do ktorej wpisywane sa wagi pol wg przyspieszonego algorytmu propagacji fali
int waga[255];
int tablica[255];
int tablica2[255]; //tablica odpowiadajaca za inna orientacje robota w pozycji poczatkowej
//zmienna okreslajaca granice okreslajaca czy sciana jest w obrebie aktualnego pola
int maxOdl;
//zmienna pozwalajaca na realizacje odbierania informacji o sterowaniu ręcznym WiFi
char z;
//zalozenie, ze zaczynamy w lewym dolnym rogu, konczymy w prawym gornym

int spd1=100;
int bladCzujnika=2100;

void prosto(int spd){  //funkcja do jazdy prosto
  //jazda do przodu
  //ustawienie prędkości silnika do jazdy przód/tył
  float startDist = writeValues(pinPrzedniCzujnik, pinPrzedniCzujnik); //pomiar przy rozpoczeciu ruchu
  float currentDist, prev_error;
  unsigned long current_time, prev_reg_time;
  do{
    currentDist = writeValues(pinPrzedniCzujnik, pinPrzedniCzujnik); //pomiary podczas ruchu
    current_time=millis();  //odczyt aktualnego czasu od uruchomienia arduino
    while(current_time-prev_reg_time<SAMPLE_TIME_PROSTO_PD);  //oczekiwanie upłynięcie czasu do końca cyklu(stały czas próbkowania regulatora)

    prev_reg_time=current_time; //zapis ostatniego czasu przeliczenia sygnału sterującego
    int spd_reg = (int)PD_reg(PROSTO_REG_P, PROSTO_REG_D, currentDist, startDist - jednostkaPola, prev_error); //wyznaczenie sygnału sterującego
    prev_error=startDist - jednostkaPola - currentDist;
    spd_reg = constrain(spd_reg, -spd, spd);
    set_motor_spd(DRIVE_ENG_PIN1,DRIVE_ENG_PIN2, -spd_reg);
    poprawPozycje();  
  }while(abs(prev_error)>=1);
  set_motor_spd(DRIVE_ENG_PIN1,DRIVE_ENG_PIN2, 0);
}
void tyl(int spd){
  // IN PROGESS
  //jazda do tyłu
  //ustawienie prędkości silnika do jazdy przód/tył
  float odlLewoCurr = writeValues(pinLewyCzujnik, pinLewyCzujnik);
  float odlPrawoCurr = writeValues(pinPrawyCzujnik, pinPrawyCzujnik);
  if(odlLewoCurr > odlPrawoCurr){
    lewo();
    lewo();
  }
  else{
    prawo();
    prawo();
  }
  prosto(spd1);
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
void korektaLewo(){ // funkcja do korekty odleglosci od prawej sciany
  bool dir=false;
  yaw=get_yaw();  //odczyt aktualnego kąta yaw
  target_yaw=yaw+1; //ustawienie odpowiedniej orientacji
  rotate(target_yaw); //ustawienie zadanej orientacji
  target_yaw=limit_yaw(target_yaw+180); //przeliczenie (teoretycznego)yaw do odpowiedniego zakresu
}
void korektaPrawo(){ // funkcja do korekty odleglosci od lewej sciany
  bool dir=true;
  yaw=get_yaw();  //odczyt aktualnego kąta yaw
  target_yaw=yaw-1; //ustawienie odpowiedniej orientacji
  rotate(target_yaw); //ustawienie zadanej orientacji
  target_yaw=limit_yaw(target_yaw); //przeliczenie (teoretycznego)yaw do odpowiedniego zakresu
}

// funkcja korygujaca odleglosc od scianek robota
void poprawPozycje(){
  float odlLewoTeraz = writeValues(pinLewyCzujnik, pinLewyCzujnik);
  float odlPrawoTeraz = writeValues(pinPrawyCzujnik, pinPrawyCzujnik);
  
  // 20% mniej niz odleglosc czujnika lewego od scianki gdy robot jest na srodku pola 
  if(odlLewoTeraz < 0.8 * odlLewoStart){
    if(odlLewoTeraz < odlLewoPop){ // jesli odleglosc od lewej scianki maleje
      //skrecenie w prawo o 1 st
      korektaPrawo();
    }
  }
  // 20% mniej niz odleglosc czujnika prawego od scianki gdy robot jest na srodku pola
  else if(odlPrawoTeraz < 0.8 * odlPrawoStart){
    if(odlPrawoTeraz < odlPrawoPop){ // jesli odleglosc od prawej scianki maleje
      //skrecenie w lewo o 1 st
      korektaLewo();
    }
  }
  // zapisanie do zmiennych stanu obecnego - w nastepnym cyklu stan poprzedni
  if(odlLewoTeraz < jednostkaPola){
    odlLewoPop = odlLewoTeraz;
  }
  if(odlPrawoTeraz < jednostkaPola){
    odlPrawoPop = odlPrawoTeraz;
  } 
}
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
  prev_yaw_error=target_yaw - yaw;
  turn(turn_spd); //skręcanie robotem
  }while(abs(target_yaw-yaw)>=1);   //skrecanie dopóki uchyb  >=1
  turn(0); // zatrzymanie silnika po skrecie
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
// Odczyt z czujnikow
// funkcja odczytujaca czas przebiegu fali dzwiekowej czujnika
long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // reset triggera
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Ustawienie pinu triggerujacego na HIGH na 10 [us] mikrosekund
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // zwraca czas przebiegu dzwieku w [us] mikrosekundach
  return pulseIn(echoPin, HIGH);
}

// funkcja obliczajaca odleglosc od obiektu w [cm]
float writeValues(int triggerPin, int echoPin)
{
  int cm;
  // obliczenie odleglosci obiektu
  cm = 0.01723 * readUltrasonicDistance(triggerPin, echoPin);
  // zwraca odleglosc od obiektu w [cm]
  return cm;
}

// czujnik przedni
float czujnik1(){
  //moze byc tez int
  //zwraca odleglosc z czujnika przedniego w [cm]
  return writeValues(pinPrzedniCzujnik, pinPrzedniCzujnik);
}

// czujnik prawy
float czujnik2(){
  //zwraca odleglosc z czujnika prawego w [cm]
  return writeValues(pinPrawyCzujnik, pinPrawyCzujnik);
}

// czujnik tylny (fizycznie niewystepujacy)
float czujnik3(){
  //zwraca rozwnowartosc okolo 1.5 jednostki pola w [cm] - zapewnia zawsze pusta przestrzen z tylu
  return 1.5 * jednostkaPola;
}

// czujnik lewy
float czujnik4(){
  //zwraca odleglosc z czujnika lewego w [cm]
  return writeValues(pinLewyCzujnik, pinLewyCzujnik);
}

//funkcja pozwala na dokonanie pomiaru otoczenia, a takze tworzy mape (w postaci tablicy)
void pomiar(int konfiguracja, int i){
  
  int sensor1;
  int sensor2;
  int sensor3;
  int sensor4;

  //w zaleznosci od ustawienia robota w przestrzeni wartosci z czujnikow zostana przypisane zgodnie z kierunkami NESW
  if(konfiguracja==0){
    sensor1=czujnik1();
    sensor2=czujnik2();
    sensor3=czujnik3();
    sensor4=czujnik4();
  }
  if(konfiguracja==90){
    sensor1=czujnik4();
    sensor2=czujnik1();
    sensor3=czujnik2();
    sensor4=czujnik3();
  }
  if(konfiguracja==180){
    sensor1=czujnik3();
    sensor2=czujnik4();
    sensor3=czujnik1();
    sensor4=czujnik2();
  }
  if(konfiguracja==270){
    sensor1=czujnik2();
    sensor2=czujnik3();
    sensor3=czujnik4();
    sensor4=czujnik1();
  }
    
  
  if(sensor1<maxOdl || sensor1>bladCzujnika){ 
    //wartosc 0 oznacza, ze mozliwy jest ruch w tamtym kierunku (nie ma sciany), a 1 oznacza przeszkode 
    n[i]=1;
    //jesli sciana znajduje sie po jednej stronie komorki to odpowiednio po drugiej stronie sciany jest inna komorka 
    //na brzegach labiryntu nie ma sasiadujacych komorek
    if(i>=(x-1)*y && i<=x*y-1);
    else s[i+y]=1;
  }
  else{
    n[i]=0;
    if(i>=(x-1)*y && i<=x*y-1);
    else s[i+y]=0;
    if(i==pozKoncowa) n[1]=1;
  }
  if(sensor2<maxOdl || sensor2>bladCzujnika){
    e[i]=1;
    if((i+1)%y==0);
    else w[i+1]=1;
    
  }
  else{
    e[i]=0;
    if((i+1)%y==0);
    else w[i+1]=0;
    if(i==pozKoncowa) e[1]=1;
  }
  if(sensor3<maxOdl || sensor3>bladCzujnika){
    s[i]=1;
    if(i>=0 && i<y);
    else n[i-y]=1;
  }
  else{
    s[i]=0;
    if(i>=0 && i<y);
    else n[i-y]=0;
    //jesli labirynt jest otwarty na poczatku to niemozliwe ma byc wyjechanie z niego
    if(i==0) s[1]=1;
    
  }
  if(sensor4<maxOdl || sensor4>bladCzujnika){
    w[i]=1;
    if(i%y==0);
    else e[i-1]=1;
  }
  else{
    w[i]=0;
    if((i+1)%y==0) e[i-1]=0;
    //takie dzialanie zapewni, ze robot nie wyjedzie z labiryntu z pola startowego na otwarta przestrzen (poza labirynt)
    if(i==0) w[1]=1;
  }
}

//funkcja sprawdzajaca mozliwy ruch i wybierajaca kierunek ruchu przy algorytmie dogłębnego przeszukiwania
int sprawdzenie(int i){
  int a=1;
  
      //jesli nie ma sciany i pole nie bylo wczesnniej zbadane to wybierze ten kierunek ruchu
      //priorytet wedlug rozy wiatrow NESW
      if(n[i]==0 && pole[i+y]==0){
        return 1;
      }
      else if(e[i]==0 && pole[i+1]==0){
        return 2;
      }
      else if(s[i]==0 && pole[i-y]==0){
        return 3;
      }
      else if(w[i]==0 && pole[i-1]==0){
        return 4;
      }
      //jesli wszystkie pola zostaly zbadane to zostanie wybrany losowo kierunek tam gdzie nie ma sciany
      //kierunek bedzie losowany do skutku, az zostanie znaleziona mozliwa droga ruchu
      else{
        while(1){
          los= random(1,4);
            switch(los) {
              case 1:
                if(n[i]==0){
                  return 1;
                }
              break;
              case 2:
                if(e[i]==0){
                  return 2;
                }
              break;
              case 3:
                if(s[i]==0){
                  return 3;
                }
              break;
              case 4:
                if(w[i]==0){
                  return 4;
                }
              break;
          }
      }
    }
}

//funkcja, ktora pozwala na wykonanie ruchu jesli znane sa obecna orientacja robota w terenie, kierunek dalszego ruchu oraz aktualne pole
void ruch(int konfiguracja, int kierunek, int i){
  
  if(konfiguracja==0){
    if(kierunek==1){
      prosto(spd1);
      aktpole=aktpole+y;
      konfiguracja=0;
    }
    if(kierunek==2){
      prawo();
      prosto(spd1);
      aktpole++;
      konfiguracja=90;
    }
    if(kierunek==3){
      tyl(spd1);
      aktpole=aktpole-y;
      konfiguracja=180; //lub 0
    }
    if(kierunek==4){
      lewo();
      prosto(spd1);
      aktpole--;
      konfiguracja=270;
    }
  }
  if(konfiguracja==90){
     if(kierunek==1){
      lewo();
      prosto(spd1);
      aktpole=aktpole+y;
      konfiguracja=0;
    }
    if(kierunek==2){
      prosto(spd1);
      aktpole++;
      konfiguracja=90;
    }
    if(kierunek==3){
      prawo();
      prosto(spd1);
      aktpole=aktpole-y;
      konfiguracja=180;
    }
    if(kierunek==4){
      tyl(spd1);
      aktpole--;
      konfiguracja=270; //lub 90
    }
  }
  if(konfiguracja ==180){
     if(kierunek==1){
      tyl(spd1);
      aktpole=aktpole+y;
      konfiguracja=0; //lub 180
    }
    if(kierunek==2){
      lewo();
      prosto(spd1);
      aktpole++;
      konfiguracja=90;
    }
    if(kierunek==3){
      prosto(spd1);
      aktpole=aktpole-y;
      konfiguracja=180;
    }
    if(kierunek==4){
      prawo();
      prosto(spd1);
      aktpole--;
      konfiguracja=270;
    }
  }
  if(konfiguracja ==270){
     if(kierunek==1){
      prawo();
      prosto(spd1);
      aktpole=aktpole+y;
      konfiguracja=0;
    }
    if(kierunek==2){
      tyl(spd1);
      aktpole++;
      konfiguracja=90; //lub 270
    }
    if(kierunek==3){
      lewo();
      prosto(spd1);
      aktpole=aktpole-y;
      konfiguracja=180;
    }
    if(kierunek==4){
      prosto(spd1);
      aktpole--;
      konfiguracja=270;
    }
  }
}

int wagi(int orientacja, int i){
    int orientacja1=orientacja;
    if(n[i]==0){
        if(orientacja1==0 || orientacja1==180){
          if(waga[i+y]>waga[i]+1) waga[i+y]=waga[i]+1;
        }
        else{ 
          if(waga[i+y]>waga[i]+3) waga[i+y]=waga[i]+3;
        }
     }
      
     if(e[i]==0){
        if(orientacja1==0 || orientacja1==180)
          if(waga[i+1]>waga[i]+3) waga[i+1]=waga[i]+3;
        else{
          if(waga[i+1]>waga[i]+1) waga[i+1]=waga[i]+1;
        }
     }
     if(s[i]==0){
        if(orientacja1==0 || orientacja1==180)
          if(waga[i-y]>waga[i]+1) waga[i-y]=waga[i]+1;
        else{
          if(waga[i-y]>waga[i]+3) waga[i-y]=waga[i]+3;
        }
     }
     if(w[i]==0){
        if(orientacja1==0 || orientacja1==180)
          if(waga[i-1]>waga[i]+3) waga[i-1]=waga[i]+3;
        else{
          if(waga[i-1]>waga[i]+1) waga[i-1]=waga[i]+1;
        }
     }
}

//funkcja wybierajaca zooptymalizowana sciezke
void sciezka(){
  int i=0;
  tablica[i]=pozKoncowa;
  int pozycja=pozKoncowa;
  i++;
  while(pozycja!=poleStart){
    //wpisujemy do tablicy pole, gdzie mozliwy jest ruch o najmniejszej wadze
    tablica[i]=sprawdzWagi(pozycja,i);
    pozycja=sprawdzWagi(pozycja,i);
    i++;
  }
}

void mapuj(int orientacja,int i){
//jak na północ nie ma ściany i pole na północ nie zostało już zapisane mniejsza liczba ale obecne pole jest zapisane
  if(n[i]==0 && waga[i+y]>waga[i]) {
/*//jak na południe nie ma sciany i waga pola na poludnie jest o 1 mniejsza  to na polnoc wpisz wage o 1 wieksza
  if{s[i]==0 && waga[i-y]==waga[i]-1) waga(i+y)=waga[i]+1;
  //na polnoc wpisz wage o 3 wieksza;
  */
    waga[i+y]=wagi(orientacja,i);
  }
//jak na poludnie nie ma ściany i pole na poludnie nie zostało już zapisane mniejsza liczba ale obecne pole jest zapisane
  if(s[i]==0 && waga[i-y]>waga[i]) {
/*//jak na polnoc nie ma sciany i waga pola na poludnie jest o 1 mniejsza  to na poludnie wpisz wage o 1 wieksza
  if{n[i]==0 && waga[i+y]==waga[i]-1) waga(i-y)=waga[i]+1;
  //na polnoc wpisz wage o 3 wieksza;
  else waga[i-y]=waga[i]+3;*/
    waga[i-y]=wagi(orientacja,i);
  }
//jak na wschod nie ma ściany i pole na wschod nie zostało już zapisane mniejsza liczba ale obecne pole jest zapisane
  if(e[i]==0 && waga[i+1]>waga[i]) {
/*//jak na zachod nie ma sciany i waga pola na zachod jest o 1 mniejsza  to na wschod wpisz wage o 1 wieksza
  if{w[i]==0 && waga[i-1]==waga[i]-1) waga(i+1)=waga[i]+1;
  //na polnoc wpisz wage o 3 wieksza;
  else waga[i+1]=waga[i]+3;*/
    waga[i+1]=wagi(orientacja,i);
  }
//jak na zachod nie ma ściany i pole na zachod nie zostało już zapisane mniejsza liczba ale obecne pole jest zapisane
  if(w[i]==0 && waga[i-1]>waga[i]) {
/*//jak na wschod nie ma sciany i waga pola na wschod jest o 1 mniejsza  to na zachod wpisz wage o 1 wieksza
  if{e[i]==0 && waga[i+1]==waga[i]-1) waga(i-1)=waga[i]+1;
  //na polnoc wpisz wage o 3 wieksza;
  else waga[i-1]=waga[i]+3;*/
    waga[i-1]=wagi(orientacja,i);
  }
}

int getKierunek(int pole1,int pole2){
  if(pole2==pole1+y) return 0;
  else if(pole2==pole1+1) return 90;
  else if(pole2==pole1-y) return 180;
  else if(pole2==pole1-1) return 270;
}

int sprawdzWagi(int i, int a){
  int sasiednieWagi[4]={pozKoncowa,pozKoncowa,pozKoncowa,pozKoncowa};
  //trzeba znalezc minimum, ktore bedzie dokladnie o 1 lub o 3 mniejsze
  if(n[i]==0) sasiednieWagi[1]=waga[i+y];
  if(e[i]==0) sasiednieWagi[2]=waga[i+1];
  if(s[i]==0) sasiednieWagi[3]=waga[i-y];
  if(w[i]==0) sasiednieWagi[4]=waga[i-1];
  int minimum;
  //znalezenie indeksu najmniejszej wagi z tablicy sasiednieWagi
  int mniejsze=1;
  for(int b=2;b<=4;b++){
     //sprawdzenie po kolei wag elementow czy sa mniejsze od poprzedniej wagi
     //jesli tak to mniejszy element jest zastepowany
     if(sasiednieWagi[mniejsze]>sasiednieWagi[b]) mniejsze=b;
     //warunek okreslajacy dzialanie jesli wartosci sa rowne
     else if(sasiednieWagi[mniejsze]==sasiednieWagi[b]){
        //zbadanie roznicy pol z wartoscia poprzednia
        int c=tablica[a-1]-tablica[i];
        //jesli roznica pol wynosi y to, aby ruch byl w linii prostej musimy sie poruszac w kierunku polnocnym lub poludniowym
        if(abs(c)==y){
          if(mniejsze==1 || mniejsze==3);
          //jesli mniejsze nie jet w kierunku polnocnym lub poludniowym to wybieramy wariant b
          //wariant b moze powodowac ruch po linii prostej lub tak ruch jak poprzedni wariant mniejsze
          else mniejsze=b; 
        } 
        //jesli roznica pol jest inna (wynosi 1) to, aby ruch byl w linii prostej musimy sie poruszac w kierunku wschodnim lub zachodnim
        else{
          if(mniejsze==2 || mniejsze==4);
          else mniejsze=b; 
        } 
     }
  }
  minimum=mniejsze;
  
  switch(minimum) {
              case 1:
               return i+y;
              break;
              case 2:
                return i+1;
              break;
              case 3:
                return i-y;
              break;
              case 4:
                return i-1;
              break;
          }
}

void setup() {
  Serial.begin(9600);
  //deklaracja trybów pinów
  pinMode(DRIVE_ENG_PIN1, OUTPUT);
  pinMode(DRIVE_ENG_PIN2, OUTPUT);
  pinMode(TURN_ENG_PIN1, OUTPUT);
  pinMode(TURN_ENG_PIN2, OUTPUT);
  IMU_setup(); //inicjalizacja IMU

  // zmienne dla startowej odleglosci od scianek labiryntu
  odlLewoStart = writeValues(pinLewyCzujnik, pinLewyCzujnik);
  odlPrawoStart = odlLewoStart;
  // zmienne dla odczytu odleglosci z poprzedniego pola
  odlLewoPop = odlLewoStart;
  odlPrawoPop = odlLewoStart;
  
  // obliczenie jednostki pola - 2 * odleglosc od lewej scianki
  jednostkaPola = 2 * odlLewoStart + grRobota;
  maxOdl=jednostkaPola-grRobota;
  
  //zmienna, ktora posluzy do zbadania czy wszystkie pola zostały sprawdzone
  int j=1;
  
  //wpisanie da tablicy waga duzych wartosci
  for(int k=0; k<=sizeof(waga);k++){
    waga[k]=pozKoncowa+1;
  }
  //nadanie polu poczatkowemu wagi 1
  waga[0]=1;
    
  //petla zotanie wykonywana dopoki nie zostana zbadane wszystkie pola
  while(j!=0){
     

    //jesli pole nie zostalo wczesniej zbadane to nastepuje zebranie pomiarow
    if(pole[aktpole]=0){
      pomiar(konfiguracja,aktpole);
      pole[aktpole]=1;
      //gdy pole zostaje zbadane to dokonane zostaje sprawdzenie czterech sasiednich pol czy polozenie scian nie jest wiadome poprzez znajomosc otaczajacych ich pol
      if(pole[aktpole+2*y]==1 && pole[aktpole+y-1]==1 && pole[aktpole+y+1]==1){
        pole[aktpole+y]=1;  
      }
      if(pole[aktpole+y+1]==1 && pole[aktpole+2]==1 && pole[aktpole-y+1]==1){
        pole[aktpole+1]=1;  
      }
      if(pole[aktpole-y+1]==1 && pole[aktpole-2*y]==1 && pole[aktpole-y-1]==1){
        pole[aktpole-y]=1;  
      }
      if(pole[aktpole-y-1]==1 && pole[aktpole-2]==1 && pole[aktpole+y-1]==1){
        pole[aktpole-1]=1;  
      }
    }
    //wywolanie funkcji wybierajacej kierunek ruchu dla algorytmu eksploracyjnego (doglebnego przeszukiwania)
    int kierunek=sprawdzenie(aktpole);
    //mapowanie odbywa się na kazdym polu w fazie badania
    //takie dzialanie pozwoli na szybsze wprowadzenie optymalizacji trasy
    //gdy odbywa się to juz w fazie eksploracji
    mapuj(konfiguracja,aktpole);
    //wywolanie funkcji powodujacej ruch mechanizmu
    ruch(konfiguracja,kierunek,aktpole);
    //ustawienie licznika niezbadanych pol na zero
    j=0;
    //sprawdzenie ile pol zostalo niezbadanych
    for(int k=0;k<255;k++){
      if(pole[k]==0) j++;  
    }
  }
  //po zakonczeniu algorytmu robot powinien zostac ustawiony na pole startowe
  //nastapi oczekiwanie 30 sekund na ustawienie robota
  delay(3000);
  //wpisanie do tablic z wagami najwiekszej mozliwej wartosci
  for(int i=0;i<=pozKoncowa;i++){
    waga[i]=pozKoncowa;
  }
  if(aktpole=0){
    int nastpole;
    //wywolujemy funkcje, ktora utworzy nam trase (wpisze do tablicy kolejne pola)
    //sciezka jest tworzona, gdy robot stoi na polu startowym
    //sciezka jest tworzona dla obecnej orientacji robota w labiryncie
    //dlatego nie potrzeba tworzyc dwoch tablic z wagami dla roznych oreintacji
    sciezka();
    //poruszanie sie po optymalnej trasie bedzie sie wykonywalo dopoki nie zostanie osiagnieta pozycja koncowa
    for(int l;aktpole!=pozKoncowa;l++){
      //sprawdzamy jakie jest nastepne pole do ktorego chcemy sie poruszyc
      nastpole=tablica[sizeof(tablica)-l];
      //sprawdzamy jaki jest kierunek na podstawie pola aktualnego i pola docelowego
      kierunek=getKierunek(aktpole,nastpole);
      //jest wykonywany ruch na pole docelowe
      ruch(konfiguracja,kierunek,aktpole);
    }
  }
}

void loop() {
 //gdy algorytmy sie wykonaja to mozliwe jest reczne sterowanie robotem
  if (Serial.available()) {
    z = Serial.read();
      switch(z) {
              case '1':
                ruch(konfiguracja,1,aktpole);
              break;
              case '2':
                ruch(konfiguracja,2,aktpole);
              break;
              case '3':
                ruch(konfiguracja,kierunek,aktpole);
              break;
              case '4':
                ruch(konfiguracja,kierunek,aktpole);
              break;
          }
          //zamiast switcha
          //int z1=Serial.parseInt()
          //ruch(konfiguracja,z1,aktpole);
  }
}
