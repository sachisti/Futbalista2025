#include <Arduino.h>
#include "futbalista.h"

#define START_SWITCH 8
#define CONFIG_SWITCH 9

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
      digitalWrite(13, HIGH);
      delay(200);
      digitalWrite(13, LOW);
      delay(200);
    }
    if (digitalRead(CONFIG_SWITCH) == 0)
    {
      digitalWrite(13, HIGH);
      delay(400);
      digitalWrite(13, LOW);
      delay(400);
    }
  }
}
