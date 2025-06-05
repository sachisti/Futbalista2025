#ifndef __FUTBALISTA_H__
#define __FUTBALISTA_H__

#include <inttypes.h>

#define YELLOW 1
#define BLUE 2
#define ROBOT2

extern int opponent_color;
extern int sirka;
extern int vyska;

extern uint8_t gui;

typedef struct veci_str {
   int sirka_lopty, vyska_lopty, velkost_lopty, riadok_lopty, stlpec_lopty;
   int sirka_zltej_branky, vyska_zltej_branky, velkost_zltej_branky, riadok_zltej_branky, stlpec_zltej_branky;
   int sirka_modrej_branky, vyska_modrej_branky, velkost_modrej_branky, riadok_modrej_branky, stlpec_modrej_branky;
} hladane_veci;

extern volatile hladane_veci veci;
extern volatile int mam_veci;

extern uint8_t uhol[10][10];

// komunikacia 

void setup_komunikacia();
void zapis_paket_do_arduina(uint8_t *zapisovany_paket); 
void ukonci_komunikaciu();

// kamera

void najdi_veci_v_obraze(uint8_t *RGB);
void test_kamery();
uint64_t usec();

// logovanie
void setup_log();
void zaloguj(char *sprava);
void zaloguj_n(char *sprava, int cislo);

#endif
