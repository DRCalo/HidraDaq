
#include <csignal>
#include <ctime>
#include <cstdio>
#include <sys/time.h>

#include "myV977.h"
#include "myV560.h"
#include "hidraEventHeader.h"

#define MAXEVENTS 10000
#define TDC_LSB_ps 140

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

#define SEC_OFFSET 1759179710

uint64_t uTime ()
 {
   uint64_t usec(0);
   struct timeval tv;

   int ret = gettimeofday ( &tv, NULL);

   usec = (tv.tv_sec-SEC_OFFSET)*1000000+tv.tv_usec;

   return usec;
 }

int time_now ( uint32_t* sec, uint32_t* usec )
 {
   struct timeval tv;

   int ret = gettimeofday ( &tv, NULL);

   if (ret == 0) {
     *sec = uint32_t(tv.tv_sec);
     *usec = uint32_t(tv.tv_usec);
   }

   return ret;
 }

uint32_t yboard[] = { 0x0a000000, 0x0b000000, 0x0c000000, 0x0d000000, 0x01000000, 0x0f000000 };
uint16_t ygeoadrs[] = { 5, 6, 7, 8, 9, 10 };

uint32_t xboard[] = { 0x09000000, 0x05000000, 0x08000000 };
uint16_t xgeoadrs[] = { 8, 10, 12 };

uint32_t board[] = { 0x0a000000, 0x0b000000, 0x0c000000, 0x05000000, 0x09000000, 0x01000000, 0x08000000 };
uint16_t geoadrs[] = { 5, 6, 7, 8, 9, 10, 11 };

uint32_t wboard[] = { 0x0d000000, 0x0a000000, 0x0b000000, 0x0c000000, 0x05000000, 0x09000000, 0x01000000, 0x08000000, 0x0f000000 }; // v862 0x0e000000 removed
uint16_t wgeoadrs[] = { 4, 5, 6, 7, 8, 9, 10, 11, 12 };

#define NQTP (sizeof(board)/sizeof(uint32_t))
#define NQDC (NQTP-1)
#define TDC_OFFS (NQDC*34)

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
    // printf ("header %x geo %x marker %x crate %x zero %d n chans %x not used %x\n", hd, u.h.g, u.h.m, u.h.c, u.h.z, u.h.n, u.h.u );
    *nch = u.h.n;
 }

void decodetrail ( uint32_t tr, uint32_t* evc )
 {
    union trail_u u;
    u.v = tr;
    // printf ("trail %x geo %x marker %x event counter %d\n", tr, u.t.g, u.t.m, u.t.c );
    *evc = u.t.c;
 }

void decodeqdc ( uint32_t idx, uint32_t dt, bool* valid, uint32_t* chan, uint32_t* val )
 {
    union data_u u;
    u.v = dt;
    u.d.c &= 0x1f;
    u.d.f &= 0x3;
    // printf ("QDC index %d data %x geo %x marker %x chan %x not used %d flags %x val %d\n", idx, dt, u.d.g, u.d.m, u.d.c, u.d.u, u.d.f, u.d.v );
    *valid = (u.d.m == 0) && (u.d.f == 0);
    *chan = u.d.c;
    *val = u.d.v;
 }

void decodetdc ( uint32_t idx, uint32_t dt, bool* valid, uint32_t* chan, uint32_t* val )
 {
    union data_u u;
    u.v = dt;
    u.d.c >>= 1;
    u.d.c &= 0xf;
    // printf ("TDC index %d data %x geo %x marker %x chan %x not used %d flags %x val %d\n", idx, dt, u.d.g, u.d.m, u.d.c, u.d.u, u.d.f, u.d.v );
    *valid = (u.d.m == 0) && (u.d.f == 8);
    *chan = u.d.c;
    *val = u.d.v;
 }

#define MASKPED (1 << 14)
#define MASKT1T2 (1 << 13)

/*
 * list of read operations
 *     scal4spill.readcounter ( chan4we, &nSpills );     // chan4we = 15, base_address = SCALSPILL_ADDR    base_address+0x10+4*chan4we
 *     scal4spill.readcounter ( chan4eoe, &nEoEs );      // chan4eoe = 14, base_address = SCALSPILL_ADDR   base_address+0x10+4*chan4eoe
 *     scal4ped.readcounter ( chan4fast, &nfastgates );  // chan4fast = 0, base_address = SCALPED_ADDR     base_address+0x10+4*chan4fast
 *     scal4ped.readcounter ( chan4phys, &nphysgates );  // chan4phys = 2, base_address = SCALPED_ADDR     base_address+0x10+4*chan4phys
 *     scal4ped.readcounter ( chan4peds, &npedgates );   // chan4peds = 4, base_address = SCALPED_ADDR     base_address+0x10+4*chan4peds
 *     scal4ped.clear();                                 // base_address = SCALPED_ADDR                    base_address+0x50
 *     ioreg.multihitreadclearreg(&regw);                // base_address = IOREG_ADDR                      base_address+0x18
 *     ioreg.singlehitreadclearreg(&regv);               // base_address = IOREG_ADDR                      base_address+0x16
 */

#define SCALSPILL_BASE_ADDR 0x22000000
#define SCALPED_BASE_ADDR 0x11000000
#define IOREG_BASE_ADDR 0x01000000

#define SCAL_CHN_REG 0x10

#define SCAL_CLR_REG 0x50
#define IOR_MLT_RDCLR_REG 0x18
#define IOR_SNGL_RDCLR_REG 0x16

#define WE_CHN 15
#define EoE_CHN 14
#define FASTG_CHN 0
#define PHY_CHN 2
#define PED_CHN 4

#define WE_CHN_ADDRS ( SCALSPILL_BASE_ADDR + SCAL_CHN_REG + 4 * WE_CHN )
#define EoE_CHN_ADDRS ( SCALSPILL_BASE_ADDR + SCAL_CHN_REG + 4 * EoE_CHN )
#define FASTG_CHN_ADDRS ( SCALPED_BASE_ADDR + SCAL_CHN_REG + 4 * FASTG_CHN )
#define PHY_CHN_ADDRS ( SCALPED_BASE_ADDR + SCAL_CHN_REG + 4 * PHY_CHN )
#define PED_CHN_ADDRS ( SCALPED_BASE_ADDR + SCAL_CHN_REG + 4 * PED_CHN )

#define SCAL_CLR_ADDRS ( SCALPED_BASE_ADDR + SCAL_CLR_REG )
#define IOR_MLT_RDCLR_ADDRS ( IOREG_BASE_ADDR + IOR_MLT_RDCLR_REG )
#define IOR_SNGL_RDCLR_ADDRS ( IOREG_BASE_ADDR + IOR_SNGL_RDCLR_REG )

uint32_t address_list[] = { WE_CHN_ADDRS, EoE_CHN_ADDRS, FASTG_CHN_ADDRS, PHY_CHN_ADDRS, PED_CHN_ADDRS, SCAL_CLR_ADDRS, IOR_MLT_RDCLR_ADDRS, IOR_SNGL_RDCLR_ADDRS };

#define NREAD (sizeof(address_list)/sizeof(uint32_t))

CVAddressModifier am_list[NREAD];
CVDataWidth dw_list[NREAD];

void prepareMultiRead ()
 {
   for (int j=0; j<NREAD; j++)
    {
      am_list[j] = cvA32_U_DATA;
      dw_list[j] = cvD16;
    }
 }

int main ( int argc, char** argv )
 {
  hidraEventHeader heh;
  hidraEventTrailer het(EVENT_END_MARKER);

  uint32_t multi_read_data[NREAD];
  CVErrorCodes multi_read_cv[NREAD];

  time_t startT;
  struct tm* info;
  char rawfilename [256];

  uint32_t nTriggers(0), nT1T2(0), nPed(0), nBoth(0), nNone(0), nMultiT1T2(0), nMultiPed(0), nMultiBoth(0), nMultiNone(0), goodPhy(0), goodPed(0), badEvts(0);
  uint32_t nSpills(0), nEoEs(0);
  uint32_t isPedFromMask(0), isPedFromQDC(0), isPedFromTDC(0), isPedFromScaler(0);
  uint32_t pedMatrix[16] = {0};
  uint32_t isPedFromMaskT1T2(0), isPedFromQDCT1T2(0), isPedFromTDCT1T2(0), isPedFromScalerT1T2(0);
  uint32_t pedMatrixT1T2[16] = {0};
  uint32_t isPedFromMaskT1T2bar(0), isPedFromQDCT1T2bar(0), isPedFromTDCT1T2bar(0), isPedFromScalerT1T2bar(0);
  uint32_t pedMatrixT1T2bar[16] = {0};
  uint64_t u0, u1, u2, u3, u4, u5;
  double um1(0), um2(0), um3(0), um4(0);
  bool end_run(false);
  uint32_t runnr, numevts, mask4daq;
  uint16_t datasourcemask, actualmask;
  bool takeboth, valid;
  uint32_t val, nchans, ch;
  uint64_t totTime, uSecPerEv(0);

  runnr = (argc >= 2) ? atoi(argv[1]) : 0;
  numevts = (argc >= 3) ? atoi(argv[2]) : MAXEVENTS;
  mask4daq = (argc >= 4) ? atoi(argv[3]) : 0;

  datasourcemask = (mask4daq & 3) << 13;   // using output set register bit 13 for masking physics and bit 14 for masking pedestal
  printf ( " datasourcemask is %d\n", datasourcemask);
  takeboth = (datasourcemask == 0);
  actualmask = (takeboth) ? MASKPED : datasourcemask;

  // instance of class myCaen for VME access via A4818 (id 49086) + V3718
  myCaen mc ( true );

  // instance of V977 I/O reg
  myV977 ioreg ( &mc, 0x01000000 );
  uint16_t tdclsb;
  double toapedck, toafastg;
  uint16_t trigMask, regv, regw;
  uint32_t nfastgates(0), nphysgates(0), npedgates(0);
  uint32_t totfastgates(0), totphysgates(0), totpedgates(0);
  const uint32_t chan4fast(0), chan4phys(2), chan4peds(4), chan4we(15), chan4eoe(14);

  FILE* rawfile;

  // instance of V560 scaler for ped gate counting event by event
  myV560 scal4ped ( &mc, 0x11000000 );

  // instance of V560 scaler for spill counting
  myV560 scal4spill ( &mc, 0x22000000 );

  printf ( "time since the Epoch: %ld\n", time(NULL));

  // check I/O reg status (no need for this, just for monitoring
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

  // set veto to block triggers
  ioreg.reset();
  ioreg.outputsetreg(uint16_t(0x8000));
  // clear quad NIM scaler
  ioreg.outputsetreg(uint16_t(0x9000));
  ioreg.outputsetreg(uint16_t(0x8000));

  // initialise QTP boards (initialisation funcion in class myCaen)
  for(int i=0; i<NQTP; i++) if ( mc.initQTP ( board[i], geoadrs[i] ) != 0 ) goto QuitProgram;

  // enable CBLT/MCST for all QTP boards at address 0xAA
  for(int i=0; i<NQTP; i++) if ( mc.write_reg ( board[i]+0x1004, 0xAA ) != 0 ) goto QuitProgram;

  // set first QTP as first board to be read out
  if ( mc.write_reg ( board[0]+0x101A, 2 ) != 0 ) goto QuitProgram;
  // set last QTP as last board to be read out
  if ( mc.write_reg ( board[NQTP-1]+0x101A, 1 ) != 0 ) goto QuitProgram;
  // set other QTPs as intermediate boards to be read out
  for(int i=1; i<NQTP-1; i++) if ( mc.write_reg ( board[i]+0x101A, 3 ) != 0 ) goto QuitProgram;

  tdclsb = TDC_LSB_ps;
  mc.setFsr ( board[NQTP-1], tdclsb );

  mc.getLsb ( board[NQTP-1], &tdclsb );

  // set RESET MODE for all the boards`
  // if ( mc.write_reg ( 0xAA001006, 0x80 ) != 0 ) goto QuitProgram;

  // catch signals SIGINT, SIGTERM, SIGUSR1
  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  signal(SIGTERM, sigterm_handler);     // SIGTERM handler
  signal(SIGUSR1, sigusr1_handler);     // SIGUSR1 handler

  printf ( "before while loop\n\n" );

  system ( "date" );
  printf ( "\n" );

  startT = time ( NULL );
  info = localtime ( & startT );

  if (runnr == 0)
   {
     sprintf ( rawfilename, "/home/hidra/TB2025data/RawData-%.4d.%.2d.%.2d-%.2d.%.2d.%.2d.txt", info->tm_year+1900, info->tm_mon, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec );
     printf ( "Rawfile is /home/hidra/TB2025data/RawDaaa-%.4d.%.2d.%.2d-%.2d.%.2d.%.2d.txt", info->tm_year+1900, info->tm_mon, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec );
   }
  else
   {
     sprintf ( rawfilename, "/home/hidra/TB2025data/RawData-%.4d.%.2d.%.2d-%.2d.%.2d.%.2d.run%d.txt", info->tm_year+1900, info->tm_mon, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec, runnr );
     printf ( "Rawfile is /home/hidra/TB2025data/RawDaaa-%.4d.%.2d.%.2d-%.2d.%.2d.%.2d.run%d.txt", info->tm_year+1900, info->tm_mon, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec, runnr );
   }

  rawfile = fopen ( rawfilename, "w" );
  if (rawfile == NULL) {
	  printf ( " can't open file for saving data, going to send data to stdout\n" );
	  rawfile = stdout;
  }

  // reset V560 scaler contents
  scal4ped.clear();
  scal4spill.clear();

  prepareMultiRead();

  // release veto, open quad NIM scaler gate and mask (if requested) data sources
  ioreg.outputsetreg(uint16_t(0x1000 | actualmask));

  totTime = uTime();

  while (1) {
    int howmany;
    bool isT1T2, pedFromMask, pedFromQDC, pedFromTDC, pedFromScaler;
    uint16_t pedTagMask;
    uint32_t sec, usec;
    CVErrorCodes cv;

    u0 = uTime();
    ioreg.singlehitreadreg(&regv);
    end_run = stop_run;
    if (regv & 0x8000)
     {
       time_now ( &sec, &usec);

       nTriggers ++;

       trigMask = regv & 3;

       isT1T2 = (trigMask & 1);
       (trigMask == 1) ? nT1T2 ++ : (trigMask == 2) ? nPed ++ : (trigMask == 3) ? nBoth ++ : nNone ++;
       pedFromMask = (trigMask & 2);

       u1 = uTime();
       um1 += u1 - u0;

       mc.read_MBLT ( 0xAA000000, buffer, &howmany );

       u2 = uTime();
       um2 += u2 - u1;

       howmany /= sizeof(uint32_t);

       /*
	*
       cv = mc.multiRead( address_list, multi_read_data, NREAD, am_list, dw_list, multi_read_cv );

       if (cv != cvSuccess) printf ( " error from multiRead %d\n", cv );
        */

       scal4spill.readcounter ( chan4we, &nSpills );
       scal4spill.readcounter ( chan4eoe, &nEoEs );

       scal4ped.readcounter ( chan4fast, &nfastgates );
       scal4ped.readcounter ( chan4phys, &nphysgates );
       scal4ped.readcounter ( chan4peds, &npedgates );

       scal4ped.clear();

       ioreg.multihitreadclearreg(&regw);
       ioreg.singlehitreadclearreg(&regv);

       nSpills = multi_read_data[0];
       nEoEs = multi_read_data[1];
       nfastgates = multi_read_data[2];
       nphysgates = multi_read_data[3];
       npedgates = multi_read_data[4];

       regw = multi_read_data[6];
       regv = multi_read_data[7];

       pedFromScaler = (npedgates > 0);

       if (nTriggers == numevts) stop_run = true;

       u3 = uTime();
       um3 += u3-u2;

       if (stop_run) ioreg.outputsetreg(uint16_t(0x8000));

       else if (takeboth) {
	 if ( 10*nPed < nT1T2 ) {
		 if (actualmask != MASKT1T2 ) {
			 actualmask = MASKT1T2;
			 // ioreg.outputsetreg(uint16_t(0x1000 | actualmask));
			 ioreg.outputsetreg(uint16_t(0x1000));
		 }
	 } else {
		 if (actualmask != MASKPED ) {
			 actualmask = MASKPED;
			 ioreg.outputsetreg(uint16_t(0x1000 | actualmask));
		 }
	 }
       }

       u4 = uTime();
       um4 += u4-u3;

       printf(" nTriggers %d nSpills  %d trigMask %d\n", nTriggers, nSpills, trigMask );

       if (regw) (trigMask == 1) ? nMultiT1T2 ++ : (trigMask == 2) ? nMultiPed ++ : (trigMask == 3) ? nMultiBoth ++ : nMultiNone ++;
       if (regw) printf(" regv %x , regw %x \n", regv, regw);
       printf ( "regv %x, trigMask %d, npedgates %d\n", regv, trigMask, npedgates );

       decodehead ( buffer[0], &nchans );
       decodeqdc ( 4, buffer[1+15], &valid, &ch, &val );    // QDC # 4 ch 15 -> end marker ped clock
       if (valid) pedFromQDC = (val > 1000);

       toapedck = toafastg = ch = 0;
       decodehead ( buffer[TDC_OFFS], &nchans );
       if (nchans) 
        {
          for (int j=0; j<nchans; j ++)
	   {
	     uint32_t xt;
	     decodetdc ( NQDC, buffer[TDC_OFFS+j], &valid, &ch, &xt );
	     if (ch == 8)
	      {
	        if (valid) toapedck = double(xt * tdclsb) / 1000;
                printf ( " nchans %d valid %d ch %d val %d ToA ped ck EM %f ns\n", nchans, valid, ch, xt, toapedck );
	      }
	     else if (ch == 15)
	      {
	        if (valid) toafastg = double(xt * tdclsb) / 1000;
                printf ( " nchans %d valid %d ch %d val %d ToA fast gate EM %f ns\n", nchans, valid, ch, xt, toafastg );
	      }
	   }
	}

       pedFromTDC = (ch == 8) && (toapedck > 10);

       // pedFromMask   1
       // pedFromScaler 2
       // pedFromTDC    4
       // pedFromQDC    8

       pedTagMask = 0;
       if (pedFromMask)
        {
	  isPedFromMask ++;
	  (isT1T2) ?  isPedFromMaskT1T2 ++ : isPedFromMaskT1T2bar ++;
	  pedTagMask |= 0x1;
        }
       if (pedFromQDC)
        {
	  isPedFromQDC ++;
	  (isT1T2) ?  isPedFromQDCT1T2 ++ : isPedFromQDCT1T2bar ++;
	  pedTagMask |= 0x8;
        }
       if (pedFromTDC)
        {
	  isPedFromTDC ++;
	  (isT1T2) ?  isPedFromTDCT1T2 ++ : isPedFromTDCT1T2bar ++;
	  pedTagMask |= 0x4;
        }
       if (pedFromScaler)
        {
	  isPedFromScaler ++;
	  (isT1T2) ?  isPedFromScalerT1T2 ++ : isPedFromScalerT1T2bar ++;
	  pedTagMask |= 0x2;
        }

       pedMatrix [ pedTagMask ] ++;
       (isT1T2) ? pedMatrixT1T2 [ pedTagMask ] ++ : pedMatrixT1T2bar [ pedTagMask ] ++;

       totfastgates += nfastgates;
       totphysgates += nphysgates;
       totpedgates += npedgates;

       heh.eventMarker = HIDRA_EVENT_MARKER;
       heh.eventNumber = nTriggers;
       heh.spillNumber = nSpills;
       heh.headerSize = HEH_NW;
       heh.trailerSize = HET_NW;
       heh.dataSize = uint32_t(howmany);
       heh.eventSize = HEH_NW+HET_NW+heh.dataSize;
       heh.eventTimeSecs = sec;
       heh.eventTimeMicroSecs = usec;
       heh.triggerMask = trigMask;
       heh.isPedMask = pedTagMask;
       heh.isPedFromScaler = npedgates;
       {
	 bool okPhys, okPed;
         okPhys = ( trigMask == 1 ) && ( pedTagMask == 0 );
         okPed = ( trigMask == 2 ) && ( pedTagMask == 0xf );
         heh.sanityFlag = ((okPhys) ? 1 : 0) + ((okPed) ? 2 : 0);
	 switch (heh.sanityFlag) {
	       case 0:
		 badEvts ++;
	 	 break;
	       case 1:
		 goodPhy ++;
	 	 break;
	       case 2:
		 goodPed ++;
	 	 break;
	       default:
	 	 break;
		
         }
       }
       heh.headerEndMarker = HEADER_END_MARKER;

       printf ( "\nspill %d total events %d T1T2 %d pedestal %d both %d\n\n", nSpills, nTriggers, nT1T2, nPed, nBoth );

       {
	 uint32_t heh_a [ HEH_NW ];
	 memcpy ( heh_a, &heh, HEH_SZ );
	 for (int i=0; i<HEH_NW; i ++) fprintf ( rawfile, "%x ", heh_a[i] );
       }

       for (int i=0; i<howmany; i++) fprintf ( rawfile, "%x ", buffer[i] );
       fprintf ( rawfile, "%x\n", het );
       fflush ( rawfile );


       printf ( "\n\n" );
       fflush ( stdout );

       loopcounter ++;
     }
    if (end_run) break;
  }
  ioreg.outputsetreg(uint16_t(0x0));
  totTime = uTime() - totTime;

  if ((rawfile != NULL) && (rawfile != stdout)) fclose ( rawfile );

  if (nTriggers) uSecPerEv = totTime / nTriggers;

  totTime /= 1000000;
  printf ( " Total time %ld sec, time per event %ld microsec\n", totTime, uSecPerEv );

  printf ( "after while loop ... \n counter %d Triggers %d nT1T2 %d nPed %d nBoth %d nNone %d nMultiT1T2 %d nMultiPed %d nMultiBoth %d nMultiNone %d nT1T2+Both %d nPed+nBoth %d nT1T2+nPed+nBoth %d\n",
		  loopcounter, nTriggers, nT1T2, nPed, nBoth, nNone, nMultiT1T2, nMultiPed, nMultiBoth, nMultiNone, nT1T2+nBoth, nPed+nBoth, nT1T2+nPed+nBoth );
  printf ( " Good for physics %d Good for pedestal %d Bad %d Total %d\n", goodPhy, goodPed, badEvts, goodPhy+goodPed+badEvts );
  printf ( "totfastgates %d totphysgates %d totpedgates %d\n", totfastgates, totphysgates, totpedgates );
  printf ( "ioreg.singlehitreadreg %x\n", regv);

  printf ( " Pedestal gate counters          : TriggerMask %d QDC %d TDC %d Scaler %d\n", isPedFromMask, isPedFromQDC, isPedFromTDC, isPedFromScaler );
  printf ( " Pedestal gate counters (T1T2)   : TriggerMask %d QDC %d TDC %d Scaler %d\n", isPedFromMaskT1T2, isPedFromQDCT1T2, isPedFromTDCT1T2, isPedFromScalerT1T2 );
  printf ( " Pedestal gate counters (T1T2bar): TriggerMask %d QDC %d TDC %d Scaler %d\n", isPedFromMaskT1T2bar, isPedFromQDCT1T2bar, isPedFromTDCT1T2bar, isPedFromScalerT1T2bar );
  printf ( "\n\n Matrix for ped gates (all):\n" );
  for (int i=0; i<16; i++) printf ( " [%d] %d ", i, pedMatrix[i] );
  printf ( "\n" );
  printf ( "\n\n Matrix for ped gates (T1T2):\n" );
  for (int i=0; i<16; i++) printf ( " [%d] %d ", i, pedMatrixT1T2[i] );
  printf ( "\n" );
  printf ( "\n\n Matrix for ped gates (T1T2bar):\n" );
  for (int i=0; i<16; i++) printf ( " [%d] %d ", i, pedMatrixT1T2bar[i] );
  printf ( "\n" );

  uint32_t nevts[NQTP];
  printf ( " Events in " );
  for(int i=0; i<NQTP; i++)
   { 
     uint16_t lo, hi;
     if ( mc.read_reg ( board[i]+0x1024, &lo ) != 0 ) goto QuitProgram;
     if ( mc.read_reg ( board[i]+0x1026, &hi ) != 0 ) goto QuitProgram;
     nevts[i] = (hi << 16) | lo;
     printf ( "QTP[%d] %d ", i, nevts[i] );
   }
  printf ( "\n" );
  um1 /= nTriggers;
  um2 /= nTriggers;
  um3 /= nTriggers;
  um4 /= nTriggers;

  printf ( "average time for ioreg.singlehitreadreg %lf - ", um1 );
  printf ( "average time for mc.read_MBLT %lf - ", um2 );
  printf ( "average time for CAENVME_MultiRead %lf\n\n", um3 );
  printf ( "average time for ioreg.singlehitreadclearreg %lf\n\n", um4 );
  system ( "date" );
  printf ( "\n" );

QuitProgram:

  return 0;
 }
