#include <inttypes.h>

#define YELLOW 1
#define BLUE 2

extern int opponent_color;

// komunikacia 

int setup_komunikacia();
void zapis_paket_do_arduina(uint8_t *zapisovany_paket); 
void ukonci_komunikaciu();

// kamera

int setup_kamera();
void ukonci_kameru();
void najdi_veci(int *sirka_lopty, int *vyska_lopty, int *velkost_lopty, int *riadok_lopty, int *stlpec_lopty,
               int *sirka_zltej_branky, int *vyska_zltej_branky, int *velkost_zltej_branky, int *riadok_zltej_branky, int *stlpec_zltej_branky,
               int *sirka_modrej_branky, int *vyska_modrej_branky, int *velkost_modrej_branky, int *riadok_modrej_branky, int *stlpec_modrej_branky);
void test_kamery();


// logovanie
void setup_log();
void zaloguj(char *sprava);
void zaloguj_n(char *sprava, int cislo);

