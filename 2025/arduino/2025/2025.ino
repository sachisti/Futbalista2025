#include "futbalista.h"

void setup()
{
  Serial.begin(115200);
  pinMode(13, OUTPUT);

  setup_vypinace();
  setup_pohyb();
  setup_senzory();
  //setup_kompas();5
  setup_strategia();
  setup_komunikacia();  
}

void loop()
{

  komunikacia();
  
  strategia();
  kontrola_senzorov();
  
  if (start_on())
  {   
    zastav();
    while (start_on());
  }

}
