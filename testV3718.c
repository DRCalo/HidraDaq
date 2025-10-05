#include <stdio.h>
#include <CAENVMElib.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#define BASEADDRESS 0x0f000000

int main(){
	//varaibles
	int re, i;
	int32_t handle;
	int link = 1;
	unsigned int data;

	printf("Let's start\n");
	
	//Connect to V4718
	// re = CAENVME_Init2(cvV1718, &link, 0, &handle);
	//Connect to V4818
	link = 49086;
	re = CAENVME_Init2(cvUSB_A4818, &link, 0, &handle);
	if (re != cvSuccess){
		printf("Error %d connecting the bridge\n", re);
		return re;
	}
	printf("Connection Done - handle %d \n", handle);
	
	//check if everything is working fine
	re = CAENVME_ReadRegister(handle, (CVRegisters)0x02, &data);	
	if (re != cvSuccess){
		printf("Error %d reading the V3718\n", re);
		return re;
	}
	printf ("re %x reg %x data %x\n", re, 0x02, data);

	data = 0x03;
	re = CAENVME_WriteRegister(handle, (CVRegisters)0x2D, data);
	if (re != cvSuccess){
		printf("Error %d reading the V3718\n", re);
		return re;
	}

	re = CAENVME_ReadRegister(handle, (CVRegisters)0x2D, &data);	
	if (re != cvSuccess){
		printf("Error %d reading the V3718\n", re);
		return re;
	}
	printf ("re %x reg %x data %x\n", re, 0x2D, data);

	//free handle
	CAENVME_End(handle);
	
	return re;
}
