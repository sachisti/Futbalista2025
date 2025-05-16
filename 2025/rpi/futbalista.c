#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include "futbalista.h"
#include "v4l_module.h"
#include "gui.h"

uint8_t headless = 0;
uint8_t gui = 0;
int opponent_color = YELLOW;

//extern void start_camera_thread();

void navod()
{
	printf("0 - navod\n");
	printf("1 - test senzorov\n");
	printf("2 - test vypinacov\n");
	printf("3 - test pohybov\n");
	printf("4 - test dokola\n");
	printf("5 - test dolava\n");
	printf("6 - test dopredu\n");
	printf("7 - test doprava\n");
	printf("8 - test zastav\n");
	printf("9 - test kamery\n");
	printf("11 - start hry\n");
	printf("100 - koniec\n");
	printf("9999 - test komunikacie\n");
	printf("31 - Arduino dolava_vzad()\n");
	printf("32 - Arduino doprava_vzad()\n");
	printf("33 - Arduino dozadu()\n");
}

//spravy RPI -> Arduino:
//  21 - lopta vpravo
//  22 - lopta vlavo
//  23 - lopta v strede
//  24 - nevidi loptu


void posli_lopta_vpravo()
{
    char *s = "21";    
    zapis_paket_do_arduina((uint8_t *)s);
    printf(">");

}

void posli_lopta_vlavo()
{
    char *s = "22";    
    zapis_paket_do_arduina((uint8_t *)s);
    printf("<");

}

void posli_lopta_vstrede()
{
    char *s = "23";    
    zapis_paket_do_arduina((uint8_t *)s);
    printf("^");
}

void posli_nevidi_loptu()
{
    char *s = "24";    
    zapis_paket_do_arduina((uint8_t *)s);
    printf("0");
}


int hra()
{
    int iter = 0;
    zaloguj("futbalista bezi v headless rezime");
    
    do {
	       //printf("lopta: s: %d, v: %d, P: %d, R: %d, S: %d\n", veci.sirka_lopty, veci.vyska_lopty, veci.velkost_lopty, veci.riadok_lopty, veci.stlpec_lopty);
   //printf("zlta: s: %d, v: %d, P: %d, R: %d, S: %d\n", veci.sirka_zltej_branky, veci.vyska_zltej_branky, veci.velkost_zltej_branky, veci.riadok_zltej_branky, veci.stlpec_zltej_branky);
   //printf("modra: s: %d, v: %d, P: %d, R: %d, S: %d\n-----\n", veci.sirka_modrej_branky, veci.vyska_modrej_branky, veci.velkost_modrej_branky, veci.riadok_modrej_branky, veci.stlpec_modrej_branky);


	    int je_pred_nami_nasa_branka = 0;
	    if ((opponent_color == BLUE) && (veci.velkost_zltej_branky > 50))
	        je_pred_nami_nasa_branka =1;
	     if ((opponent_color == YELLOW) && (veci.velkost_modrej_branky > 50))
	        je_pred_nami_nasa_branka =1;
	   
	     if (!veci.stlpec_lopty) posli_nevidi_loptu();
         else if (veci.velkost_lopty < 10) continue;
         else if (veci.stlpec_lopty < sirka / 3) posli_lopta_vpravo();
	     else if (veci.stlpec_lopty < 2 * sirka / 3)
	     {
	        if (je_pred_nami_nasa_branka) posli_nevidi_loptu();
	        else posli_lopta_vstrede();
	     } else posli_lopta_vlavo();
	
	     iter++;
	     if (iter % 100 == 0)
	         zaloguj_n("hra() iter", iter);

    } while (1);
    
    return 0;
}

void hlavny_program()
{
    if (headless) hra();
    
    zaloguj("futbalista bezi v interaktivnom rezime");
    char sprava[100];
    int a = 0;
    printf("0 = navod\n");

    do {
        printf("filomena> ");
        fgets(sprava, 5, stdin);
        sscanf(sprava, "%d", &a);
        if (a == 0) navod();
	    else if (a == 9) test_kamery();
	    else if (a == 10) 
	    {
		    for (int poc = 0; poc < 300; poc++)
		    {
		      test_kamery();
		      usleep(100000);
		    }
	    }

        else
        {
            sprintf(sprava, "%d", a);
            zapis_paket_do_arduina((uint8_t *)sprava);
        }
    } while (a != 100);
}

void load_color()
{
    FILE *f = fopen("/home/pi/opponent_color", "r");
    char s[17];

    fgets(s,15,f);
    fclose(f);
    if (strncmp(s, "yellow", 6) == 0) {
	 opponent_color = YELLOW;
	 printf("opponent: YELLOW\n");
    }
    else {
	opponent_color = BLUE;
	printf("opponent: BLUE\n");
    }    
}


int main(int argc, char **argv)
{
    if ((argc > 1) && (strcmp(argv[1], "headless") == 0)) headless = 1;
    if ((argc > 1) && (strcmp(argv[1], "gui") == 0)) gui = 1;
    
    load_color();
    setup_log();
    if (gui) setup_gui();
    setup_komunikacia();
    setup_camera_callback(najdi_veci_v_obraze);
    start_camera_thread();

    hlavny_program();

    ukonci_komunikaciu();
    shutdown_gui();
    return 0;
}

