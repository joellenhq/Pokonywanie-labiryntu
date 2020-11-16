int pozKoncowa=255;
int los;
//zmienne okreslajace wielkosc labiryntu
int x=16,y=16;
//zmienna okreslajaca pozycje na mapie
int aktpole; 
//tablice pozwalajace okreslic polozenie scian wokol kazdego pola
boolean n[255],e[255],s[255],w[255];
//byte kierunki[255];
void lewo(){

  
}
void prawo(){

  
}
void prosto(){

  
}

void tyl(){

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
  else s[i]=0;
  if(sensor4<15) w[i]=1;
  else w[i]=0;
  
}
int sprawdzenie(int i){
  while(1){los= random(1,4);
    switch(los) {
      case 1:
      if(n[i]==1){
        return 1;
      }
      break;
      case 2:
      if(e[i]==1){
        return 2;
      }
      break;
      case 3:
      if(s[i]==1){
        return 3;
      }
      break;
      case 4:
      if(w[i]==1){
        return 4;
      }
      break;
  }}
}

void konfiguracja(int konfiguracja, int i){
  int kierunek=sprawdzenie(i);
  if(konfiguracja==0){
    if(kierunek==1){
      prosto();
      aktpole++;
    }
    if(kierunek==2){
      prawo();
      aktpole=aktpole-y;
    }
    if(kierunek==3){
      tyl();
      aktpole--;
    }
    if(kierunek==4){
      lewo();
      aktpole=aktpole+y;
    }
  }
  if(konfiguracja==90){
     if(kierunek==1){
      lewo();
      aktpole=aktpole+y;
    }
    if(kierunek==2){
      prosto();
      aktpole++;
    }
    if(kierunek==3){
      prawo();
      aktpole=aktpole-y;
    }
    if(kierunek==4){
      tyl();
      aktpole--;
    }
  }
  if(konfiguracja ==180){
     if(kierunek==1){
      tyl();
      aktpole--;
    }
    if(kierunek==2){
      lewo();
      aktpole=aktpole+y;
    }
    if(kierunek==3){
      prosto();
      aktpole++;
    }
    if(kierunek==4){
      prawo();
      aktpole=aktpole-y;
    }
  }
  if(konfiguracja ==270){
     if(kierunek==1){
      prawo();
      aktpole=aktpole-y;
    }
    if(kierunek==2){
      tyl();
      aktpole--;
    }
    if(kierunek==3){
      lewo();
      aktpole=aktpole+y;
    }
    if(kierunek==4){
      prosto();
      aktpole++;
    }
  }
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
    boolean pole[255];
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
    for(int k=0;k<255;k++){
      if(pole[k]==0) j++;  
    }
  
  }
}

void loop() {
 //wykonanie algorytmu 
  
}
