#include <Arduino.h>
#include "futbalista.h"

#define MAX_DLZKA_PAKETU 10

// pakety v oboch smeroch zacinaju znakom '$' a koncia znakom '\n'

uint8_t paket[MAX_DLZKA_PAKETU];
uint8_t *p;
uint8_t citam;

void setup_komunikacia() 
{
  p = paket;
  citam = 0;
  while (1)
  {  
    Serial.print("#");
    delay(100);
    if (Serial.available())
    {
      char c = Serial.read();
      if (c == '%') break; 
    }
  }
  Serial.write('^');
}


//spravy RPI -> Arduino:
//  21 - lopta vpravo
//  22 - lopta vlavo
//  23 - lopta v strede
//  24 - nevidi loptu

void spracuj_paket()
{
  //Serial.print("!p:");
  //Serial.println((char *)(paket));
  // na tomto mieste mame v poli paket[] prijaty 0-ukonceny retazec znakov
  int a;
  // v tejto ukazke retazec dekodujeme ako cislo a vypiseme spravu s dvojnasobkom
  // vo vasom pripade budete na tomto mieste reagovat na spravu podla potreby...
  sscanf((char *)paket, "%d", &a);
  if (a >= 97) usmerneny_pohyb(a-97);
  else if (a == 1) test_senzorov();
  else if (a == 2) test_vypinace();
  else if (a == 3) test_motors();
  else if (a == 4) dokola(); 
  else if (a == 5) dolava(); 
  else if (a == 6) dopredu();
  else if (a == 7) doprava(); 
  else if (a == 8) zastav();
  else if (a == 12) soft_kalibracia();
  else if (a == 15) test_kompas();
  else if (a == 31) dolava_vzad();
  else if (a == 32) doprava_vzad();
  else if (a == 33) dozadu();
  else if (a == 90) Serial.print("$zelena je trava\n");
  else if (a == 21) doprava();
  else if (a == 22) dolava();
  else if (a == 23) dopredu();
  else if (a == 24) dokola();
  else if (a == 41) test_left_motor(0);
  else if (a == 42) test_right_motor(0);
  else if (a == 43) test_back_motor(0);
  else if (a == 51) test_left_motor(1);
  else if (a == 52) test_right_motor(1);
  else if (a == 53) test_back_motor(1);
  else if (a == 45) setup_komunikacia();
  else if (a == 55) kick();
  //else if (a == 46) test_kompas();
  
  
}

void precitaj_dalsi_znak()
{
  if (p - paket >= MAX_DLZKA_PAKETU) p = paket;
  
  *p = Serial.read();
  //Serial.print("'");Serial.print(*p);Serial.print("';");
  //digitalWrite(13, HIGH); delay(100); digitalWrite(13, LOW); 
  if (*p == '$') 
  {
    p = paket;
    citam = 1;
  }
  else if (citam)
  {
    if ((*p != '\n') && (*p != '\r')) p++;
    else 
    {
      *p = 0;
      citam = 0;
      if (p > paket)
        spracuj_paket();
      p = paket;
    }
  }
}

void komunikacia() 
{
  if (Serial.available()) precitaj_dalsi_znak();
}
