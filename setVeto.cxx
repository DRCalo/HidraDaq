
#include <csignal>
#include <ctime>
#include <cstdio>
#include <sys/time.h>

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
  // instance of class myCaen for VME access via A4818 (id 49086) + V3718
  myCaen mc;

  // instance of I/O reg V977
  myV977 ioreg ( &mc, 0x01000000 );

  uint16_t outr;

  if (argc != 2) return -1;

  ioreg.outputsetreg(&outr);
  printf ( "ioreg.outputsetreg %x\n", outr);

  if (strcasecmp(argv[1] , "SETPHYV") == 0)
	  outr |= 1<<13;
  else if (strcasecmp(argv[1] , "CLRPHYV") == 0)
	  outr &= ~(1<<13);
  else if (strcasecmp(argv[1] , "SETPEDV") == 0)
	  outr |= 1<<14;
  else if (strcasecmp(argv[1] , "CLRPEDV") == 0)
	  outr &= ~(1<<14);
  else
     return -2;

  printf ( "ioreg.outputsetreg %x\n", outr);
  ioreg.outputsetreg(outr);

  ioreg.outputsetreg(&outr);
  printf ( "ioreg.outputsetreg %x\n", outr);

  return 0;
 }
