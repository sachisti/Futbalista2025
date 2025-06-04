#include <inttypes.h>

// vsetky prototypy funkcii

// pohyb

void setup_pohyb();
void riadenie_cez_seriovy_port();

void dolava();
void doprava();
void dokola();
void dopredu();
void zastav(); 
void dolava_vzad();
void doprava_vzad();
void dozadu();

void obrat_smer();

void kick();

void simple_test_motors();
void motor_speed(uint8_t motor, uint8_t speed);
void motor_smer(uint8_t motor, uint8_t smer);
void test_motors2();
void test_motors();

void test_left_motor(int where);
void test_right_motor(int where);
void test_back_motor(int where);

void usmerneny_pohyb(int where);
void adjust_based_on_compass();

// vypinace

void bezpecnostny_vypinac();
void setup_vypinace();
void test_vypinace();
uint8_t config_on();
uint8_t start_on();

// senzory (pouzi analogRead(0)-analogRead(7) okrem 4,5

void setup_senzory();
void test_senzorov();
int vidi_ciaru();
void soft_kalibracia();

// kompas

void setup_kompas();
int kompas();
void test_kompas();
void read_kompas();

// komunikacia

void setup_komunikacia();
void komunikacia();        // treba volat v hlavnej slucke casto

// strategia

void setup_strategia();
void strategia();          // treba volat v hlavnej slucke casto
void kontrola_senzorov();
