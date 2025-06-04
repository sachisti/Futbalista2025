#include <termios.h>
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
#ifdef ROBOT1 
#define ZARIADENIE "/dev/ttyS0"
#else
#define ZARIADENIE "/dev/ttyUSB0"
#endif

static int fdZapis[2];
static int fdCitanie[2];

static pid_t plink_pid;
static char paket[MAX_DLZKA_PAKETU];
static volatile int program_bezi;
volatile int pocet_beziacich_vlakien;
static volatile int arduino_inicializovane;

int pripoj_arduino()
{
    int serial_port = open(ZARIADENIE, O_RDWR);
    if (serial_port < 0) {
      printf("Error %i from open: %s\n", errno, strerror(errno));
      return 0;
    } 
    printf("opened\n");

    struct termios tty;

    if(tcgetattr(serial_port, &tty) != 0) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      return 0;
    }
    printf("tcgetattr\n");

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity 
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication 
    tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
    tty.c_cflag |= CS8; // 8 bits per byte 
			
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control 
			     //
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
				   
    tty.c_lflag &= ~ICANON; // Disable canonical mode
    tty.c_lflag &= ~ECHO; // Disable echo

    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl

    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return 0;
    }
    printf("pripoj done\n");
    fdZapis[1] = serial_port;
    fdCitanie[0] = serial_port;
    return 1;
}

int pripoj_arduino_plink()
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

    if (!arduino_inicializovane)
    {
	    write(fdZapis[1], "$45\n", 4);
    }
    while (arduino_inicializovane == 0) 
    {
        if ((precital = read(fdCitanie[0], &ch, 1)) == 1)
	    {
            if (ch == '#') {
                arduino_inicializovane = 1;
		        int zapisal = write(fdZapis[1], "%", 1);
                if (zapisal < 1)
                {
                   perror("nepodarilo sa zapisat paket do arduina");
                   exit(-1);
                }
                printf("arduino inicializovane\n");
            }
	    }
    }
  
      static uint8_t komentar;
      komentar = 0;
      char text_komentara[255];
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
	  if (komentar < 254) text_komentara[komentar++] = ch;
	  if (ch == '!') komentar = 1;
	  else if ((ch == '\n') && komentar) 
	  {
		  text_komentara[komentar] = 0;
		  printf("k: %s", text_komentara);
		  komentar = 0;
	  }
      } while (program_bezi && (ch != '$') & !komentar);
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
}

void spracuj_paket_z_arduina()
{
    // v poli paket[] je precitany 0-ukonceny paket prijaty z arduina
    // na tomto mieste ho mozete spracovat a zareagovat nan
    //printf("arduino->rpi: %s\n", paket);
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

long long next_possibility_to_write = 0;
void zapis_paket_do_arduina(uint8_t *zapisovany_paket)
{
    if (usec() < next_possibility_to_write) 
	    usleep(next_possibility_to_write - usec());

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

    next_possibility_to_write = usec() + (dlzka_paketu + 2) * 400; // 100?

    //tcflush(fdZapis[1], TCIOFLUSH);
    //printf("$%s\n", zapisovany_paket);
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

