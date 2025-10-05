
#include <csignal>
#include <ctime>
#include <cstdio>
#include <sys/time.h>

#include "myV977.h"
#include "myV560.h"

#define MAXEVENTS 10000

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

uint32_t board ( 0x0f000000 );

uint32_t buffer[MAX_BLT_SIZE/sizeof(uint32_t)];

struct head_s
 {
    unsigned u : 8;
    unsigned n : 6;
    unsigned z : 2;
    unsigned c : 8;
    unsigned m : 3;
    unsigned g : 5;
 };

union head_u
 {
    uint32_t v;
    struct head_s h;
 };

struct data_s
 {
    unsigned v : 12;
    unsigned f : 3;
    unsigned u : 1;
    unsigned c : 8;
    unsigned m : 3;
    unsigned g : 5;
 };

union data_u
 {
    uint32_t v;
    struct data_s d;
 };

struct trail_s
 {
    unsigned c : 24;
    unsigned m : 3;
    unsigned g : 5;
 };

union trail_u
 {
    uint32_t v;
    struct trail_s t;
 };

void decodehead ( uint32_t hd, uint32_t* nch )
 {
    union head_u u;
    u.v = hd;
    printf ("header %x geo %x marker %x crate %x zero %d n chans %x not used %x\n", hd, u.h.g, u.h.m, u.h.c, u.h.z, u.h.n, u.h.u );
    *nch = u.h.n;
 }

void decodetrail ( uint32_t tr, uint32_t* evc )
 {
    union trail_u u;
    u.v = tr;
    printf ("trail %x geo %x marker %x event counter %d\n", tr, u.t.g, u.t.m, u.t.c );
    *evc = u.t.c;
 }

void decodeqdc ( uint32_t idx, uint32_t dt, bool* valid, uint32_t* chan, uint32_t* val )
 {
    union data_u u;
    u.v = dt;
    u.d.c &= 0x1f;
    u.d.f &= 0x3;
    printf ("QDC index %d data %x geo %x marker %x chan %x not used %d flags %x val %d\n", idx, dt, u.d.g, u.d.m, u.d.c, u.d.u, u.d.f, u.d.v );
    *valid = (u.d.m == 0) && (u.d.f == 0);
    *chan = u.d.c,
    *val = u.d.v;
 }

void decodetdc ( uint32_t idx, uint32_t dt, bool* valid, uint32_t* chan, uint32_t* val )
 {
    union data_u u;
    u.v = dt;
    u.d.c >>= 1;
    u.d.c &= 0xf;
    printf ("TDC index %d data %x geo %x marker %x chan %x not used %d flags %x val %d\n", idx, dt, u.d.g, u.d.m, u.d.c & 0x1f, u.d.u, u.d.f, u.d.v );
    *valid = (u.d.m == 0) && (u.d.f == 0);
    *chan = u.d.c,
    *val = u.d.v;
 }

int main ( int argc, char** argv )
 {
  // instance of class myCaen for VME access via A4818 (id 49086) + V3718
  myCaen mc;

  if ( mc.initQTP ( board, 13 ) != 0 ) goto QuitProgram;

  // catch signals SIGINT, SIGTERM, SIGUSR1
  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  signal(SIGTERM, sigterm_handler);     // SIGTERM handler
  signal(SIGUSR1, sigusr1_handler);     // SIGUSR1 handler

QuitProgram:

  return 0;
 }
