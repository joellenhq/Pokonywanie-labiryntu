int los;
//zmienne okreslajace wielkosc labiryntu
int x=16,y=16;
//zmienna okreslajaca pozycje koncowa
int pozKoncowa=x*y-1;
//zmienna okreslajaca pozycje na mapie
int poleStart=0;
int aktpole=0; 
//tablice pozwalajace okreslic polozenie scian wokol kazdego pola
boolean n[400],e[400],s[400],w[400];
//byte kierunki[255];
//zdefiniowanie tablicy, ktora bedzie zawierac wszystkie pola
boolean pole[400];
//poczatkowe ustawienie robota (orientacja w terenie) to 0 stopni (robot ustawiony w kierunku N labiryntu
int konfiguracja=0;
//kierunek ruchu
int kierunek;
//tablica, do ktorej zostana wpisane pola, po ktorych robot ma sie poruszac podczas algorytmu zoptymalizowanego 
int polaRuchu[400];
//tablica, do ktorej wpisywane sa wagi pol wg przyspieszonego algorytmu propagacji fali
int waga[400];
int tablica[400];
//zmienna okreslajaca granice okreslajaca czy sciana jest w obrebie aktualnego pola
int maxOdl;
//zmienna pozwalajaca na realizacje odbierania informacji o sterowaniu ręcznym WiFi
char z;
//zalozenie, ze zaczynamy w lewym dolnym rogu, konczymy w prawym gornym

int spd1=100;
int bladCzujnika=2100;
int dlugoscSciezki=0;
int s1,s2,s3,s4;

void fileWrite(char kierunekChar){
  //musze sie jeszcze zastanowic nad tymi plikami i komunikacja po serialu
  if (Serial.available()) {
    Serial.write(kierunekChar);
  }
}

void lewo(){
  fileWrite("d");
}
void prawo(){
  fileWrite("b");
}
void prosto(){
  fileWrite("a");
}

void tyl(){
  fileWrite("d");
}

void fileRead(int obecnepole){
  //wtedy trzeba by odebrac wart pola, znalezc w tablicy wartosc i zwrocic wartosc 0 lub 1
  if (Serial.available()) {
    Serial.write(obecnepole);
    s1=Serial.parseInt();
    s2=Serial.parseInt();
    s3=Serial.parseInt();
    s4=Serial.parseInt();
  }
}

float czujnik1(){
  //przod robota (wedlug jego lokalnego ukladu wspolrzednych)
  return s1;
}
float czujnik2(){
  //prawa strona robota (wedlug jego lokalnego ukladu wspolrzednych)
  return s2;
}
float czujnik3(){
  return s3; 
}
float czujnik4(){
  //lewa strona robota (wedlug jego lokalnego ukladu wspolrzednych)
  return s4;
}

//funkcja pozwala na dokonanie pomiaru otoczenia, a takze tworzy mape (w postaci tablicy)
void pomiar(int konfiguracja, int i){
  fileRead(i);
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
    
  
  if(sensor1==1){ 
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
  if(sensor2==1){
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
  if(sensor3==1){
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
  if(sensor4==1){
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
  
  tablica[dlugoscSciezki]=pozKoncowa;
  int pozycja=pozKoncowa;
  dlugoscSciezki++;
  while(pozycja!=poleStart){
    //wpisujemy do tablicy pole, gdzie mozliwy jest ruch o najmniejszej wadze
    tablica[dlugoscSciezki]=sprawdzWagi(pozycja,dlugoscSciezki);
    pozycja=sprawdzWagi(pozycja,dlugoscSciezki);
    dlugoscSciezki++;
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
  //zmienna, ktora posluzy do zbadania czy wszystkie pola zostały sprawdzone
  int j=1;
  
  //wpisanie da tablicy waga duzych wartosci
  for(int k=0; k<=pozKoncowa;k++){
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
    //mapowanie odbywa się na kazdym pole w fazie badania
    //takie dzialanie pozwoli na szybsze wprowadzenie optymalizacji trasy
    //gdy odbywa się to juz w fazie eksploracji
    mapuj(konfiguracja,aktpole);
    //wywolanie funkcji powodujacej ruch mechanizmu
    ruch(konfiguracja,kierunek,aktpole);
    //ustawienie licznika niezbadanych pol na zero
    j=0;
    //sprawdzenie ile pol zostalo niezbadanych
    for(int k=0;k<pozKoncowa;k++){
      if(pole[k]==0) j++;  
    }
  }
  //po zakonczeniu algorytmu robot powinien ustawic sie na pole startowe
  aktpole=0;
  konfiguracja=0;

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
      nastpole=tablica[dlugoscSciezki-l];
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
              case 'n':
                ruch(konfiguracja,1,aktpole);
              break;
              case 'e':
                ruch(konfiguracja,2,aktpole);
              break;
              case 'w':
                ruch(konfiguracja,3,aktpole);
              break;
              case 's':
                ruch(konfiguracja,4,aktpole);
              break;
          }
          //zamiast switcha
          //tylko trzeba wykonac testy
          //int z1=Serial.parseInt()
          //ruch(konfiguracja,z1,aktpole);
  }
}
