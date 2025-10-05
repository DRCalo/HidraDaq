
#include <ctime>
#include <cstdio>
#include <cstring>
#include <sys/time.h>

#include "hidraEventHeader.h"

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

void printHeader ( hidraEventHeader heh )
 {
   uint32_t heh_a [ HEH_NW ];

   memcpy ( heh_a, &heh, HEH_SZ );

   for (int i=0; i<HEH_NW; i ++)
    {
     printf ( "%x ", heh_a[i] );
    }
   printf ( "\n" );
 }

void printTrailer ( hidraEventTrailer het )
 {
   printf ( "%x ", het );
   printf ( "\n" );
 }

int main ( int argc, char** argv )
 {
   hidraEventHeader heh;
   hidraEventTrailer het;

   uint32_t sec, usec, nTriggers, nSpills, dataSz, trigMask, pedMask, isPedScalerCounts, evSanity;
   int ret;

   ret = time_now ( &sec, &usec);
   if (ret != 0) goto QuitProgram;

   nTriggers = 0x409;
   nSpills = 0x27;
   dataSz = 0xac;
   trigMask = 3;
   pedMask = 0xb;
   isPedScalerCounts = 1;
   evSanity = 0xf;

   heh.eventMarker = HIDRA_EVENT_MARKER;
   heh.eventNumber = nTriggers;
   heh.spillNumber = nSpills;
   heh.headerSize = HEH_NW;
   heh.trailerSize = HET_NW;
   heh.dataSize = dataSz;
   heh.eventSize = HEH_NW+HET_NW+dataSz;
   heh.eventTimeSecs = sec;
   heh.eventTimeMicroSecs = usec;
   heh.triggerMask = trigMask;
   heh.isPedMask = pedMask;
   heh.isPedFromScaler = isPedScalerCounts;
   heh.sanityFlag = evSanity;
   heh.headerEndMarker = HEADER_END_MARKER;

   het = EVENT_END_MARKER;

   printHeader ( heh );
   printTrailer ( het );

QuitProgram:

   return 0;
 }
