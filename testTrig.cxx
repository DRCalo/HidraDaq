
#include <csignal>
#include <ctime>
#include <cstdio>

#include "myV977.h"

volatile bool abort_run(false);
volatile bool pause_run(false);

void cntrl_c_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"%s cntrl_c_handler: sig%d\n\n", stime, sig);
  fprintf(stderr,"aborting run\n");
  abort_run = true;
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
  myV977 ioreg ( &mc, 0x01000000 );
  uint16_t regv;

  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  signal(SIGUSR1, sigusr1_handler);     // Control-USR1 handler

  ioreg.reset();

  ioreg.inputsetreg(&regv);
  printf ( "ioreg.inputsetreg %x\n", regv);

  ioreg.inputmaskreg(&regv);
  printf ( "ioreg.inputmaskreg %x\n", regv);

  ioreg.inputreadreg(&regv);
  printf ( "ioreg.inputreadreg %x\n", regv);

  ioreg.singlehitreadreg(&regv);
  printf ( "ioreg.singlehitreadreg %x\n", regv);

  ioreg.multihitreadreg(&regv);
  printf ( "ioreg.multihitreadreg %x\n", regv);

  ioreg.outputsetreg(&regv);
  printf ( "ioreg.outputsetreg %x\n", regv);


  ioreg.outputmaskreg(&regv);
  printf ( "ioreg.outputmaskreg %x\n", regv);

  ioreg.interruptmaskreg(&regv);
  printf ( "ioreg.interruptmaskreg %x\n", regv);

  /*
  ioreg.singlehitreadclearreg(&regv);
  printf ( "ioreg.singlehitreadclearreg %x\n", regv);

  ioreg.multihitreadclearreg(&regv);
  printf ( "ioreg.multihitreadclearreg %x\n", regv);
  */

  ioreg.testcontrolreg(&regv);
  printf ( "ioreg.testcontrolreg %x\n", regv);

  while (1) {
    ioreg.singlehitreadclearreg(&regv);
    if (abort_run) break;
  }
  printf ( "ioreg.singlehitreadreg %x\n", regv);

  return 0;
 }
