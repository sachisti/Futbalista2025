
#include <Adafruit_BMP085.h>
#include <HMC5883L_Simple.h>
#include <MPU6050.h>


#include <Arduino.h>
#include "futbalista.h"

MPU6050 accelgyro;
Adafruit_BMP085 bmp;
HMC5883L_Simple Compass;

void setup_kompas()
{
  Wire.begin();
  // initialize devices
  Serial.println("$I2Cini");

  // initialize bmp085
  if (!bmp.begin()) {
    Serial.println("$BMPerr");
    while (1) {}
  }

  delay(20);
  // initialize mpu6050
  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "$MPUok" : "$MPUfail");
  accelgyro.setI2CBypassEnabled(true); // set bypass mode for gateway to hmc5883L
  
  
  // initialize hmc5883l
  Compass.SetDeclination(8, 27, 'E');
  Compass.SetSamplingMode(COMPASS_SINGLE);
  Compass.SetScale(COMPASS_SCALE_130);
  Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);
}

int kompas()
{
   return (int) Compass.GetHeadingDegrees();
}

void test_kompas()
{
   Serial.print("$");
   Serial.println(kompas());
}
