#include <stdio.h>
#include <CAENVMElib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

#define SEC_OFFSET 1759179710

uint64_t uTime ()
 {
   uint64_t usec;

   struct timeval tv;

   int ret = gettimeofday ( &tv, NULL);

   usec = (tv.tv_sec-SEC_OFFSET)*1000000+tv.tv_usec;

   return usec;
 }

#define BASEADDRESS 0x0f000000

int printtofile(double *v, int dimension){
	FILE *file;
	int i;
	double max = 0.0;
	double mean = 0.0;
	//Open file
	file = fopen("./time_4.dat", "w");
	if (file == NULL){
		printf("Error opening file\n");
		exit(1);
	}
	//loop on time
	fprintf(file, "Execution time\n");
	for(i = 0; i < dimension; i++){
		//calcolate maximum value
		if (max < v[i]){
			max = v[i];
		}
		//calcolate mean
		mean += v[i];
		//print the vector
		fprintf(file, "%f\n", v[i]);
	}
	mean = mean/dimension;
	fprintf(file, "mean = %f \nmax=%f\n", mean, max);
	fclose(file);
	return 0;
}

int main(){
	//varaibles
	double time_v[1000]= {0}; //in ms
	clock_t begin, end;
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
	// CVRegisters v3718reg(cvStatusReg);  // register 0x02
	re = CAENVME_ReadRegister(handle, (CVRegisters)0x02, &data);	
	printf ("re %x reg %x data %x\n", re, 0x02, data);
	// re = CAENVME_ReadRegister(handle, BASEADDRESS+0x1024, &data);
	// printf ("Base addr %x reg %x full addr %x\n", BASEADDRESS, 0x1024, BASEADDRESS+0x1024);
	re = CAENVME_ReadCycle(handle, BASEADDRESS+0x1024, &data, cvA32_U_DATA, cvD16);
	if (re != cvSuccess){
		printf("Error %d ReadCycle\n", re);
		return re;
	}
	printf("Read ok\n");

	//Do something in a loop
	for(i= 0; i<1000; i++){
		//start time
		begin = clock();
		//ReadCycle ROC FPGA of a V17XX
		// re = CAENVME_ReadRegister(handle, v3718reg, &data);
		// re = CAENVME_ReadRegister(handle, BASEADDRESS+0x1024, &data);	
		// re = CAENVME_ReadCycle(handle, BASEADDRESS+0x1024, &data, cvA32_U_DATA, cvD16);     // 150 us secondo clock()  320 us secondo gettimeofday()
		re = CAENVME_ReadRegister(handle, (CVRegisters)0x02, &data);	          // 165 us secondo clock()  320 us secondo gettimeofday()
		//stop time
		end = clock();
		//update time vector
		//we divided for CLOCKS_PER_SEC to have seconds and then multipled for 1000000 to have us
		time_v[i]= ((double)(end - begin))/CLOCKS_PER_SEC * 1000000;
	}
	
	//free handle
	CAENVME_End(handle);
	
	
	//print time vector on std output
	re = printtofile(time_v, 1000);
	
	
	return re;
}
