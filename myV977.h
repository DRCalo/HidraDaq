#ifndef _MY_V977_H_
#define _MY_V977_H_

/*****************************************

  myV977.h
  --------

  Definition of the CAEN V977 IO Register

*****************************************/

#include "myCaen.h"

class myV977 {
  public:
    myV977 ( myCaen* mc, uint32_t ba ) : m_Caen(mc), m_ba(ba) {}
    ~myV977() {}

    CVErrorCodes reset ()
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba+0x2E, 0 ); return ret; }

    CVErrorCodes inputsetreg ( uint16_t val )
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba, val ); return ret; }
    CVErrorCodes inputsetreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba, val ); return ret; }
    
    CVErrorCodes inputmaskreg ( uint16_t val )
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba+2, val ); return ret; }
    CVErrorCodes inputmaskreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+2, val ); return ret; }

    CVErrorCodes inputreadreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+4, val ); return ret; }

    CVErrorCodes singlehitreadreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+6, val ); return ret; }

    CVErrorCodes multihitreadreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+8, val ); return ret; }

    CVErrorCodes outputsetreg ( uint16_t val )
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba+0xA, val ); return ret; }
    CVErrorCodes outputsetreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+0xA, val ); return ret; }

    CVErrorCodes outputmaskreg ( uint16_t val )
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba+0xC, val ); return ret; }
    CVErrorCodes outputmaskreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+0xC, val ); return ret; }

    CVErrorCodes interruptmaskreg ( uint16_t val )
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba+0xE, val ); return ret; }
    CVErrorCodes interruptmaskreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+0xE, val ); return ret; }

    CVErrorCodes outputclearreg ( uint16_t val )
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba+0x10, val ); return ret; }

    CVErrorCodes singlehitreadclearreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+0x16, val ); return ret; }

    CVErrorCodes multihitreadclearreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+0x18, val ); return ret; }

    CVErrorCodes testcontrolreg ( uint16_t val )
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba+0x1A, val ); return ret; }
    CVErrorCodes testcontrolreg ( uint16_t* val )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+0x1A, val ); return ret; }


  private:
    myCaen* m_Caen;
    uint32_t m_ba;

};

#endif // _MY_V977_H_
