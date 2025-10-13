
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#include <inttypes.h>
#define Sleep(x) usleep((x)*1000)

#include <CAENVMElib.h>
#include <CAENVMEtypes.h>

#define ENABLE_LOG 0

int VMEerror ( 0 );
char ErrorString[100];
FILE *logfile;

/*******************************************************************************/
/*                               READ_REG                                      */
/*******************************************************************************/
uint16_t read_reg(int32_t handle, uint32_t BaseAddress, uint16_t reg_addr)
{
        uint16_t data=0;
        CVErrorCodes ret;
        ret = CAENVME_ReadCycle(handle, BaseAddress + reg_addr, &data, cvA32_U_DATA, cvD16);
        if(ret != cvSuccess) {
                sprintf(ErrorString, "Cannot read at address %08X\n", (uint32_t)(BaseAddress + reg_addr));
                VMEerror = 1;
        }
        if (ENABLE_LOG)
                fprintf(logfile, " Reading register at address %08X; data=%04X; ret=%d\n", (uint32_t)(BaseAddress + reg_addr), data, (int)ret);
        return(data);
}



/*******************************************************************************/
/*                                WRITE_REG                                    */
/*******************************************************************************/
void write_reg(int32_t handle, uint32_t BaseAddress, uint16_t reg_addr, uint16_t data)
{
        CVErrorCodes ret;
        ret = CAENVME_WriteCycle(handle, BaseAddress + reg_addr, &data, cvA32_U_DATA, cvD16);
        if(ret != cvSuccess) {
                sprintf(ErrorString, "Cannot write at address %08X\n", (uint32_t)(BaseAddress + reg_addr));
                VMEerror = 1;
        }
        if (ENABLE_LOG)
                fprintf(logfile, " Writing register at address %08X; data=%04X; ret=%d\n", (uint32_t)(BaseAddress + reg_addr), data, (int)ret);
}


static void findModelVersion(uint16_t model, uint16_t vers, char *modelVersion, int *ch) {
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


uint32_t initQTP ( int32_t handle, uint32_t BaseAddress )
{
	int brd_nch(32);
	uint16_t QTP_LLD[32] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	char modelVersion[3];
	uint16_t fwrev, vers, model;
        uint32_t sernum;
	uint16_t Iped ( 255 );                    // pedestal of the QDC (or resolution of the TDC)
	int EnableSuppression ( 0 );              // Enable Zero and Overflow suppression if QTP boards


	// ************************************************************************
        // QTP settings
        // ************************************************************************
        // Reset QTP board
        write_reg(handle,BaseAddress,0x1016, 0);
        if (VMEerror) {
                printf("Error during QTP programming: ");
                printf(ErrorString);
                return -1;
        }

        // Read FW revision
        fwrev = read_reg(handle,BaseAddress,0x1000);
        if (VMEerror) {
                printf(ErrorString);
                return -1;
        }

        model = (read_reg(handle,BaseAddress,0x803E) & 0xFF) + ((read_reg(handle,BaseAddress,0x803A) & 0xFF) << 8);
        // read version (> 0xE0 = 16 channels)
        vers = read_reg(handle,BaseAddress,0x8032) & 0xFF;

        findModelVersion(model, vers, modelVersion, &brd_nch);
        printf("\nBase Address 0x%X Model = V%d%s\n", BaseAddress, model, modelVersion);

        // Read serial number
        sernum = (read_reg(handle,BaseAddress,0x8F06) & 0xFF) + ((read_reg(handle,BaseAddress,0x8F02) & 0xFF) << 8);
        if (sernum == UINT32_C(0xFFFF)) {
                sernum = (read_reg(handle,BaseAddress,0x8EF6) & 0xFF) + ((read_reg(handle,BaseAddress,0x8EF4) & 0xFF) << 8) + ((read_reg(handle,BaseAddress,0x8EF2) & 0xFF) << 16); + ((read_reg(handle,BaseAddress,0x8EF0) & 0xFF) << 24);
        }
        printf("Serial Number = %d\n", sernum);

        printf("FW Revision = %d.%d\n", (fwrev >> 8) & 0xFF, fwrev & 0xFF);

        write_reg(handle,BaseAddress,0x1060, Iped);  // Set pedestal
        write_reg(handle,BaseAddress,0x1010, 0x60);  // enable BERR to close BLT at and of block

        // Set LLD (low level threshold for ADC data)
        write_reg(handle,BaseAddress,0x1034, 0x100);  // set threshold step = 16
        for(int i=0; i<brd_nch; i++) {
                if (brd_nch == 16)      write_reg(handle,BaseAddress,0x1080 + i*4, QTP_LLD[i]/16);
                else                            write_reg(handle,BaseAddress,0x1080 + i*2, QTP_LLD[i]/16);
        }

        if (!EnableSuppression) {
                write_reg(handle,BaseAddress,0x1032, 0x0010);  // disable zero suppression
                write_reg(handle,BaseAddress,0x1032, 0x0008);  // disable overrange suppression
                write_reg(handle,BaseAddress,0x1032, 0x1000);  // enable empty events
        }

        printf("Ctrl Reg = %04X\n", read_reg(handle,BaseAddress,0x1032));
        printf("QTP board programmed\n");

	return 0;
}

// QTP_BASE_ADDRESS 0x0a000000   // V792 in slot 8
// QTP_BASE_ADDRESS 0x0b000000   // V792 in slot 9
// QTP_BASE_ADDRESS 0x0c000000   // V792 in slot 11
// QTP_BASE_ADDRESS 0x0d000000   // V792 in slot 13
// QTP_BASE_ADDRESS 0x01000000   // V862 in slot 15
// QTP_BASE_ADDRESS 0x0f000000   // V775N in slot 17

// uint32_t board[] = { 0x05000000, 0x0a000000, 0x0b000000, 0x0c000000, 0x0d000000, 0x09000000, 0x01000000, 0x0f000000, 0x08000000 }; // v862 0x0e000000 removed
uint32_t board[] = { 0x0d000000, 0x0a000000, 0x0b000000, 0x0c000000, 0x05000000, 0x09000000, 0x01000000, 0x08000000, 0x0f000000 }; // v862 0x0e000000 removed

uint32_t xboard[] = { 0x0D000000 };

#define NQTP (sizeof(board)/sizeof(uint32_t))

int main ( int argc, char** argv )
 {
   CVBoardTypes ctype ( cvUSB_A4818 );
   uint32_t pid ( 49086 );
   int bdnum ( 0 );
   int32_t handle ( -1 );

   CVErrorCodes ret = CAENVME_Init2 ( ctype, &pid, bdnum, &handle );
   if ( ret != cvSuccess )
    {
      printf("X Can't open VME controller\n");
      Sleep(1000);
      goto QuitProgram;
    }

   for(int i=0; i<NQTP; i++)
    {
	    sleep(1);
      if ( initQTP ( handle, board[i] ) != 0 ) goto QuitProgram;
    }

QuitProgram:
   if (handle >= 0) CAENVME_End(handle);

   return 0;
 }
