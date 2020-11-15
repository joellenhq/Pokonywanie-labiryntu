//zmienne okreslajace wielkosc labiryntu
int x=16,y=16;
//zmienna okreslajaca pozycje na mapie
int aktpole; 
//tablice pozwalajace okreslic polozenie scian wokol kazdego pola
boolean n[256],e[256],s[256],w[256];

void lewo(){

  aktpole=aktpole+y;
}
void prawo(){

  aktpole=aktpole-y;
}
void prosto(){

  aktpole++;
}

void tyl(){
  aktpole--;
}

void ukos(int kat){
  //na razie jako idea na koncowe poprawki 
  
}

float czujnik1(){
  //moze byc tez int
}
float czujnik2(){
  
}
float czujnik3(){
  
}
float czujnik4(){
  
}

void pomiar(int konfiguracja, int i){
  
  int sensor1;
  int sensor2;;
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
    
  
  if(sensor1<15) n[i]=1;
  else n[i]=0;
  if(sensor2<15) e[i]=1;
  else e[i]=0;
  if(sensor3<15) s[i]=1;
  else s[i]=1;
  if(sensor4<15) w[i]=1;
  else w[i]=0;
  
}

void setup() {
  //zmienna sprawdzająca czy wszystkie pola zostały sprawdzone
  int j=1;
  //zmienna okreslajaca pozycje na mapie jest ustawiana na 0 (pole poczatkowe)
  aktpole=0;
  
  //konfiguracja robota w przestrzeni wyrazana w stopniach
  int konfiguracja=0;
  
  //petla zotanie wykonywana dopoki nie zostana zbadane wszystkie pola
  while(j!=0){
     
    //zdefiniowanie tablicy, ktora bedzie zawierac wszystkie pola
    boolean pole[256];
    //jesli pole nie zostalo wczesniej zbadane to nastepuje zebranie pomiarow
    if(pole[aktpole]=0){
      pomiar(konfiguracja,aktpole);
      pole[aktpole]=1;
    }
    //zastosowanie algorytmu siłowego do eksploracji labiryntu
    //losowanie z wolnych nesw  z tablicy i wykonanie lewo/prawo/prosto
    //jakis kod
    
    //ustawienie licznika niezbadanych pol na zero
    j=0;
    //sprawdzenie ile pol zostalo niezbadanych
    for(int k=0;k<256;k++){
      if(pole[k]==0) j++;  
    }
  
  }
}

void loop() {
 //wykonanie algorytmu 
  
}
