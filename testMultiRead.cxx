
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>

#include <CAENVMElib.h>

#define SEC_OFFSET 1759179710

uint64_t uTime ()
 {
   uint64_t usec(0);
   struct timeval tv;

   int ret = gettimeofday ( &tv, NULL);

   usec = (tv.tv_sec-SEC_OFFSET)*1000000+tv.tv_usec;

   return usec;
 }

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
CVErrorCodes ec_list[NREAD];

uint32_t reading_list[NREAD];

uint32_t getHandle()
 {
   CVBoardTypes ctype ( cvUSB_A4818 );
   uint32_t pid ( 49086 );
   int bdnum ( 0 );

   int32_t handle;

   CVErrorCodes ret = CAENVME_Init2 ( ctype, &pid, bdnum, &handle );
   
   if ( ret != cvSuccess )
    {
      printf("X Can't open VME controller\n");
      exit(-1);
    }
   printf("X Connection is on - handle %d\n", handle);

   return handle;
}

int main ( int argc, char** argv )
 {
   int32_t handle;
   CVErrorCodes cv;
   uint32_t u0, u1, ut(0);

   handle = getHandle();

   for (int j=0; j<NREAD; j++)
    { 
      am_list[j] = cvA32_U_DATA;
      dw_list[j] = cvD16;
    }

   for (int j=0; j<1000; j++)
    {
      u0 = uTime();
      cv = CAENVME_MultiRead ( handle, address_list, reading_list, NREAD, am_list, dw_list, ec_list );
      u1 = uTime() - u0;
      ut += u1;
      printf ( "%d - time %d\n", j, u1 );
    }
   ut /= 1000;
   printf ( "average time %d\n", ut );

   for (int j=0; j<NREAD; j++)
    { 
      printf ( "reading_list [%d] %d\n", j, reading_list[j] );
    }

   CAENVME_End ( handle );

   return 0;
 }
