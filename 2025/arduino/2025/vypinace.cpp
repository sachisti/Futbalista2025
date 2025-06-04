#include <Arduino.h>
#include "futbalista.h"

#define START_SWITCH 11
#define CONFIG_SWITCH 12

extern volatile uint8_t r1, r2, r3;

void bezpecnostny_vypinac() {
    if (digitalRead(START_SWITCH) == 0)
    {
      r1 = r2 = r3 = 0;
      zastav();
      while (digitalRead(START_SWITCH) == 0) {      
        r1 = r2 = r3 = 0;
      }
    }
}

void setup_vypinace()
{
  pinMode(CONFIG_SWITCH, INPUT);
  pinMode(START_SWITCH, INPUT);
  digitalWrite(CONFIG_SWITCH, HIGH);  // config pull-up
  digitalWrite(START_SWITCH, HIGH); // config pull-up
}


uint8_t config_on()
{
  return digitalRead(CONFIG_SWITCH);
}

uint8_t start_on()
{
  return digitalRead(START_SWITCH);
}

void test_vypinace()
{
  while (!Serial.available())
  {
    if (digitalRead(START_SWITCH) == 0)
    {
  Serial.println('start switch on');
  delay(500);
    }
    if (digitalRead(CONFIG_SWITCH) == 0)
    {
  Serial.println('config switch on');
  delay(500);
    }
  }
}
