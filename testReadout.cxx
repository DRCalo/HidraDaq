
#include <csignal>
#include <ctime>
#include <cstdio>

#include "myV977.h"

volatile bool abort_run(false);
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
  abort_run = true;
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

#define yNQTP 6
uint32_t yboard[yNQTP] = { 0x0a000000, 0x0b000000, 0x0c000000, 0x0d000000, 0x01000000, 0x0f000000 };
uint16_t ygeoadrs[yNQTP] = { 5, 6, 7, 8, 9, 10 };

#define xNQTP 3
uint32_t xboard[xNQTP] = { 0x09000000, 0x05000000, 0x08000000 };
uint16_t xgeoadrs[xNQTP] = { 8, 10, 12 };

#define NQTP 7
// uint32_t board[NQTP] = { 0x0a000000, 0x0b000000, 0x0c000000, 0x0d000000, 0x09000000, 0x01000000, 0x0f000000 };
uint32_t board[NQTP] = { 0x0a000000, 0x0b000000, 0x0c000000, 0x05000000, 0x09000000, 0x01000000, 0x08000000 };
uint16_t geoadrs[NQTP] = { 5, 6, 7, 8, 9, 10 };

#define wNQTP 9
// uint32_t wboard[wNQTP] = { 0x05000000, 0x0a000000, 0x0b000000, 0x0c000000, 0x0d000000, 0x09000000, 0x01000000, 0x0f000000, 0x08000000 }; // v862 0x0e000000 removed
uint32_t wboard[wNQTP] = { 0x0d000000, 0x0a000000, 0x0b000000, 0x0c000000, 0x05000000, 0x09000000, 0x01000000, 0x08000000, 0x0f000000 }; // v862 0x0e000000 removed
uint16_t wgeoadrs[wNQTP] = { 4, 5, 6, 7, 8, 9, 10, 11 };

#define NQDC (NQTP-1)

int main ( int argc, char** argv )
 {
  myCaen mc;

  myV977 ioreg ( &mc, 0x01000000 );
  uint16_t regv;

  ioreg.outputsetreg(uint16_t(0x8000));

  for(int i=0; i<NQTP; i++) if ( mc.initQTP ( board[i], geoadrs[1] ) != 0 ) goto QuitProgram;

  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  signal(SIGTERM, sigterm_handler);      // Control-C handler
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

  printf ( "before while loop\n" );
  while (1) {
       ioreg.singlehitreadclearreg(&regv);
    // ioreg.singlehitreadreg(&regv);
    if (0 && (regv & 0x8000))
     {
       printf ( "ioreg.singlehitreadreg %x\n", regv);
       ioreg.singlehitreadclearreg(&regv);
       printf ( "ioreg.multihitreadclearreg %x\n", regv);
     }
    if (abort_run) break;
    if (stop_run) break;
    loopcounter ++;
  }
  printf ( "after while loop ... counter %d\n", loopcounter );
  printf ( "ioreg.singlehitreadreg %x\n", regv);

QuitProgram:

  return 0;
 }
