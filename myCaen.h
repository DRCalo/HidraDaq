#ifndef _MY_CAEN_H_
#define _MY_CAEN_H_

/*****************************************

  myCaen.h
  --------

  Definition of the myCaen class

*****************************************/

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <inttypes.h>

#include <CAENVMElib.h>
#include <CAENVMEtypes.h>

#define MAX_BLT_SIZE (256*1024)
#define psRESOL 140
#define IPED 255
#define CommonStart true


class myCaen {
  public:
    myCaen();
    myCaen ( bool isPCIe );
    ~myCaen();

    CVErrorCodes read_reg ( uint32_t reg_addr, uint16_t* val )
      { return CAENVME_ReadCycle ( m_handle, reg_addr, val, cvA32_U_DATA, cvD16 ); }

    CVErrorCodes write_reg ( uint32_t reg_addr, uint16_t val )
      { return CAENVME_WriteCycle ( m_handle, reg_addr, &val, cvA32_U_DATA, cvD16 ); }

    CVErrorCodes read_data ( uint32_t addr, uint32_t* val )
      { return CAENVME_ReadCycle ( m_handle, addr, val, cvA32_U_DATA, cvD32 ); }

    CVErrorCodes write_data ( uint32_t addr, uint32_t val )
      { return CAENVME_WriteCycle ( m_handle, addr, &val, cvA32_U_DATA, cvD32 ); }

    void findModelVersion ( uint16_t model, uint16_t vers, char* modelVersion, int* ch );

    CVErrorCodes initQTP ( uint32_t base_addr, uint16_t geo_addr );

    CVErrorCodes setIped ( uint32_t base_addr, uint16_t Iped );

    CVErrorCodes getIped ( uint32_t base_addr, uint16_t* Iped );

    CVErrorCodes setFsr ( uint32_t base_addr, uint16_t lsb );

    CVErrorCodes getFsr ( uint32_t base_addr, uint16_t* fsr );

    CVErrorCodes getLsb ( uint32_t base_addr, uint16_t* lsb );

    CVErrorCodes read_MBLT ( uint32_t addr,  void* buffer, int* howmany )
      {
        // return CAENVME_FIFOBLTReadCycle ( m_handle, addr,  buffer, MAX_BLT_SIZE, cvA32_U_BLT, cvD32, howmany );
	// average time for ioreg.singlehitreadreg 316.279900 - average time for mc.read_MBLT 321.858400 - average time for ioreg.singlehitreadclearreg 1332.749600
        // return CAENVME_FIFOBLTReadCycle ( m_handle, addr,  buffer, MAX_BLT_SIZE, cvA32_U_BLT, cvD64, howmany );
	// average time for ioreg.singlehitreadreg 314.300700 - average time for mc.read_MBLT 321.185300 - average time for ioreg.singlehitreadclearreg 1390.936200
        return CAENVME_FIFOMBLTReadCycle ( m_handle, addr,  buffer, MAX_BLT_SIZE, cvA32_U_MBLT, howmany );
	// average time for ioreg.singlehitreadreg 314.378400 - average time for mc.read_MBLT 321.532500 - average time for ioreg.singlehitreadclearreg 1320.603100
        // return CAENVME_MBLTReadCycle ( m_handle, addr,  buffer, MAX_BLT_SIZE, cvA32_U_MBLT, howmany );
	// average time for ioreg.singlehitreadreg 315.257200 - average time for mc.read_MBLT 321.791300 - average time for ioreg.singlehitreadclearreg 1325.440500
        // return CAENVME_BLTReadCycle ( m_handle, addr,  buffer, MAX_BLT_SIZE, cvA32_U_BLT, cvD32, howmany );
	// average time for ioreg.singlehitreadreg 314.643900 - average time for mc.read_MBLT 321.203300 - average time for ioreg.singlehitreadclearreg 1320.942900
        // return CAENVME_BLTReadCycle ( m_handle, addr,  buffer, MAX_BLT_SIZE, cvA32_U_BLT, cvD64, howmany );
	// average time for ioreg.singlehitreadreg 314.060200 - average time for mc.read_MBLT 320.362500 - average time for ioreg.singlehitreadclearreg 1365.706700
      }

    CVErrorCodes multiRead ( uint32_t* address_list, uint32_t* multi_read_data, uint32_t readNbr, CVAddressModifier* am_list, CVDataWidth* dw_list, CVErrorCodes* multi_read_cv )
      {
        return CAENVME_MultiRead ( m_handle, address_list, multi_read_data, readNbr, am_list, dw_list, multi_read_cv );
      }

  private:
    int32_t m_handle;
    uint16_t m_xdc_nr;

};

myCaen::myCaen () : m_xdc_nr(0)
 {
   CVBoardTypes ctype ( cvUSB_A4818 );
   uint32_t pid ( 49086 );
   int bdnum ( 0 );

   CVErrorCodes ret = CAENVME_Init2 ( ctype, &pid, bdnum, &m_handle );
   if ( ret != cvSuccess )
    {
      printf("X Can't open VME controller\n");
      exit(-1);
    }
   printf("X Connection is on - handle %d\n", m_handle);
 }

myCaen::myCaen ( bool isPCIe ) : m_xdc_nr(0)
 {
   if ( isPCIe ) {
   CVBoardTypes ctype ( cvPCIE_A5818_V3718 );
   // uint32_t pid ( 19853 );
   uint32_t pid ( 0 );
   int bdnum ( 0 );

   CVErrorCodes ret = CAENVME_Init2 ( ctype, &pid, bdnum, &m_handle );
   if ( ret != cvSuccess )
    {
      printf("X Can't open VME controller\n");
      exit(-1);
    }
   printf("X Connection is on - handle %d\n", m_handle);
   }
 }

myCaen::~myCaen ()
 {
   if (m_handle >= 0) CAENVME_End(m_handle);
 }


void myCaen::findModelVersion ( uint16_t model, uint16_t vers, char* modelVersion, int* ch )
 {
   switch (model) {
   case 792:
        switch (vers) {
        case 0x11:
            strcpy(modelVersion, "AA");
            *ch = 32;
            break;
        case 0x13:
            strcpy(modelVersion, "AC");
            *ch = 32;
            break;
        case 0xE1:
            strcpy(modelVersion, "NA");
            *ch = 16;
            break;
        case 0xE3:
            strcpy(modelVersion, "NC");
            *ch = 16;
            break;
        default:
            strcpy(modelVersion, "-");
            *ch = 32;
            break;
        }
        break;
   case 965:
        switch (vers) {
        case 0x1E:
            strcpy(modelVersion, "A");
            *ch = 16;
            break;
        case 0xE3:
        case 0xE1:
            strcpy(modelVersion, " ");
            *ch = 32;
            break;
        default:
            strcpy(modelVersion, "-");
            *ch = 32;
            break;
        }
        break;
   case 775:
        switch (vers) {
        case 0x11:
            strcpy(modelVersion, "AA");
            *ch = 32;
            break;
        case 0x13:
            strcpy(modelVersion, "AC");
            *ch = 32;
            break;
        case 0xE1:
            strcpy(modelVersion, "NA");
            *ch = 16;
            break;
        case 0xE3:
            strcpy(modelVersion, "NC");
            *ch = 16;
            break;
        default:
            strcpy(modelVersion, "-");
            *ch = 32;
            break;
        }
        break;
   case 785:
        switch (vers) {
        case 0x11:
            strcpy(modelVersion, "AA");
            *ch = 32;
            break;
        case 0x12:
            strcpy(modelVersion, "Ab");
            *ch = 32;
            break;
        case 0x13:
            strcpy(modelVersion, "AC");
            *ch = 32;
            break;
        case 0x14:
            strcpy(modelVersion, "AD");
            *ch = 32;
            break;
        case 0x15:
            strcpy(modelVersion, "AE");
            *ch = 32;
            break;
        case 0x16:
            strcpy(modelVersion, "AF");
            *ch = 32;
            break;
        case 0x17:
            strcpy(modelVersion, "AG");
            *ch = 32;
            break;
        case 0x18:
            strcpy(modelVersion, "AH");
            *ch = 32;
            break;
        case 0x1B:
            strcpy(modelVersion, "AK");
            *ch = 32;
            break;
        case 0xE1:
            strcpy(modelVersion, "NA");
            *ch = 16;
            break;
        case 0xE2:
            strcpy(modelVersion, "NB");
            *ch = 16;
            break;
        case 0xE3:
            strcpy(modelVersion, "NC");
            *ch = 16;
            break;
        case 0xE4:
            strcpy(modelVersion, "ND");
            *ch = 16;
            break;
        default:
            strcpy(modelVersion, "-");
            *ch = 32;
            break;
        }
        break;
   case 862:
        switch (vers) {
        case 0x11:
            strcpy(modelVersion, "AA");
            *ch = 32;
            break;
        case 0x13:
            strcpy(modelVersion, "AC");
            *ch = 32;
            break;
        default:
            strcpy(modelVersion, "-");
            *ch = 32;
            break;
        }
        break;
   }
   printf("Model = %d vers %d version %s nch %d\n", model, vers, modelVersion, *ch);
 }

CVErrorCodes myCaen::initQTP ( uint32_t base_addr, uint16_t geo_addr )
 {
   CVErrorCodes ret;
   int brd_nch(32);
   uint16_t QTP_LLD[32] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   char modelVersion[3];
   uint16_t fwrev, vers, mod0, mod1, model;
   uint16_t sn0, sn1, sn2, sn3;
   uint32_t sernum;
   uint16_t Iped ( 255 );                    // pedestal of the QDC (or resolution of the TDC)
   int EnableSuppression ( 0 );              // disable Zero and Overflow suppression if QTP boards
   uint16_t cntrl_reg, gaddr, adc_type, adc_info, crate_nr;

   // ************************************************************************
   // QTP settings
   // ************************************************************************

   // Reset QTP board
   ret = this->write_reg ( base_addr+0x1016, 0 );
   if (ret != cvSuccess) return ret;

   // Read FW revision
   ret = this->read_reg ( base_addr+0x1000, &fwrev);
   if (ret != cvSuccess) return ret;

   // Read model
   ret = this->read_reg ( base_addr+0x803E, &mod0);
   if (ret != cvSuccess) return ret;

   ret = this->read_reg ( base_addr+0x803A, &mod1);
   if (ret != cvSuccess) return ret;

   model = (mod0 & 0xFF) + ((mod1 & 0xFF) << 8);

   // Read version (> 0xE0 = 16 channels)
   ret = this->read_reg ( base_addr+0x8032, &vers );
   if (ret != cvSuccess) return ret;
   vers &= 0xFF;

   findModelVersion(model, vers, modelVersion, &brd_nch);
   printf("\nBase Address 0x%X Model = V%d%s\n", base_addr, model, modelVersion);

   // Read serial number
   ret = this->read_reg ( base_addr+0x8F06, &sn0);
   if (ret != cvSuccess) return ret;

   ret = this->read_reg ( base_addr+0x8F02, &sn1);
   if (ret != cvSuccess) return ret;

   sernum = (sn0 & 0xFF) + ((sn1 & 0xFF) << 8);
   if (sernum == UINT32_C(0xFFFF)) {
   	ret = this->read_reg ( base_addr+0x8EF6, &sn0);
   	if (ret != cvSuccess) return ret;

   	ret = this->read_reg ( base_addr+0x8EF4, &sn1);
   	if (ret != cvSuccess) return ret;

   	ret = this->read_reg ( base_addr+0x8EF2, &sn2);
   	if (ret != cvSuccess) return ret;

   	ret = this->read_reg ( base_addr+0x8EF0, &sn3);
   	if (ret != cvSuccess) return ret;

   	sernum = (sn0 & 0xFF) + ((sn1 & 0xFF) << 8) + ((sn2 & 0xFF) << 16) + ((sn3 & 0xFF) << 24);
   }
   printf("Serial Number = %d\n", sernum);

   printf("FW Revision = %d.%d\n", (fwrev >> 8) & 0xFF, fwrev & 0xFF);

   ret = this->read_reg ( base_addr+0x1062, &gaddr );  // Get geo address
   if (ret != cvSuccess) return ret;
   gaddr &= 0x1f;
   printf("BEFORE - Geo adddress for base address 0x%X is %d\n", base_addr, gaddr);
   ret = this->write_reg ( base_addr+0x1062, geo_addr );  // Set geo address

   ret = this->read_reg ( base_addr+0x1062, &gaddr );  // Get geo address
   if (ret != cvSuccess) return ret;
   gaddr &= 0x1f;
   printf("AFTER - Geo adddress for base address 0x%X is %d\n", base_addr, gaddr);

   adc_type = 0;
   if (brd_nch == 32) {
	   adc_type |= 8;
   } else if (brd_nch == 16) {
	   adc_type |= 4;
   }
   if ((model == 792) || (model == 862)) {
	   adc_type |= 2;
   } else if (model == 775) {
	   adc_type |= 1;
   }
   ret = this->read_reg ( base_addr+0x103C, &crate_nr );  // Get crate number
   if (ret != cvSuccess) return ret;
   crate_nr &= 0xff;
   adc_info = (adc_type << 4) | m_xdc_nr;
   printf("BEFORE - Crate number for base address 0x%X is %d\n", base_addr, crate_nr);
   ret = this->write_reg ( base_addr+0x103C, adc_info );  // Set crate number
   if (ret != cvSuccess) return ret;
   ret = this->read_reg ( base_addr+0x103C, &crate_nr );  // Get crate number
   if (ret != cvSuccess) return ret;
   crate_nr &= 0xff;
   printf("AFTER - Crate number for base address 0x%X is 0x%X (adc_tpe 0x%X, xDC nr %d)\n", base_addr, crate_nr, adc_type, m_xdc_nr);

   m_xdc_nr ++;

   if ((model == 792) || (model == 862)) {
      uint16_t ip(IPED);
      ret = this->setIped ( base_addr, ip );
      if (ret != cvSuccess) return ret;
   } else if (model == 775) {
      uint16_t lsb = psRESOL;
      ret = this->setFsr ( base_addr, lsb );
      if (ret != cvSuccess) return ret;
   }

   ret = this->write_reg ( base_addr+0x1010, 0x60 );  // Enable BERR to close BLT at and of block
   if (ret != cvSuccess) return ret;

   // Set LLD (low level threshold for ADC data)
   ret = this->write_reg ( base_addr+0x1034, 0x100 );  // Set threshold step = 16
   if (ret != cvSuccess) return ret;

   for(int i=0; i<brd_nch; i++) {
        if (brd_nch == 16) {
		this->write_reg ( base_addr+0x1080 + i*4, QTP_LLD[i]/16);
		if (ret != cvSuccess) return ret;
	} else {
		this->write_reg ( base_addr+0x1080 + i*2, QTP_LLD[i]/16);
		if (ret != cvSuccess) return ret;
	}
   }

   if (!EnableSuppression) {
        ret = this->write_reg ( base_addr+0x1032, 0x0010 );  // Disable zero suppression
        if (ret != cvSuccess) return ret;
        ret = this->write_reg ( base_addr+0x1032, 0x0008 );  // Disable overrange suppression
        if (ret != cvSuccess) return ret;
        ret = this->write_reg ( base_addr+0x1032, 0x1000 );  // Enable empty events
        if (ret != cvSuccess) return ret;
   }

   ret = this->read_reg ( base_addr+0x1032, &cntrl_reg );
   if (ret != cvSuccess) return ret;

   printf("Ctrl Reg = %04X\n", cntrl_reg);
   printf("QTP board programmed\n");

   return cvSuccess;
 }

CVErrorCodes myCaen::setIped ( uint32_t base_addr, uint16_t Iped )
 {
   CVErrorCodes ret;
   printf("Setting Iped current to %d\n",Iped);
   ret = this->write_reg ( base_addr+0x1060, Iped );  // Set pedestal current
   if (ret != cvSuccess) return ret;
   ret = this->read_reg ( base_addr+0x1060, &Iped );  // Get pedestal current
   if (ret != cvSuccess) return ret;
   printf("Read back Iped current val %d\n", Iped);
   return cvSuccess;
 }

CVErrorCodes myCaen::getIped ( uint32_t base_addr, uint16_t* Iped )
 {
   CVErrorCodes ret;
   ret = this->read_reg ( base_addr+0x1060, Iped );  // Get pedestal current
   if (ret != cvSuccess) return ret;
   printf("Read back Iped current val %d\n", *Iped);
   return cvSuccess;
 }

CVErrorCodes myCaen::setFsr ( uint32_t base_addr, uint16_t lsb )
 {
   CVErrorCodes ret;
   // uint16_t lsb = int(0.5+8900.0/double(psRESOL));
   uint16_t fsr = uint32_t(0.5+8900.0/double(lsb));
   printf("Setting fsr to %d lsb is %d \n", fsr, lsb);
   ret = this->write_reg ( base_addr+0x1060, fsr );  // Set full scale range
   if (ret != cvSuccess) return ret;
   ret = this->read_reg ( base_addr+0x1060, &fsr );  // Get full scale range
   if (ret != cvSuccess) return ret;
   printf("Read back FSR register val %d\n", fsr);
   lsb = int(8900./double(fsr)+0.5);
   printf("TDC (lsb / fullrange) %d ps / %d ns\n", lsb, int(lsb*4096+0.5)/1000);
   return cvSuccess;
 }

CVErrorCodes myCaen::getFsr ( uint32_t base_addr, uint16_t* fsr )
 {
   CVErrorCodes ret;
   ret = this->read_reg ( base_addr+0x1060, fsr );  // Get full scale range
   if (ret != cvSuccess) return ret;
   printf("Read back FSR register val %d\n", *fsr);
   return cvSuccess;
 }

CVErrorCodes myCaen::getLsb ( uint32_t base_addr, uint16_t* lsb )
 {
   CVErrorCodes ret;
   uint16_t fsr;
   ret = this->read_reg ( base_addr+0x1060, &fsr );  // Get full scale range
   if (ret != cvSuccess) return ret;
   printf("Read back FSR register val %d\n", fsr);
   *lsb = int(8900./double(fsr)+0.5);
   printf("TDC (lsb / fullrange) %d ps / %d ns\n", *lsb, int(*lsb*4096+0.5)/1000);
   return cvSuccess;
 }

#endif // _MY_CAEN_H_
