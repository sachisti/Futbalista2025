#ifndef __FUTBALISTA_H__
#define __FUTBALISTA_H__

#include <inttypes.h>

#define YELLOW 1
#define BLUE 2

extern int opponent_color;
extern int sirka;
extern int vyska;

typedef struct veci_str {
   int sirka_lopty, vyska_lopty, velkost_lopty, riadok_lopty, stlpec_lopty;
   int sirka_zltej_branky, vyska_zltej_branky, velkost_zltej_branky, riadok_zltej_branky, stlpec_zltej_branky;
   int sirka_modrej_branky, vyska_modrej_branky, velkost_modrej_branky, riadok_modrej_branky, stlpec_modrej_branky;
} hladane_veci;

extern hladane_veci veci;
extern int mam_veci;

// komunikacia 

void setup_komunikacia();
void zapis_paket_do_arduina(uint8_t *zapisovany_paket); 
void ukonci_komunikaciu();

// kamera

void najdi_veci_v_obraze(uint8_t *RGB);
void test_kamery();


// logovanie
void setup_log();
void zaloguj(char *sprava);
void zaloguj_n(char *sprava, int cislo);

#endif
