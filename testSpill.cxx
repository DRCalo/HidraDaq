
#include <csignal>
#include <ctime>
#include <cstdio>
#include <sys/time.h>
#include <unistd.h>
#include <sched.h>

#include "myV977.h"

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

#define SEC_OFFSET 1758731600

uint64_t uTime ()
 {
   uint64_t usec(0);
   struct timeval tv;

   int ret = gettimeofday ( &tv, NULL);

   usec = (tv.tv_sec-SEC_OFFSET)*1000000+tv.tv_usec;

   return usec;
 }

int main ( int argc, char** argv )
 {
  uint16_t regv;
  // instance of class myCaen for VME access via A4818 (id 49086) + V3718
  myCaen mc;

   // catch signals SIGINT, SIGTERM, SIGUSR1
  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  signal(SIGTERM, sigterm_handler);     // SIGTERM handler
  signal(SIGUSR1, sigusr1_handler);     // SIGUSR1 handler

  // instance of V977 I/O reg
  myV977 ioreg ( &mc, 0x01000000 );

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

  ioreg.testcontrolreg(&regv);
  printf ( "ioreg.testcontrolreg %x\n", regv);

  while(1)
   {
     // wait for the spill
     while(1)
      {
        printf ( "wait for the spill\n" );
        if (stop_run) goto QuitProgram;
        usleep(100000);
        ioreg.singlehitreadreg(&regv);
        printf ( "ioreg.singlehitreadreg %x\n", regv);
        if (regv & 4) break;
        sched_yield();
      }
     ioreg.singlehitreadclearreg(&regv);
     printf ( "Beam is ON - ioreg.singlehitreadclearreg %x\n", regv);

     // wait for end of spill
     while(1)
      {
        printf ( "wait for end of spill\n" );
        if (stop_run) goto QuitProgram;
        usleep(100000);
        ioreg.singlehitreadreg(&regv);
        printf ( "ioreg.singlehitreadreg %x\n", regv);
        if (regv & 8) break;
        sched_yield();
      }
     ioreg.singlehitreadclearreg(&regv);
     printf ( "Beam is OFF - ioreg.singlehitreadclearreg %x\n", regv);
   }

QuitProgram:

  return 0;
 }
