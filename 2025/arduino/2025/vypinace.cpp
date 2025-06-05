#include <Arduino.h>
#include "futbalista.h"

#ifdef ROBOT1

#define START_SWITCH 11
#define CONFIG_SWITCH 12

#else

// second robot has touch, and exchanged connections

#define START_SWITCH 12
#define CONFIG_SWITCH 11

#endif

extern volatile uint8_t r1, r2, r3;

void bezpecnostny_vypinac() {
    if (digitalRead(START_SWITCH) == 1)
    {
      r1 = r2 = r3 = 0;
      zastav();
      while (digitalRead(START_SWITCH) == 1) {      
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

#ifdef ROBOT2

static int config_state = 0;

uint8_t config_on()
{
  if (config_state == 0) 
  {
    if (digitalRead(CONFIG_SWITCH))
    {
      config_state = 1;
      return 0;
    }
    return 1;
  } 

  if (config_state == 1) 
  {
    if (digitalRead(CONFIG_SWITCH) == 0)
    {
      config_state = 2;
    }
    return 0;
  }

  if (config_state == 3)
  {
    if (digitalRead(CONFIG_SWITCH) == 1)
    {
      config_state = 3;
    }
    return 0;
  }

  config_state = 0;
  return 1;
}

#else

uint8_t config_on()
{
  return digitalRead(CONFIG_SWITCH);
}

#endif

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
//  Serial.println("start switch on");
  delay(500);
    }
    if (digitalRead(CONFIG_SWITCH) == 0)
    {
//  Serial.println("config switch on");
  delay(500);
    }
  }
}
