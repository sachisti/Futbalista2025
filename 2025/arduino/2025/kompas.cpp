#include <QMC5883LCompass.h>
#include <Arduino.h>
#include "futbalista.h"

QMC5883LCompass compass;

static int opponent_dir;
extern volatile int direction_correction;

int kompas()
{
   compass.read();
   int a = compass.getAzimuth() - opponent_dir;
   if (a > 180) a -= 360;
   else if (a < -180) a += 360;
   return a;
}

void setup_kompas()
{
  compass.init();
  compass.setMagneticDeclination(3, 20);  // BARI 3deg 20minutes
  opponent_dir = kompas();


//  // initialize hmc5883l
//  Compass.SetDeclination(8, 27, 'E');
//  Compass.SetSamplingMode(COMPASS_SINGLE);
//  Compass.SetScale(COMPASS_SCALE_130);
//  Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);
}

int bitwidth(int i)
{
  int rv = 0;
  while (i) { rv++; i >>= 1; }
  return rv;
}

void read_kompas()
{
  int a = kompas();
  if (a < 0) direction_correction = -bitwidth(a);
  else direction_correction = bitwidth(a);
  adjust_based_on_compass();
}

void test_kompas()
{
   Serial.print("$");
   Serial.println(kompas());
}
