
#include <csignal>
#include <cstdio>
#include <ctime>

#include "myV560.h"

volatile bool stop_run(false);
volatile bool pause_run(false);
volatile uint32_t loopcounter(0);

void cntrl_c_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"%s cntrl_c_handler: sig%d\n\n", stime, sig);
  fprintf(stderr,"aborting run - counter %d\n", loopcounter);
  stop_run = true;
 }

void sigterm_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"%s sigterm_handler: sig%d\n\n", stime, sig);
  fprintf(stderr,"terminating run - counter %d\n", loopcounter);
  stop_run = true;
 }

void sigusr1_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"%s sigusr1_handler: sig%d  pause_run is %d\n\n", stime, sig, pause_run);
  pause_run = not pause_run;
  pause_run ?  fprintf(stderr,"pausing run\n") : fprintf(stderr,"resuming run\n");
 }

int main ( int argc, char** argv )
 {
  myCaen mc;
  myV560 scal ( &mc, 0x22000000 );

  // catch signals SIGINT, SIGTERM, SIGUSR1
  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  signal(SIGTERM, sigterm_handler);     // SIGTERM handler
  signal(SIGUSR1, sigusr1_handler);     // SIGUSR1 handler

  scal.clear();

  do {
  usleep(200000);

  uint32_t isped, eoe, we;
  scal.readcounter ( 4, &isped );
  scal.readcounter ( 14, &eoe );
  scal.readcounter ( 15, &we );
  printf ( "scal.inputsetreg isped %d eoe %d we %d\n", isped, eoe, we );
  if (0) for (uint32_t i=0; i<16; i ++)
   {
      uint32_t v;
      scal.readcounter ( i, &v );
      printf ( "scal.inputsetreg cnt %d val %d\n", i, v );
   }

  usleep(200000);

  // scal.scaleincr();
  if (0) for (uint32_t i=0; i<16; i ++)
   {
      uint32_t v;
      scal.readcounter ( i, &v );
      printf ( "scal.inputsetreg cnt %d val %d\n", i, v );
   }

   if (stop_run) goto QuitProgram;

  } while(1);

QuitProgram:

  return 0;
 }
