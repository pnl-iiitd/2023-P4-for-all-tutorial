/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xtime_l.h>
#include "xparameters.h"
#include "xaxidma.h"

#define SIZE 1024
void square_sw(int input[SIZE], int output[SIZE])
{
	for(int i=0;i<SIZE;i++)
	{
		output[i]=input[i]*input[i];
	}
}
int input[SIZE],output_sw[SIZE],output_hw_ACP[SIZE];


int main()
{
	    init_platform();

	//initializing the inputs
    	for(int i=0;i<SIZE;i++)
    	{
    		input[i]=i;
    	}

//Declaring the timer variable
     XTime time_PS_start , time_PS_end;

    XTime_SetTime(0);
    XTime_GetTime(&time_PS_start);
    square_sw(input,output_sw); 		//calling the Software function
    XTime_GetTime(&time_PS_end);

//-----------------------------------------FPGA____ACP____operation------------------------------------


    //creating config pointer and DMA instances
   	  XAxiDma_Config *DMA_confptr;
   	  XAxiDma DMAACP_instance;
   	  DMA_confptr= XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
   	  int status;
   	  status=XAxiDma_CfgInitialize(&DMAACP_instance,DMA_confptr);
   	  if (status!=XST_SUCCESS)
   	  {
   		  printf("Failure if ACP DMA \n");
   		  return 1;//returning of failure
   	  }
   	  //timer variable for the ACP port to connected.
   	  XTime DMA_ACP_start , DMA_ACP_end;
   	  //starting the timer of DMA
   	  XTime_SetTime(0);
   	  XTime_GetTime(&DMA_ACP_start);

   	  status=XAxiDma_SimpleTransfer(&DMAACP_instance, (UINTPTR)output_hw_ACP,SIZE*sizeof(int),XAXIDMA_DEVICE_TO_DMA); //uses axi lite
   	  if (status!=XST_SUCCESS)
   	 	  {
   	 		  return 1;//returning of failure
   	 	  }
   	 status=XAxiDma_SimpleTransfer(&DMAACP_instance, (UINTPTR)input,SIZE*sizeof(int),XAXIDMA_DMA_TO_DEVICE); //uses axi lite
   	  if (status!=XST_SUCCESS)
   		 	  {
   		 		  return 1;//returning of failure
   		 	  }

   //	  XAxiDMA_readreg(Base add. of dma)
   	  status=XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x04) & 0x00000002;
   //	  status=status&0x00000002;
   	  while(status!=0x00000002)
   	  {
   		 status=XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x04) & 0x00000002;
   	  }
   	  status=XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x34) & 0x00000002;
   	  while(status!=0x00000002)
   	  {
   		status=XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR,0x34) & 0x00000002;
   	  }
   	  XTime_GetTime(&DMA_ACP_end);


    for(int i=0;i<SIZE;i++)
    	{
    		if(output_sw[i]!=output_hw_ACP[i])
    		{
    			printf("Error at %d",i);
    			return 1;
    		}
    	}
    printf("No error\n");


    printf("\n PS  and  PL output: \r\n");
    	for (int i = 0 ; i < SIZE ; i++)
    		  {
    			  printf("output for PS: %d \n",output_sw[i] );
    			  printf("output for PL: %d \n",output_hw_ACP[i] );
    		  }



    	printf("\n-----------EXECUTION TIME of PL-------------------\n");
    float time_processorr = 0;
    time_processorr = (float)1.0 * (DMA_ACP_end - DMA_ACP_start) / (COUNTS_PER_SECOND/1000000);
    printf("Execution Time for PL in Micro-Seconds : %f\n" , time_processorr);

    //PS time
    printf("\n-----------EXECUTION TIME OF PS--------------------\n");
    float time_processor = 0;
    time_processor = (float)1.0 * (time_PS_end - time_PS_start) / (COUNTS_PER_SECOND/1000000);
    printf("Execution Time for PS in Micro-Seconds : %f\n" , time_processor);
    cleanup_platform();
    return 0;

}


