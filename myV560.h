#ifndef _MY_V560_H_
#define _MY_V560_H_

/*****************************************

  myV560.h
  --------

  Definition of the CAEN V560 IO Register

*****************************************/

#include "myCaen.h"

class myV560 {
  public:
    myV560 ( myCaen* mc, uint32_t ba ) : m_Caen(mc), m_ba(ba) {}
    ~myV560() {}

    CVErrorCodes clear ()
      { uint16_t dummy; CVErrorCodes ret = m_Caen->read_reg ( m_ba+0x50, &dummy ); return ret; }

    CVErrorCodes xclear ( uint16_t* dummy )
      { CVErrorCodes ret = m_Caen->read_reg ( m_ba+0x50, dummy ); return ret; }

    CVErrorCodes vetoset ()
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba+0x52, 0 ); return ret; }
    
    CVErrorCodes vetoclear ()
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba+0x54, 0 ); return ret; }
    
    CVErrorCodes scaleincr ()
      { CVErrorCodes ret = m_Caen->write_reg ( m_ba+0x56, 0 ); return ret; }
    
    CVErrorCodes readcounter ( uint32_t cnt, uint32_t* val )
      { CVErrorCodes ret = m_Caen->read_data ( m_ba+0x10+4*cnt, val ); return ret; }

  private:
    myCaen* m_Caen;
    uint32_t m_ba;

};

#endif // _MY_V560_H_
