int pozKoncowa=255;
int los;
//zmienne okreslajace wielkosc labiryntu
int x=16,y=16;
//zmienna okreslajaca pozycje na mapie
int aktpole; 
//tablice pozwalajace okreslic polozenie scian wokol kazdego pola
boolean n[255],e[255],s[255],w[255];
//byte kierunki[255];
    //zdefiniowanie tablicy, ktora bedzie zawierac wszystkie pola
    boolean pole[255];
//zalozenie, ze zaczynamy w lewym dolnym rogu, konczymy w prawym gornym

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
    
  
  if(sensor1<15){ 
    n[i]=1;
    if(i>=(x-2)*y && i<(x-1)*y-1) s[i-y]=1;
  }
  else{
    n[i]=0;
    if(i>=0 && i<y) s[i-y]=0;
  }
  if(sensor2<15){
    e[i]=1;
    if(i%y==0) w[i-1]=1;
  }
  else{
    e[i]=0;
    if(i%y==0) w[i-1]=0;
  }
  if(sensor3<15){
    s[i]=1;
    if(i>=0 && i<y) n[i-y]=1;
    if(i==1) s[1]=0;
  }
  else{
    s[i]=0;
    if(i>=0 && i<y) n[i-y]=0;
  }
  if(sensor4<15){
    w[i]=1;
    if((i+1)%y==0) e[i+1]=1;
    //takie dzialanie zapewni, ze robot nie wyjedzie z labiryntu z pola startowego na otwarta przestrzen
    if(i==1) w[1]=0;
  }
  else{
    w[i]=0;
    if((i+1)%y==0) e[i+1]=0;
  }
  
}

int sprawdzenie(int i){
  int a=1;
  while(1){
 
     
      if(n[i]==1 && pole[i+y]==0){
        return 1;
      }
      else if(e[i]==1 && pole[i+1]==0){
        return 2;
      }
      else if(s[i]==1 && pole[i-y]==0){
        return 3;
      }
      else if(w[i]==1 && pole[i-1]==0){
        return 4;
      }
      else{
        los= random(1,4);
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
  }
    }
  }
}

void konfiguracja1(int konfiguracja, int i){
  int kierunek=sprawdzenie(i);
  if(konfiguracja==0){
    if(kierunek==1){
      prosto();
      aktpole=aktpole+y;
      konfiguracja=0;
    }
    if(kierunek==2){
      prawo();
      aktpole++;
      konfiguracja=90;
    }
    if(kierunek==3){
      tyl();
      aktpole=aktpole-y;
      konfiguracja=180; //lub 0
    }
    if(kierunek==4){
      lewo();
      aktpole--;
      konfiguracja=270;
    }
  }
  if(konfiguracja==90){
     if(kierunek==1){
      lewo();
      aktpole=aktpole+y;
      konfiguracja=0;
    }
    if(kierunek==2){
      prosto();
      aktpole++;
      konfiguracja=90;
    }
    if(kierunek==3){
      prawo();
      aktpole=aktpole-y;
      konfiguracja=180;
    }
    if(kierunek==4){
      tyl();
      aktpole--;
      konfiguracja=270; //lub 90
    }
  }
  if(konfiguracja ==180){
     if(kierunek==1){
      tyl();
      aktpole=aktpole+y;
      konfiguracja=0; //lub 180
    }
    if(kierunek==2){
      lewo();
      aktpole++;
      konfiguracja=90;
    }
    if(kierunek==3){
      prosto();
      aktpole=aktpole-y;
      konfiguracja=180;
    }
    if(kierunek==4){
      prawo();
      aktpole--;
      konfiguracja=270;
    }
  }
  if(konfiguracja ==270){
     if(kierunek==1){
      prawo();
      aktpole=aktpole+y;
      konfiguracja=0;
    }
    if(kierunek==2){
      tyl();
      aktpole++;
      konfiguracja=90; //lub 270
    }
    if(kierunek==3){
      lewo();
      aktpole=aktpole-y;
      konfiguracja=180;
    }
    if(kierunek==4){
      prosto();
      aktpole--;
      konfiguracja=270;
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
     

    //jesli pole nie zostalo wczesniej zbadane to nastepuje zebranie pomiarow
    if(pole[aktpole]=0){
      pomiar(konfiguracja,aktpole);
      pole[aktpole]=1;
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

    //wywolanie funkcji powodujacej ruch mechanizmu
    konfiguracja1(konfiguracja, aktpole);
    //zastosowanie algorytmu siłowego do eksploracji labiryntu
    //nie wiem czy to silowy, ale wydaje sie ok
    
    //ustawienie licznika niezbadanych pol na zero
    j=0;
    //sprawdzenie ile pol zostalo niezbadanych
    for(int k=0;k<255;k++){
      if(pole[k]==0) j++;  
    }
  
  }
}

void loop() {
 //wykonanie algorytmu optymalnego
  
}
