#include <Arduino.h>
#include <EEPROM.h> 
#include "futbalista.h"

//A0 - zadne kolo pravy
//A1 - prave kolo zadny
//A2 - prave kolo predny
//A3 - lave kolo predny
//A6 - zadne kolo lavy
//A7 - lave kolo zadny

int val = 0;
int min[6], max[6], prah[6];

void nacitaj_kalibraciu_z_EEPROM()
{
  for (int i = 0; i < 6; i++)
  {  
    prah[i] = EEPROM.read(i * 2) + (EEPROM.read(i * 2 + 1) << 8);
  }  
  Serial.println("$loadedcalib");
}

void zapis_kalibraciu_do_EEPROM()
{
  for (int i = 0; i < 6; i++)
  {  
    EEPROM.write(i * 2, prah [i]&255);
    EEPROM.write(i * 2 + 1, prah [i]>>8);
  }
  Serial.println("$donecalib");
}

void test_senzorov()
{
    Serial.print("$");
    for (int i=0; i<6; i++)
    {
        if (i>3) val=analogRead (i+2);
        else val=analogRead (i);
     
        if (val<prah[i]) Serial.print("0");  
        else Serial.print("1");
     }
     Serial.println();     
}

int vidi_ciaru()
{
    for (int i=0; i<6; i++)
    {
        if (i>3) val=analogRead (i+2);
#ifdef ROBOT2        
        else if (i == 3) val = 2000;  // fix June 5th: A3 is not working currently on ROBOT2 :(
#endif
	else val=analogRead (i);
     
        if (val<prah[i]){
          return 1; // !!!!!
          //return 0;
        }
    }
    return 0;
}

void kalibracia()
{
  while(!config_on()) 
  {  
     Serial.print("$");
     for (int i=0; i<6; i++)
     {              
       if (i>3) val=analogRead (i+2);
       else val=analogRead (i);
       if (val>max[i]) max[i]=val;
       if (val<min[i]) min[i]=val;
       Serial.print(val);
       Serial.print("[");
       Serial.print(min[i]);
       Serial.print("-");
       Serial.print(max[i]);
       if (i < 5) Serial.print("], ");
       else Serial.println("]");
     }     
  }

  for (int i=0; i<6; i++)
  {
       //druha cast programu (vypocet prahovej hodnoty)
       
       // 2/3 of the range min...max 
       prah[i]=(2*max[i]+min[i])/3;    

       // 1/3 of the range min...max 
       //prah[i]=(max[i]+2 * min[i])/3;    
       
       // middle of the range min..max
       //prah[i]=(max[i] * min[i])/2;    
  }
  // zapisat kalibraciu do EEPROM
  zapis_kalibraciu_do_EEPROM();
}

void soft_kalibracia()
{
 while (config_on());
 kalibracia(); 
}

void setup_senzory() 
{  
  //Serial.println("$Zacinam kalibraciu.");
  for (int i = 0; i < 6; i++)
  {
    if (i>3) val=analogRead (i+2);
    else val=analogRead (i);
    min[i]=max[i]=val;
  }

  if (!config_on()) kalibracia();
  else nacitaj_kalibraciu_z_EEPROM();
}
