#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>

#include "futbalista.h"

#define MAX_DLZKA_PAKETU 1000

// na nasledujucom riadku mozno treba upravit zariadenie:
#define ZARIADENIE "/dev/ttyUSB0"

static int fdZapis[2];
static int fdCitanie[2];

static pid_t plink_pid;
static char paket[MAX_DLZKA_PAKETU];
static volatile int program_bezi;
static volatile int pocet_beziacich_vlakien;
static volatile int arduino_inicializovane;

int pripoj_arduino()
{
    if (pipe(fdZapis) < 0)
    {
        perror("nepodarilo sa vytvorit pipe na citanie");
        return 0;
    }
    if (pipe(fdCitanie) < 0)
    {
        perror("nepodarilo sa vytvorit pipe na zapisovanie");
        return 0;
    }

    if ((plink_pid = fork()) == 0)
    {
        /* dcersky proces */

        close(0);
        close(1);
        dup2(fdZapis[0], 0);
        dup2(fdCitanie[1], 1);
        close(fdZapis[0]);
        close(fdZapis[1]);
        close(fdCitanie[0]);
        close(fdCitanie[1]);

        if (execl("/usr/bin/plink", "/usr/bin/plink", ZARIADENIE,
                  "-serial", "-sercfg", "115200,N,n,8,1", NULL) < 0)
        {
            perror("nepodarilo sa spustit program plink");
            return 0;
        }
    }

    if (plink_pid < 0)
    {
        perror("nepodarilo sa vytvorit novy proces");
        return 0;
    }

    close(fdZapis[0]);
    close(fdCitanie[1]);
    if (fcntl( fdCitanie[0], F_SETFL, fcntl(fdCitanie[0], F_GETFL) | O_NONBLOCK) < 0)
    {
        perror("nepodarilo sa nastavit pipe ako asynchronnu");
        return 0;
    }

    sleep(1);
    if (waitpid(plink_pid, 0, WNOHANG))
    {
        printf("plink skoncil, pravdepodobne sa nepodarilo nadviazat spojenie s arduinom\n");
        return 0;
    }

    printf("otvorene spojenie s arduinom\n");
    return 1;
}

void precitaj_paket_z_arduina()
{
    unsigned char ch;
    int precital;

    if (arduino_inicializovane == 0) zapis_paket_do_arduina((uint8_t *)"45");
    do
    {
  
      // pockaj na znak $, ktory oznacuje zaciatok paketu
      do {
          ch = 0;
          if ((precital = read(fdCitanie[0], &ch, 1)) < 0)
          {
              if (errno != EAGAIN)
              {
                  perror("nastala chyba pri citani z arduina");
                  exit(-1);
              }
              else usleep(2000);
          }
      } while (program_bezi && (ch != '$'));
      if (!program_bezi) return;
  
      // nacitaj cely riadok az po znak \n
      int precitane = 0;
      do {
            if ((precital = read(fdCitanie[0], paket + precitane, 1)) < 0)
            {
                if (errno != EAGAIN)
                {
                    perror("stala sa chyba pri citani z arduina");
                    exit(-1);
                }
                else usleep(2000); 
            }
            else 
            {
                precitane += precital;
                if (precitane && paket[precitane - 1] == '\n') break;
                if (paket[precitane - 1] == '\r') precitane--; 
                if (precitane > MAX_DLZKA_PAKETU) break;
            }
      } while (program_bezi);
  
      // precitany znak \n nakoniec vymazeme
      if (precitane == 0) precitane++;
      paket[precitane - 1] = 0;
  
      if (arduino_inicializovane == 0)
      {
         if (strcmp(paket, "init") == 0) 
         {
             arduino_inicializovane = 1;
             printf("arduino inicializovane\n");
         }
      } 
      else if (arduino_inicializovane == 1) arduino_inicializovane = 2;
    } while (arduino_inicializovane < 2);
}

void spracuj_paket_z_arduina()
{
    // v poli paket[] je precitany 0-ukonceny paket prijaty z arduina
    // na tomto mieste ho mozete spracovat a zareagovat nan
    printf("arduino->rpi: %s\n", paket);
}

void *citaci_thread(void *args)
{
    precitaj_paket_z_arduina();
    while (program_bezi)
    {
        spracuj_paket_z_arduina();
        precitaj_paket_z_arduina();
    }

    usleep(100000);
    kill(plink_pid, SIGTERM);
    printf("citaci thread skoncil\n");
    pocet_beziacich_vlakien = 0;

    return 0;
}

void zapis_paket_do_arduina(uint8_t *zapisovany_paket)
{
    int zapisal = write(fdZapis[1], "$", 1);
    if (zapisal < 1)
    {
      perror("nepodarilo sa zapisat paket do arduina");
      exit(-1);
    }

    int dlzka_paketu = strlen((char *)zapisovany_paket);
    zapisal = write(fdZapis[1], zapisovany_paket, dlzka_paketu);
    if (zapisal < dlzka_paketu)
    {
      perror("nepodarilo sa zapisat paket na arduino");
      exit(-1);
    }
    zapisal = write(fdZapis[1], "\n", 1);
    if (zapisal < 1)
    {
      perror("nepodarilo sa ukoncit paket pre arduino");
      exit(-1);
    }
}

void setup_komunikacia()
{
    pthread_t t;
    pocet_beziacich_vlakien = 0;

    if (!pripoj_arduino()) exit(-1);
    program_bezi = 1;

    if (pthread_create(&t, 0, citaci_thread, 0) != 0)
    {
      perror("nepodarilo sa vytvorit thread");
      exit(-1);
    }
    else pocet_beziacich_vlakien++;

    while (!arduino_inicializovane) usleep(1000);
}

void ukonci_komunikaciu()
{
    program_bezi = 0;
    close(fdCitanie[0]);
    close(fdZapis[1]); 
    while (pocet_beziacich_vlakien > 0) usleep(1000);
    printf("program skoncil\n");
}

