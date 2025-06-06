#include <Arduino.h>
#include "futbalista.h"


#define M1_IN1 4
#define M1_IN2 5

#ifdef ROBOT1

#define M2_IN1 6
#define M2_IN2 7

#define M3_IN1 8
#define M3_IN2 9

#else

#define M3_IN1 6
#define M3_IN2 7

#define M2_IN1 8
#define M2_IN2 9

#endif


#define ML  2
#define MR  3
#define MB  1

#ifdef ROBOT1

#define LFWD 0
#define LBWD 1

#else

#define LFWD 1
#define LBWD 0

#endif

#define RFWD 0
#define RBWD 1

#ifdef ROBOT1

#define BRT  1
#define BLT  0

#else

#define BRT  0
#define BLT  1

#endif

// kam ide
#define IDE_VPRAVO       1
#define IDE_VLAVO        2
#define IDE_ROVNO        3
#define IDE_VZAD         4
#define TOCI_SA          5
#define IDE_VLAVO_VZAD   6
#define IDE_VPRAVO_VZAD  7
#define STOJI            8

#define CAS_OBRAT_SMER 500 //cas na ktory pojde robot do opacneho smeru

volatile uint8_t r1, r2, r3;
volatile uint8_t r1_adjusted, r2_adjusted, r3_adjusted;
volatile uint8_t adjusting = 0;
volatile int direction_correction;

volatile uint8_t t1_tick;
volatile uint8_t on1, off1, on2, off2, on3, off3;

static uint8_t kam_ide = STOJI;

static uint8_t max_speed = 200;
uint8_t curr_speed = 110;   // 0..200

static uint8_t smer_motora[4];

void setup_pohyb()
{
  kam_ide = STOJI;
  adjusting = 0;
  r1 = 0;
  r2 = 0;
  r3 = 0;
  t1_tick = 0;
  on1 = 1;
  off1 = 0;
  on2 = 1;
  off2 = 0;
  on3 = 1;
  off3 = 0;

  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);
  pinMode(M3_IN1, OUTPUT);
  pinMode(M3_IN2, OUTPUT);
  pinMode(10, OUTPUT);

  digitalWrite(M1_IN1, LOW);
  digitalWrite(M1_IN2, LOW);
  digitalWrite(M2_IN1, LOW);
  digitalWrite(M2_IN2, LOW);
  digitalWrite(M3_IN1, LOW);
  digitalWrite(M3_IN2, LOW);

  ICR1 = 160;  // timer1, freq = 100kHz
  // 1600;  // timer1 freq. = 10kHz
  TCNT1 = 0;
  TCCR1A = 0b00000010;
  TCCR1B = 0b00011001;
  TCCR1C = 0;
  TIMSK1 = 0b00000001;

  delay(300);
  motor_smer(1, 0);
  motor_smer(2, 0);
  motor_smer(3, 0);
}

void kick() {
 for (int i = 0;i<2;i++) {
  digitalWrite(10, HIGH);
  delay(90);
  digitalWrite(10, LOW);
  delay(100);
  }
}

void adjust_based_on_compass()
{
  //Serial.println("adjust:");
  if (!adjusting) 
  {
    //Serial.println("not!");
    return;
  }

  Serial.print("r1=");Serial.println(r1);
  Serial.print("r2=");Serial.println(r2);
  Serial.print("r3=");Serial.println(r3);
  Serial.print("corr=");Serial.println(direction_correction);
  
  if (smer_motora[MB] == BRT) // back is going right (BRT)
  {
     if (r1 > direction_correction) 
     {
       if (r1 - direction_correction > max_speed)
         r1_adjusted = max_speed;
       else r1_adjusted = r1 - direction_correction;
     }
     else r1_adjusted = 0;   
  }
  else 
  {
     if (r1 + direction_correction >= 0) 
     {
       if (r1 + direction_correction > max_speed)
         r1_adjusted = max_speed;
       else r1_adjusted = r1 + direction_correction;
     }
     else r1_adjusted = 0;
  }

  if (smer_motora[ML] == LBWD) // left is going backward (LBWD)
  {
     if (r2 > direction_correction) 
     {
       if (r2 - direction_correction > max_speed)
         r2_adjusted = max_speed;
       else r2_adjusted = r2 - direction_correction;
     }
     else r2_adjusted = 0;   
  }
  else 
  {
     if (r2 + direction_correction >= 0) 
     {
       if (r2 + direction_correction > max_speed)
         r2_adjusted = max_speed;
       else r2_adjusted = r2 + direction_correction;
     }
     else r2_adjusted = 0;
  }

  if (smer_motora[MR] == RFWD) // left is going backward (LBWD)
  {
     if (r3 > direction_correction) 
     {
       if (r3 - direction_correction > max_speed)
         r3_adjusted = max_speed;
       else r3_adjusted = r3 - direction_correction;
     }
     else r3_adjusted = 0;   
  }
  else 
  {
     if (r3 + direction_correction >= 0) 
     {
       if (r3 + direction_correction > max_speed)
         r3_adjusted = max_speed;
       else r3_adjusted = r3 + direction_correction;
     }
     else r3_adjusted = 0;
  }
    Serial.print("ar1=");Serial.println(r1_adjusted);
  Serial.print("ar2=");Serial.println(r2_adjusted);
  Serial.print("ar3=");Serial.println(r3_adjusted);

}

void simple_test_motors()
{
  //Serial.println("simple motor test");
  digitalWrite(M1_IN1, LOW);
  digitalWrite(M1_IN2, HIGH);
  delay(1000);
  digitalWrite(M2_IN1, LOW);
  digitalWrite(M2_IN2, HIGH);
  delay(1000);
  digitalWrite(M3_IN1, LOW);
  digitalWrite(M3_IN2, HIGH);
  delay(1000);
  digitalWrite(M1_IN2, LOW);
  digitalWrite(M2_IN2, LOW);
  digitalWrite(M3_IN2, LOW);
}

void motor_speed(uint8_t motor, uint8_t speed)
{
  switch (motor)
  {
    case 1: r1_adjusted = r1 = speed; break;
    case 2: r2_adjusted = r2 = speed; break;
    case 3: r3_adjusted = r3 = speed; break;
  }
}

void motor_smer(uint8_t motor, uint8_t smer)
{
  switch (motor)
  {
    case 1: digitalWrite(M1_IN2, smer); off1 = smer; on1 = 1 - smer; break;
    case 2: digitalWrite(M2_IN2, smer); off2 = smer; on2 = 1 - smer; break;
    case 3: digitalWrite(M3_IN2, smer); off3 = smer; on3 = 1 - smer; break;
  }
  smer_motora[motor] = smer;
}

void test_motors2()
{
  for (int sm = 0; sm < 2; sm++)
  {
    motor_smer(1, sm);
    motor_smer(2, sm);
    motor_smer(3, sm);
    motor_speed(1, 10);
    delay(1000);
    motor_speed(1, 0);
    delay(1000);
    motor_speed(2, 10);
    delay(1000);
    motor_speed(2, 0);
    delay(1000);
    motor_speed(3, 10);
    delay(1000);
    motor_speed(3, 0);
  }
}



void test_motors()
{
  //Serial.println("test motors");

  for (int s = 0; s < 2; s++)
  {
    motor_smer(1, s);
    motor_smer(2, s);
    motor_smer(3, s);
    
    for (int i = 0; i < 21; i++)
    {    
      motor_speed(1, i);
      motor_speed(2, i);
      motor_speed(3, i);
      delay(400);
      if (Serial.available()) break;
    }
  
    for (int i = 20; i >= 0; i--)
    {    
      motor_speed(1, i);
      motor_speed(2, i);
      motor_speed(3, i);
      delay(400);
      if (Serial.available()) break;
    }
    motor_speed(1, 0);
    motor_speed(2, 0);
    motor_speed(3, 0);
  }
}
/*

  for (int i = 0; i < 21; i++)
  {
    //Serial.println(i);

    //Serial.println("1 on");
    motor_speed(1, i);
    delay(1000);
    //Serial.println("1 off");
    motor_speed(1, 0);
    delay(1000);
    //Serial.println("2 on");
    motor_speed(2, i);
    delay(1000);
    //Serial.println("2 off");
    motor_speed(2, 0);
    delay(1000);
    //Serial.println("3 on");
    motor_speed(3, i);
    delay(1000);
    //Serial.println("3 off");
    motor_speed(3, 0);
  }
}*/


ISR(TIMER1_OVF_vect)
{
  if (t1_tick == 0)
  {
    digitalWrite(M1_IN1, on1);
    digitalWrite(M2_IN1, on2);
    digitalWrite(M3_IN1, on3);
  }
  if (t1_tick == r1_adjusted) digitalWrite(M1_IN1, off1);
  if (t1_tick == r2_adjusted) digitalWrite(M2_IN1, off2);
  if (t1_tick == r3_adjusted) digitalWrite(M3_IN1, off3);
  t1_tick++;
  if (t1_tick >= max_speed) t1_tick = 0;
}

void dolava() {
  kam_ide = IDE_VLAVO;
  motor_speed(ML, 0);
  motor_smer(ML, LFWD);
  motor_speed(MR, curr_speed);
  motor_smer(MR, RFWD);
  motor_speed(MB, curr_speed);
  motor_smer(MB, BLT);
}

void doprava_vzad() {
  kam_ide = IDE_VPRAVO_VZAD;
  motor_speed(ML, 0);
  motor_smer(ML, LBWD);
  motor_speed(MR, curr_speed);
  motor_smer(MR, RBWD);
  motor_speed(MB, curr_speed);
  motor_smer(MB, BRT);
}

void doprava() {
  kam_ide = IDE_ROVNO;
  motor_speed(MR, 0);
  motor_smer(MR, RFWD);
  motor_speed(ML, curr_speed);
  motor_smer(ML, LFWD);
  motor_speed(MB, curr_speed);
  motor_smer(MB, BRT);
}

void dolava_vzad() {
  kam_ide = IDE_VLAVO_VZAD;
  motor_speed(MR, 0);
  motor_smer(MR, RBWD);
  motor_speed(ML, curr_speed);
  motor_smer(ML, LBWD);
  motor_speed(MB, curr_speed);
  motor_smer(MB, BLT);
}

void dokola() {
  adjusting = 0;
  kam_ide = TOCI_SA;
  //digitalWrite(13, HIGH);
  motor_smer(MB, BLT);
  motor_speed(MB, curr_speed/4);
  motor_smer(MR, RBWD);
  motor_speed(MR, curr_speed/4);
  motor_smer(ML, LFWD);
  motor_speed(ML, curr_speed/4);
}


void dopredu() {
  kam_ide = IDE_ROVNO;
  motor_smer(MB, BRT);
  motor_speed(MB, 0);
  motor_smer(ML, LFWD);
  motor_speed(ML, curr_speed);
  motor_smer(MR, RFWD);
  motor_speed(MR, curr_speed);
}

void dozadu() {
  kam_ide = IDE_VZAD;
  motor_smer(MB, BRT);
  motor_speed(MB, 0);
  motor_smer(ML, LBWD);
  motor_speed(ML, curr_speed);
  motor_smer(MR, RBWD);
  motor_speed(MR, curr_speed);
}


void zastav() {
  kam_ide = STOJI;
  motor_speed(MB, 0);
  motor_speed(ML, 0);
  motor_speed(MR, 0);
}

void obrat_smer()
{
  switch (kam_ide) {
    case IDE_ROVNO: dozadu(); delay(CAS_OBRAT_SMER); zastav(); break;
    case IDE_VZAD: dopredu(); delay(CAS_OBRAT_SMER); zastav(); break;
    case IDE_VLAVO: doprava_vzad(); delay(CAS_OBRAT_SMER); zastav(); break;
    case IDE_VPRAVO: dolava_vzad(); delay(CAS_OBRAT_SMER); zastav(); break;
    case TOCI_SA: break;
    default: zastav(); break;    
  }
}


void test_left_motor(int where) {
  if (where == 0) where = LFWD;
  else where = LBWD;
  motor_speed(ML, 100);
  motor_smer(ML, where);
  motor_speed(MR, 0);
  motor_smer(MR, RBWD);
  motor_speed(MB, 0);
  motor_smer(MB, BRT);
}

void test_right_motor(int where) {
  if (where == 0) where = RFWD;
  else where = RBWD;
  motor_speed(MR, 100);
  motor_smer(MR, where);
  motor_speed(ML, 0);
  motor_smer(ML, RBWD);
  motor_speed(MB, 0);
  motor_smer(MB, BRT);
}

void test_back_motor(int where) {
  if (where == 0) where = BLT;
  else where = BRT;
  motor_speed(MB, 100);
  motor_smer(MB, where);
  motor_speed(MR, 0);
  motor_smer(MR, RBWD);
  motor_speed(ML, 0);
  motor_smer(ML, LFWD);
}

void vypis_r123()
{
  Serial.print("!r123=(");
  Serial.print(r1);
  Serial.print(",");
  Serial.print(r2);
  Serial.print(",");
  Serial.print(r3);
  Serial.println(")");
    
}

void segment_f(int where) {  // to the right (where=-90..-60)
  kam_ide = IDE_VPRAVO;
  r1_adjusted = r1 = curr_speed;
  r3_adjusted = r3 = curr_speed*(-60-where)/60;
  r2_adjusted = r2 = curr_speed*(where+120)/60;
  vypis_r123();
  motor_smer(ML, LFWD);
  motor_smer(MR, RBWD);
  motor_smer(MB, BRT);
}

void segment_a(int where) {   // to front right (where=-60..0)
  
 kam_ide = IDE_ROVNO;
  r2_adjusted = r2 = curr_speed;
  r3_adjusted = r3 = curr_speed*(where+60)/60;
  r1_adjusted = r1 = curr_speed*(-where)/60;
  vypis_r123();
  motor_smer(ML, LFWD);
  motor_smer(MR, RFWD);
  motor_smer(MB, BRT);
}

void segment_b(int where) {   // to front left (where=0..60)
  kam_ide = IDE_ROVNO;
  r3_adjusted = r3 = curr_speed;
  r2_adjusted = r2 = curr_speed*(60-where)/60;
  r1_adjusted = r1 = curr_speed*where/60;
  vypis_r123();
  motor_smer(ML, LFWD);
  motor_smer(MR, RFWD);
  motor_smer(MB, BLT);
}

void segment_c(int where) {   // to the left (where=60..90)
  kam_ide = IDE_VLAVO;
  r1_adjusted = r1 = curr_speed;
  r2_adjusted = r2 = curr_speed*(where-60)/60;
  r3_adjusted = r3 = curr_speed*(120-where)/60;
  vypis_r123();
  motor_smer(ML, LBWD);
  motor_smer(MR, RFWD);
  motor_smer(MB, BLT);
}

void usmerneny_pohyb(int where) {
  adjusting = 1;
  where *= 2;    // resolution 2 degrees / unit, scale back to degrees
  where -= 90;   // convert from 0..180 to -90 to 90
  // currently only segments in forward 180 degrees directions are covered
  if (where <= -60) segment_f(where);
  else if (where <= 0) segment_a(where);
  else if (where <= 60) segment_b(where);
  else segment_c(where);
}

void riadenie_cez_seriovy_port()
{
  if (Serial.available() > 0)
  {
    char kam = Serial.read();
    Serial.print(kam);
    switch (kam)
    {
      case '0': dokola(); break;
      case '1': dolava(); break;
      case '2': dopredu(); break;
      case '3': doprava(); break;
      case '9': test_motors2(); break;
      case ' ': zastav(); break;
    }
  }
}
