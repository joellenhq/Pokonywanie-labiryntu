#define ENG_LEFT_PIN1 5 //pin1 PWM silnika lewego
#define ENG_LEFT_PIN2 9 //pin2 PWM silnika lewego
#define ENG_RIGHT_PIN1 6 //pin1 PWM silnika prawego
#define ENG_RIGHT_PIN2 10 //pin2 PWM silnika prawego
#define SAMPLE_TIME_YAW_PD 1000 //czas próbkowania w ms regulatora PD do skręcania

float prev_yaw_error=0, yaw=0;
unsigned long prev_yaw_reg_time=0;
void setup() {
  //deklaracja trybów pinów
  pinMode(ENG_LEFT_PIN1, OUTPUT);
  pinMode(ENG_LEFT_PIN2, OUTPUT);
  pinMode(ENG_RIGHT_PIN1, OUTPUT);
  pinMode(ENG_RIGHT_PIN2, OUTPUT);
  
}

void loop() {
  

}

void move_straight(int spd){  //funkcja do jazdy prosto
  //jeśli prędkość jest dodatnia robot jedzie do przodu, jeśli ujemna do tyłu
  set_motor_spd(ENG_LEFT_PIN1, ENG_LEFT_PIN2, spd);
  set_motor_spd(ENG_RIGHT_PIN1, ENG_RIGHT_PIN2, spd);
}

void set_yaw(float target_yaw){    //funkcja służąca ustawiania zadanej orientacji robota
  unsigned long current_time=millis();  //odczyt aktualnego czasu od uruchomienia arduino
  while(current_time-prev_yaw_reg_time<SAMPLE_TIME_YAW_PD);  //oczekiwanie upłynięcie czasu do końca cyklu(stały czas próbkowania regulatora)

  prev_yaw_reg_time=current_time; //zapis ostatniego czasu przeliczenia sygnału sterującego
  int turn_spd = (int)PD_reg(1.0, 0.01, yaw, target_yaw); //wyznaczenie sygnału sterującego
  turn(turn_spd); //skręcanie robotem
}

float PD_reg(float P, float D, float current_val, float target_val){  //funkcja wyznaczająca sygnał sterujący (regulator PD)
  float error= target_val-current_val;
  float sig_out= P*error+D*(error-prev_yaw_error);
  return sig_out;
}

void turn(int turn_spd){  //funkcja służąca do skręcania robotem
  //dodatnia prędkość skręcania to skręt w prawo, ujemna w lewo
    set_motor_spd(ENG_LEFT_PIN1, ENG_LEFT_PIN2, turn_spd);
    set_motor_spd(ENG_RIGHT_PIN1, ENG_RIGHT_PIN2, -turn_spd);
}

void set_motor_spd(int pin1, int pin2, int spd){  //funkcja sterująca silnikiem
  spd = constrain(spd,-255, 255); 
  if(spd>0){  //obrót silnika w kierunku dodatnim (jazda do przodu)
    digitalWrite(pin1, LOW);
    analogWrite(pin2, spd); 
  }
  else{       //obrót silnika w kierunku ujemnym (jazda do tyłu)
    digitalWrite(pin2, LOW);
    analogWrite(pin1, abs(spd));
  }
}
