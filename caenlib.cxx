
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <inttypes.h>

#include <CAENVMElib.h>
#include <CAENVMEtypes.h>

class myCaen {
  public:
    myCaen();
    ~myCaen();

    CVErrorCodes read_reg ( uint32_t reg_addr, uint16_t* val )
      { return CAENVME_ReadCycle ( m_handle, reg_addr, val, cvA32_U_DATA, cvD16 ); }

    CVErrorCodes write_reg ( uint32_t reg_addr, uint16_t val )
      { return CAENVME_WriteCycle ( m_handle, reg_addr, &val, cvA32_U_DATA, cvD16 ); }

    void findModelVersion ( uint16_t model, uint16_t vers, char* modelVersion, int* ch );

    CVErrorCodes initQTP ( uint32_t base_addr );

  private:
    int32_t m_handle;

};

myCaen::myCaen ()
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
            return;
        case 0x13:
            strcpy(modelVersion, "AC");
            *ch = 32;
            return;
        case 0xE1:
            strcpy(modelVersion, "NA");
            *ch = 16;
            return;
        case 0xE3:
            strcpy(modelVersion, "NC");
            *ch = 16;
            return;
        default:
            strcpy(modelVersion, "-");
            *ch = 32;
            return;
        }
        break;
   case 965:
        switch (vers) {
        case 0x1E:
            strcpy(modelVersion, "A");
            *ch = 16;
            return;
        case 0xE3:
        case 0xE1:
            strcpy(modelVersion, " ");
            *ch = 32;
            return;
        default:
            strcpy(modelVersion, "-");
            *ch = 32;
            return;
        }
        break;
   case 775:
        switch (vers) {
        case 0x11:
            strcpy(modelVersion, "AA");
            *ch = 32;
            return;
        case 0x13:
            strcpy(modelVersion, "AC");
            *ch = 32;
            return;
        case 0xE1:
            strcpy(modelVersion, "NA");
            *ch = 16;
            return;
        case 0xE3:
            strcpy(modelVersion, "NC");
            *ch = 16;
            return;
        default:
            strcpy(modelVersion, "-");
            *ch = 32;
            return;
        }
        break;
   case 785:
        switch (vers) {
        case 0x11:
            strcpy(modelVersion, "AA");
            *ch = 32;
            return;
        case 0x12:
            strcpy(modelVersion, "Ab");
            *ch = 32;
            return;
        case 0x13:
            strcpy(modelVersion, "AC");
            *ch = 32;
            return;
        case 0x14:
            strcpy(modelVersion, "AD");
            *ch = 32;
            return;
        case 0x15:
            strcpy(modelVersion, "AE");
            *ch = 32;
            return;
        case 0x16:
            strcpy(modelVersion, "AF");
            *ch = 32;
            return;
        case 0x17:
            strcpy(modelVersion, "AG");
            *ch = 32;
            return;
        case 0x18:
            strcpy(modelVersion, "AH");
            *ch = 32;
            return;
        case 0x1B:
            strcpy(modelVersion, "AK");
            *ch = 32;
            return;
        case 0xE1:
            strcpy(modelVersion, "NA");
            *ch = 16;
            return;
        case 0xE2:
            strcpy(modelVersion, "NB");
            *ch = 16;
            return;
        case 0xE3:
            strcpy(modelVersion, "NC");
            *ch = 16;
            return;
        case 0xE4:
            strcpy(modelVersion, "ND");
            *ch = 16;
            return;
        default:
            strcpy(modelVersion, "-");
            *ch = 32;
            return;
        }
        break;
   case 862:
        switch (vers) {
        case 0x11:
            strcpy(modelVersion, "AA");
            *ch = 32;
            return;
        case 0x13:
            strcpy(modelVersion, "AC");
            *ch = 32;
            return;
        default:
            strcpy(modelVersion, "-");
            *ch = 32;
            return;
        }
        break;
   }
 }

CVErrorCodes myCaen::initQTP ( uint32_t base_addr )
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
   uint16_t cntrl_reg;

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
   printf("Serial Number = %"PRId32"\n", sernum);

   printf("FW Revision = %d.%d\n", (fwrev >> 8) & 0xFF, fwrev & 0xFF);

   ret = this->write_reg ( base_addr+0x1060, Iped );  // Set pedestal
   if (ret != cvSuccess) return ret;

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

#define NQTP 6
uint32_t board[6] = { 0x0a000000, 0x0b000000, 0x0c000000, 0x0d000000, 0x01000000, 0x0f000000 };
#define xNQTP 3
uint32_t xboard[6] = { 0x09000000, 0x05000000, 0x08000000 };

int main ( int argc, char** argv )
 {
   myCaen m_caen;

   for(int i=0; i<NQTP; i++)
    {
      if ( m_caen.initQTP ( board[i] ) != 0 ) goto QuitProgram;
    }

QuitProgram:

   return 0;
 }
