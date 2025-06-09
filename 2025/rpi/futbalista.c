#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include "futbalista.h"
#include "v4l_module.h"
#include "gui.h"

uint8_t headless = 0;
uint8_t gui = 0;
int opponent_color = YELLOW;
extern int zobrazovanie;

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
	printf("10 - suvisly test kamery\n");
	printf("11 - start hry\n");
	printf("12 - soft kalibracia\n");
	printf("70 - ukazuj veci\n");	
	printf("80 - koniec\n");
	printf("90 - test komunikacie\n");
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

void posli_vysoka_rychlost()
{
    char *s = "16";    
    zapis_paket_do_arduina((uint8_t *)s);
}

void posli_normalna_rychlost()
{
    char *s = "17";    
    zapis_paket_do_arduina((uint8_t *)s);
}

void posli_nizka_rychlost()
{
    char *s = "18";    
    zapis_paket_do_arduina((uint8_t *)s);
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
}

char logstr[1000];
static char *paket_kick = "55";

int hra()
{
    int iter = 0;
    zaloguj("futbalista bezi v headless rezime");
    
    do {
	       //printf("lopta: s: %d, v: %d, P: %d, R: %d, S: %d\n", veci.sirka_lopty, veci.vyska_lopty, veci.velkost_lopty, veci.riadok_lopty, veci.stlpec_lopty);
   //printf("zlta: s: %d, v: %d, P: %d, R: %d, S: %d\n", veci.sirka_zltej_branky, veci.vyska_zltej_branky, veci.velkost_zltej_branky, veci.riadok_zltej_branky, veci.stlpec_zltej_branky);
   //printf("modra: s: %d, v: %d, P: %d, R: %d, S: %d\n-----\n", veci.sirka_modrej_branky, veci.vyska_modrej_branky, veci.velkost_modrej_branky, veci.riadok_modrej_branky, veci.stlpec_modrej_branky);


	    int je_pred_nami_nasa_branka = 0;
	    if ((opponent_color == BLUE) && (veci.velkost_zltej_branky > 150))
	        je_pred_nami_nasa_branka =1;
	     if ((opponent_color == YELLOW) && (veci.velkost_modrej_branky > 150))
	        je_pred_nami_nasa_branka =1;

	    int je_pred_nami_jeho_branka = 0;
	    if ((opponent_color == BLUE) && (veci.velkost_modrej_branky > 150))
	        je_pred_nami_jeho_branka =1;
	     if ((opponent_color == YELLOW) && (veci.velkost_zltej_branky > 150))
	        je_pred_nami_jeho_branka =1;
	   
	    int modra_bunka_riadok = veci.riadok_modrej_branky / 72;
            int modra_bunka_stlpec = veci.stlpec_modrej_branky / 128;
            int modra_branka_uhol = uhol[modra_bunka_riadok][modra_bunka_stlpec];
	    int vzdial_modra_branka = vzdial[modra_bunka_riadok][modra_bunka_stlpec];

	    if ((modra_branka_uhol == -1) && (opponent_color == BLUE))
	        {
		 je_pred_nami_jeho_branka = 0;
		}
		int zlta_bunka_riadok = veci.riadok_zltej_branky / 72;
            int zlta_bunka_stlpec = veci.stlpec_zltej_branky / 128;
            int zlta_branka_uhol = uhol[zlta_bunka_riadok][zlta_bunka_stlpec];
        int vzdial_zlta_branka = vzdial[zlta_bunka_riadok][zlta_bunka_stlpec];

	    if ((modra_branka_uhol == -1) && (opponent_color == YELLOW))
	       {
		je_pred_nami_jeho_branka = 0;
	       }

	if (!veci.stlpec_lopty) 
	{
	    posli_nevidi_loptu();
	    posli_normalna_rychlost();
	    iter++;
	    if (iter % 100 == 0)
	    { 
	     sprintf(logstr, "back:%d,ball=none -> search", je_pred_nami_nasa_branka);
	     zaloguj(logstr);
	     //zaloguj_n("hra() iter", iter);
	    }
	}
	else
	{
	    int bunka_riadok = veci.riadok_lopty / 72;
            int bunka_stlpec = veci.stlpec_lopty / 128;
            int uhol_lopta = uhol[bunka_riadok][bunka_stlpec];
	    int vzdial_lopta = vzdial[bunka_riadok][bunka_stlpec];

	    if (uhol_lopta == -1) 
	    {
		posli_nevidi_loptu();
		iter++;
	        if (iter % 100 == 0)
	        { 
		 sprintf(logstr, "back:%d,ball=[%d,%d,%d],yg=[%d,%d,%d],bg=[%d,%d,%d],balldir=%d -> search", je_pred_nami_nasa_branka, 
			  veci.stlpec_lopty, veci.riadok_lopty, veci.velkost_lopty,
			  veci.stlpec_zltej_branky, veci.riadok_zltej_branky, veci.velkost_zltej_branky,
			  veci.stlpec_modrej_branky, veci.riadok_modrej_branky, veci.velkost_modrej_branky,
			  uhol_lopta);
	         zaloguj(logstr);
		 //zaloguj_n("hra() iter", iter);
	        }
	    }
	    else if veci.velkost_lopty = 10 {
		    posli_nevidi_loptu();
	    }
	    else  // normal case when ball is seen
	    {
              int uhol_smer = (180 - uhol_lopta) / 2 + 97;
	      uint8_t paket_smer[10];
	      sprintf((char *)paket_smer, "%d", uhol_smer);
	      zapis_paket_do_arduina(paket_smer);


	      if (je_pred_nami_nasa_branka) 
		posli_nizka_rychlost();
	      else if (je_pred_nami_jeho_branka) 
		posli_vysoka_rychlost();


              if ((vzdial_lopta <= 5) && je_pred_nami_jeho_branka)
	        zapis_paket_do_arduina(paket_kick);

	      iter++;
	      if (iter % 100 == 0)
	      { 
		 sprintf(logstr, "back:%d,ball=[%d,%d,%d],yg=[%d,%d,%d],bg=[%d,%d,%d],balldir=%d -> %d", je_pred_nami_nasa_branka, 
		          veci.stlpec_lopty, veci.riadok_lopty, veci.velkost_lopty,
			  veci.stlpec_zltej_branky, veci.riadok_zltej_branky, veci.velkost_zltej_branky,
			  veci.stlpec_modrej_branky, veci.riadok_modrej_branky, veci.velkost_modrej_branky,
			  uhol_lopta, uhol_smer);
	         zaloguj(logstr);
	         //zaloguj_n("hra() iter", iter);
	      }     
	    }
        }


    usleep(10000);
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
	    else if (a == 70) { zobrazovanie ^= 1; printf("zobrazenie: %d\n", zobrazovanie); }
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
    } while (a != 80);
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

