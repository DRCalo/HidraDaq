
#include "myV560.h"

int main ( int argc, char** argv )
 {
  myCaen mc;
  // myV560 scal ( &mc, 0x11000000 );    // first scaler in slot 14
  myV560 scal ( &mc, 0x22000000 );    // second scaler in slot 16

  scal.clear();

  for (uint32_t i=0; i<16; i ++)
   {
      uint32_t v;
      scal.readcounter ( i, &v );
      printf ( "scal.inputsetreg cnt %d val %d\n", i, v);
   }

  scal.scaleincr();
  for (uint32_t i=0; i<16; i ++)
   {
      uint32_t v;
      scal.readcounter ( i, &v );
      printf ( "scal.inputsetreg cnt %d val %d\n", i, v);
   }

  return 0;
 }
