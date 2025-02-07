#include "futbalista.h"

void setup_strategia()
{
}

void kontrola_senzorov()
{
  if (vidi_ciaru()) 
  {
    obrat_smer();
    //Serial.println("vidi_ciaru() je true");
  }
}

// urobi iba jeden okamzity krok strategie
void strategia()
{

}
