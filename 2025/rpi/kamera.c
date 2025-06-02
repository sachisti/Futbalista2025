#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>

#include "pngwriter.h"
#include "v4l_module.h"
#include "futbalista.h"
#include "gui.h"


#define VYSKA_OBRAZU 1       //bolo to 2/3, skusime dat cely obraz


// veci co sa hladaju:

#define VEC_LOPTA         0
#define VEC_ZLTA_BRANKA   1
#define VEC_MODRA_BRANKA  2

uint8_t uhol[10][10] = { 
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };

uint8_t vzdial[10][10] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };

uint8_t farba_veci[3][3] = { { 70, 255, 70 }, { 70, 70, 200}, {200, 200, 70 } };

int sirka = 1280; // 640; //1920;
int vyska = 720; // 480; //1080;

hladane_veci veci;
int mam_veci = 0;
 
int je_vec (uint8_t r, uint8_t g, uint8_t b, int vec)
{
  float h, s, v, max, min;
  h = 0;
 
  if ((uint16_t)r + (uint16_t)g + (uint16_t)b == 0) return 0;
   if (r < b){
    if(r < g){
      min = r;
    }
    else{
      min = g;
    }
  }
  else{
    if (b < g){
      min = b;
    }
    else{
      min = g;
    }
  }

  if (r > b){
    if(r > g){
      max = r;
      if (max!= min){
        h = 60*((g-b)/(max-min));
      }
    }
    else{
      max = g;
      if (max!= min){
        h = 60*(2+(b-r)/(max-min));
      }
    }
  }
  else{
    if (b > g){
      max = b;
      if (max!= min){
        h = 60*(4+(r-g)/(max - min));
      } 
    }
    else{
      max = g;
      if (max!= min){
        h = 60*(2+(b-r)/(max - min));
      }
    }
  }
 
  v = max;

  s = (max-min)/max;
  
  if (h < 0){
    h = h + 360;
  }
  
  if (vec == VEC_LOPTA)
  {    
    if ((h < 35) && (h > 0) && (s > 0.45) && (v > 65)){
      return 1;
    }
  }
  else if (vec == VEC_MODRA_BRANKA)
  {
     if (((h > 186) && (h < 240)) && (s > 0.4) && (v > 20) && (v < 150)){
      return 1;
    }    
  }
  else if (vec == VEC_ZLTA_BRANKA)
  {
     if (((h > 45) && (h < 65)) && (s > 0.4) && (v > 120)){
      return 1;
      
    }
  }
    
  return 0;
}

void zisti_rgb(uint8_t *buffer, int riadok, int stlpec, uint8_t *r, uint8_t *g, uint8_t *b)
{
  	      *b = buffer[riadok * sirka * 3 + stlpec * 3];
  	      *g = buffer[riadok * sirka * 3 + stlpec * 3 + 1];
  	      *r = buffer[riadok * sirka * 3 + stlpec * 3 + 2];
}

int minr, mins, maxr, maxs;

int fill(uint8_t *buffer, int riadok, int stlpec, int vec)
{
  if (riadok < minr) minr = riadok;
  if (riadok > maxr) maxr = riadok;
  if (stlpec < mins) mins = stlpec;
  if (stlpec > maxs) maxs = stlpec;
  
  buffer[riadok * sirka * 3 + stlpec * 3] = farba_veci[vec][0];
  buffer[riadok * sirka * 3 + stlpec * 3 + 1] = farba_veci[vec][1];
  buffer[riadok * sirka * 3 + stlpec * 3 + 2] = farba_veci[vec][2];
  
  uint8_t r, g, b;
  
  zisti_rgb(buffer, riadok, stlpec + 1, &r, &g, &b);
  int kolko = 1;
  
  if (je_vec(r, g, b, vec))
    kolko += fill(buffer, riadok, stlpec + 1, vec);

  zisti_rgb(buffer, riadok, stlpec - 1, &r, &g, &b);
  
  if (je_vec(r, g, b, vec))
    kolko += fill(buffer, riadok, stlpec - 1, vec);

  zisti_rgb(buffer, riadok - 1, stlpec, &r, &g, &b);
    
  if (je_vec(r, g, b, vec))
    kolko += fill(buffer, riadok - 1, stlpec, vec);

  zisti_rgb(buffer, riadok + 1, stlpec, &r, &g, &b);
  
  if (je_vec(r, g, b, vec))
    kolko += fill(buffer, riadok + 1, stlpec, vec);

  return kolko;
}

//camera callback
void najdi_veci_v_obraze(uint8_t *RGB)
{

      uint8_t *buffer = RGB;
      uint8_t *p = (uint8_t *)buffer;

      // prechadzame cely obrazok bod po bode...
      // na tomto mieste chcete program upravit podla svojich potrieb...

      // najskor vynulujeme vsetky 4 okraje, aby fill nevybehol mimo rozsah pola
      for (int i = 0; i < vyska; i++)
      {
         // lavy okraj
         buffer[i*sirka*3] = 0;
         buffer[i*sirka*3 + 1] = 0;
         buffer[i*sirka*3 + 2] = 0;
         
         // pravy okraj
         buffer[(i + 1)*sirka*3 - 3] = 0;
         buffer[(i + 1)*sirka*3 - 2] = 0;
         buffer[(i + 1)*sirka*3 - 1] = 0;
      }
      
      int index_zaciatku_dolneho_riadku = (vyska - 1) * sirka * 3;
      for (int i = 0; i < sirka; i++)
      {
         // horny okraj
         buffer[i*3] = 0;
         buffer[i*3 + 1] = 0;
         buffer[i*3 + 2] = 0;
         
         // dolny okraj
         buffer[index_zaciatku_dolneho_riadku + i * 3] = 0;
         buffer[index_zaciatku_dolneho_riadku + i * 3 + 1] = 0;
         buffer[index_zaciatku_dolneho_riadku + i * 3 + 2] = 0;
      }      

      int doteraz_najvacsi[3] = { 0, 0, 0 };
      int doteraz_najv_sirka[3] = { 0, 0, 0 };
      int doteraz_najv_vyska[3] = { 0, 0, 0 };
      int doteraz_najv_riadok[3] = { 0, 0, 0 };
      int doteraz_najv_stlpec[3] = { 0, 0, 0 };
      
      for (int i = 0; i < vyska * VYSKA_OBRAZU; i++)
        for (int j = 0; j < sirka; j++)
        {
  	      uint8_t b = *(p++);
  	      uint8_t g = *(p++);
  	      uint8_t r = *(p++);

  	      if (je_vec(r, g, b, VEC_LOPTA))
  	      {
                  mins = sirka, minr = vyska, maxs = -1, maxr = -1;
                  int pocet = fill(buffer, i, j, VEC_LOPTA);
                  if (pocet > doteraz_najvacsi[VEC_LOPTA])
                  {
                      doteraz_najvacsi[VEC_LOPTA] = pocet;
                      doteraz_najv_sirka[VEC_LOPTA] = maxs - mins + 1;
                      doteraz_najv_vyska[VEC_LOPTA] = maxr - minr + 1;
                      doteraz_najv_riadok[VEC_LOPTA] = (maxr + minr) / 2;
                      doteraz_najv_stlpec[VEC_LOPTA] = (maxs + mins) / 2;
                  }
  	      }
          else if (je_vec(r, g, b, VEC_MODRA_BRANKA))
          {
                  mins = sirka, minr = vyska, maxs = -1, maxr = -1;
                  int pocet = fill(buffer, i, j, VEC_MODRA_BRANKA);
                  if (pocet > doteraz_najvacsi[VEC_MODRA_BRANKA])
                  {
                      doteraz_najvacsi[VEC_MODRA_BRANKA] = pocet;
                      doteraz_najv_sirka[VEC_MODRA_BRANKA] = maxs - mins + 1;
                      doteraz_najv_vyska[VEC_MODRA_BRANKA] = maxr - minr + 1;
                      doteraz_najv_riadok[VEC_MODRA_BRANKA] = (maxr + minr) / 2;
                      doteraz_najv_stlpec[VEC_MODRA_BRANKA] = (maxs + mins) / 2;
                  }
          }
          else if (je_vec(r, g, b, VEC_ZLTA_BRANKA))
          {
                  mins = sirka, minr = vyska, maxs = -1, maxr = -1;
                  int pocet = fill(buffer, i, j, VEC_ZLTA_BRANKA);
                  if (pocet > doteraz_najvacsi[VEC_ZLTA_BRANKA])
                  {
                      doteraz_najvacsi[VEC_ZLTA_BRANKA] = pocet;
                      doteraz_najv_sirka[VEC_ZLTA_BRANKA] = maxs - mins + 1;
                      doteraz_najv_vyska[VEC_ZLTA_BRANKA] = maxr - minr + 1;
                      doteraz_najv_riadok[VEC_ZLTA_BRANKA] = (maxr + minr) / 2;
                      doteraz_najv_stlpec[VEC_ZLTA_BRANKA] = (maxs + mins) / 2;
                  }
          }
        }
      //printf("velkost: %d, sirka: %d, vyska: %d\n", doteraz_najvacsi, 
      //         doteraz_najv_sirka, doteraz_najv_vyska);

      veci.sirka_lopty = doteraz_najv_sirka[VEC_LOPTA];
      veci.vyska_lopty = doteraz_najv_vyska[VEC_LOPTA];
      veci.velkost_lopty = doteraz_najvacsi[VEC_LOPTA];
      veci.riadok_lopty = doteraz_najv_riadok[VEC_LOPTA];
      veci.stlpec_lopty = doteraz_najv_stlpec[VEC_LOPTA];

      veci.sirka_zltej_branky = doteraz_najv_sirka[VEC_ZLTA_BRANKA];
      veci.vyska_zltej_branky = doteraz_najv_vyska[VEC_ZLTA_BRANKA];
      veci.velkost_zltej_branky = doteraz_najvacsi[VEC_ZLTA_BRANKA];
      veci.riadok_zltej_branky = doteraz_najv_riadok[VEC_ZLTA_BRANKA];
      veci.stlpec_zltej_branky = doteraz_najv_stlpec[VEC_ZLTA_BRANKA];

      veci.sirka_modrej_branky = doteraz_najv_sirka[VEC_MODRA_BRANKA];
      veci.vyska_modrej_branky = doteraz_najv_vyska[VEC_MODRA_BRANKA];
      veci.velkost_modrej_branky = doteraz_najvacsi[VEC_MODRA_BRANKA];
      veci.riadok_modrej_branky = doteraz_najv_riadok[VEC_MODRA_BRANKA];
      veci.stlpec_modrej_branky = doteraz_najv_stlpec[VEC_MODRA_BRANKA];
      
      mam_veci = 1;
      if (gui) gui_putimage(buffer);

      //static int iter = 0;
      
      //if (iter++ == 1)
      //~ {
        //~ iter = 0;      
        //~ static int counter = 0;
        //~ char filename[30];
        //~ sprintf(filename, "image%d.png", counter++);
      
//~ #ifdef POUZI_YUV
        //~ write_yuv422_png_image((uint8_t *)buffer, filename, 320, 280);
//~ #else
        //~ write_bgr_png_image((uint8_t *)buffer, filename, 320, 200);
//~ #endif
      //~ }
}

uint64_t usec()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return (1000000UL * (uint64_t)tv.tv_sec) + tv.tv_usec;
}

void test_kamery()
{
   while (!mam_veci) {}
   mam_veci = 0;

   printf("lopta: s: %d, v: %d, P: %d, R: %d, S: %d\n", veci.sirka_lopty, veci.vyska_lopty, veci.velkost_lopty, veci.riadok_lopty, veci.stlpec_lopty);
   printf("zlta: s: %d, v: %d, P: %d, R: %d, S: %d\n", veci.sirka_zltej_branky, veci.vyska_zltej_branky, veci.velkost_zltej_branky, veci.riadok_zltej_branky, veci.stlpec_zltej_branky);
   printf("modra: s: %d, v: %d, P: %d, R: %d, S: %d\n-----\n", veci.sirka_modrej_branky, veci.vyska_modrej_branky, veci.velkost_modrej_branky, veci.riadok_modrej_branky, veci.stlpec_modrej_branky);
}
 
