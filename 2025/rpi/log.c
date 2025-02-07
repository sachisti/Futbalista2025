#include <stdio.h>
#include <time.h>

static char logsubor[100];
static time_t time_started;

void setup_log()
{
  time(&time_started);

  sprintf(logsubor, "/home/robocup/logs/%lu.txt", time_started);

}

void zaloguj(char *sprava)
{
	FILE *f = fopen(logsubor, "a+");

	time_t tm;
	time(&tm);
	
	tm -= time_started;

	fprintf(f, "%lu %s\n", tm, sprava);
	fclose(f);
}

void zaloguj_n(char *sprava, int cislo)
{
	FILE *f = fopen(logsubor, "a+");

	time_t tm;
	time(&tm);

	fprintf(f, "%lu %s: %d\n", tm, sprava, cislo);
	fclose(f);
}
