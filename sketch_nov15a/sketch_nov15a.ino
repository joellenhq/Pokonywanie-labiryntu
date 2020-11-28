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

//zmienna okreslajaca granice okreslajaca czy sciana jest w obrebie aktualnego pola
int maxOdl=15;
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
  //zwraca jakas wysoka wartosc, w trakcie pomiaru zawsze przestrzen z tylu będzie pusta
  return 40;
}

//funkcja pozwala na dokonanie pomiaru otoczenia, a takze tworzy mape (w postaci tablicy)
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
    
  
  if(sensor1<maxOdl){ 
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
  }
  if(sensor2<maxOdl){
    e[i]=1;
    if((i+1)%y==0);
    else w[i+1]=1;
  }
  else{
    e[i]=0;
    if((i+1)%y==0);
    else w[i+1]=0;
  }
  if(sensor3<maxOdl){
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
  if(sensor4<maxOdl){
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

void wagi(int orientacja, int i){
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
          if(waga[i+y]>waga[i]+3) waga[i+y]=waga[i]+3;
        else{
          if(waga[i+y]>waga[i]+1) waga[i+y]=waga[i]+1;
        }
     }
     if(s[i]==0){
        if(orientacja1==0 || orientacja1==180)
          if(waga[i+y]>waga[i]+1) waga[i+y]=waga[i]+1;
        else{
          if(waga[i+y]>waga[i]+3) waga[i+y]=waga[i]+3;
        }
     }
     if(w[i]==0){
        if(orientacja1==0 || orientacja1==180)
          if(waga[i+y]>waga[i]+3) waga[i+y]=waga[i]+3;
        else{
          if(waga[i+y]>waga[i]+1) waga[i+y]=waga[i]+1;
        }
     }
}

//funkcja wybierajaca zooptymalizowana sciezke
void sciezka{
  int i=0;
  tablica[i]=pozKoncowa;
  pozycja=pozKoncowa;
  while(pozycja!=poleStart){
    //wpisujemy do tablicy kolejno pola o mniejszych wagach, jesli wagi sa takie same to wybieramy sciezke wzgledem poprzedniej wartosci, jesli nadal takie same to losujemy
    //sprawdzamy pola wokol, od najmniejszej wagi i wybieramy tam gdzie mozliwy jest ruch
    //
    i++;
  }
}

void mapuj(int orientacja,int i){
//jak na północ nie ma ściany i pole na północ nie zostało już zapisane mniejsza liczba ale obecne pole jest zapisane
if(n[i]==0 && waga[i+y]>waga && waga[i]<300) {
/*//jak na południe nie ma sciany i waga pola na poludnie jest o 1 mniejsza  to na polnoc wpisz wage o 1 wieksza
  if{s[i]==0 && waga[i-y]==waga[i]-1) waga(i+y)=waga[i]+1;
  //na polnoc wpisz wage o 3 wieksza;
  */
  waga[i+y]=wagi(orientacja,i);
  
  }
}
//jak na poludnie nie ma ściany i pole na poludnie nie zostało już zapisane mniejsza liczba ale obecne pole jest zapisane
if(s[i]==0 && waga[i-y]>waga && waga[i]<300) {
/*//jak na polnoc nie ma sciany i waga pola na poludnie jest o 1 mniejsza  to na poludnie wpisz wage o 1 wieksza
  if{n[i]==0 && waga[i+y]==waga[i]-1) waga(i-y)=waga[i]+1;
  //na polnoc wpisz wage o 3 wieksza;
  else waga[i-y]=waga[i]+3;*/
  waga[i-y]=wagi(orientacja,i);
  }
}
//jak na wschod nie ma ściany i pole na wschod nie zostało już zapisane mniejsza liczba ale obecne pole jest zapisane
if(e[i]==0 && waga[i+1]>waga && waga[i]<300) {
/*//jak na zachod nie ma sciany i waga pola na zachod jest o 1 mniejsza  to na wschod wpisz wage o 1 wieksza
  if{w[i]==0 && waga[i-1]==waga[i]-1) waga(i+1)=waga[i]+1;
  //na polnoc wpisz wage o 3 wieksza;
  else waga[i+1]=waga[i]+3;*/
  waga[i+1]=wagi(orientacja,i);
  }
}
//jak na zachod nie ma ściany i pole na zachod nie zostało już zapisane mniejsza liczba ale obecne pole jest zapisane
if(w[i]==0 && waga[i-1]>waga && waga[i]<300) {
/*//jak na wschod nie ma sciany i waga pola na wschod jest o 1 mniejsza  to na zachod wpisz wage o 1 wieksza
  if{e[i]==0 && waga[i+1]==waga[i]-1) waga(i-1)=waga[i]+1;
  //na polnoc wpisz wage o 3 wieksza;
  else waga[i-1]=waga[i]+3;*/
  waga[i-1]=wagi(orientacja,i);
  }
}
}


void setup() {
  //zmienna, ktora posluzy do zbadania czy wszystkie pola zostały sprawdzone
  int j=1;
  
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
  //po zakonczeniu algorytmu robot powinien ustawic sie na pole startowe
  //no coz
  //wpisanie do tablic z wagami najwiekszej mozliwej wartosci
  for(int i=0;i<=pozKoncowa;i++){
    waga[i]=pozKoncowa;
  }
  if(aktpole=0){
    //nadanie polu poczatkowemu wagi 1
    waga[aktpole]=1;
    //zdefiniowanie zmiennej, ktora przysluzy do sprawdzenia wag calego labiryntu
    int badanepole=aktpole;
    //wpisanie wag sasiednim polom wedlug konfiguracji (ruch przod/tyl waga++, ruch wymagajacy skretu waga+3)
    wagi(konfiguracja,badanepole);

    //
  }
}

void loop() {
 //wykonanie algorytmu optymalnego
  
}
