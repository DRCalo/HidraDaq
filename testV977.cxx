
#include "myV977.h"

int main ( int argc, char** argv )
 {
  myCaen mc;
  myV977 ioreg ( &mc, 0x01000000 );
  uint16_t regv;

  ioreg.reset();

  ioreg.inputsetreg(0x11);
  ioreg.inputsetreg(&regv);
  printf ( "ioreg.inputsetreg %x\n", regv);

  ioreg.inputmaskreg(0x42);
  ioreg.inputmaskreg(&regv);
  printf ( "ioreg.inputmaskreg %x\n", regv);

  ioreg.inputreadreg(&regv);
  printf ( "ioreg.inputreadreg %x\n", regv);

  ioreg.singlehitreadreg(&regv);
  printf ( "ioreg.singlehitreadreg %x\n", regv);

  ioreg.multihitreadreg(&regv);
  printf ( "ioreg.multihitreadreg %x\n", regv);

  ioreg.outputsetreg(uint16_t(0));
  ioreg.outputsetreg(&regv);
  printf ( "ioreg.outputsetreg %x\n", regv);


  ioreg.outputmaskreg(uint16_t(0x23));
  ioreg.outputmaskreg(&regv);
  printf ( "ioreg.outputmaskreg %x\n", regv);

  ioreg.interruptmaskreg(uint16_t(0x73));
  ioreg.interruptmaskreg(&regv);
  printf ( "ioreg.interruptmaskreg %x\n", regv);

  ioreg.outputclearreg(uint16_t(0x80));

  ioreg.singlehitreadclearreg(&regv);
  printf ( "ioreg.singlehitreadclearreg %x\n", regv);

  ioreg.multihitreadclearreg(&regv);
  printf ( "ioreg.multihitreadclearreg %x\n", regv);

  ioreg.testcontrolreg(uint16_t(0x0f));
  ioreg.testcontrolreg(&regv);
  printf ( "ioreg.testcontrolreg %x\n", regv);

  return 0;
 }
