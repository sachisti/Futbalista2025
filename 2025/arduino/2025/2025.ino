#include "futbalista.h"

void setup()
{
  Serial.begin(115200);
  pinMode(13, OUTPUT);

  setup_vypinace();
  setup_pohyb();
  setup_senzory();
  setup_kompas();
  setup_strategia();
  setup_komunikacia();  
}

void loop()
{
  Serial.print(".");
  read_kompas();
   Serial.print("-");
  komunikacia();
  Serial.print(",");
  strategia();
  Serial.print(":");
  kontrola_senzorov();
  //bezpecnostny_vypinac();
  if (start_on())
  {   
    zastav();
    Serial.println("!emergencystop");
    while (start_on());
  }

}
