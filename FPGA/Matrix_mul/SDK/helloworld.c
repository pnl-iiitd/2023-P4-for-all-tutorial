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
#include <math.h>
#include "platform.h"
#include "xil_printf.h"
#include <xtime_l.h>		//to use the timers
#include "xparameters.h"	//info. related to DMA are present here
#include "xaxidma.h"		//to use DMA
#define MAX 4
// Matrix order i.e no. of row and col in input matrix
#define row1 8
#define col1 8
#define row2 8
#define col2 8
typedef float M_TYPE;

void mat_mul(M_TYPE mat1[row1][col1], M_TYPE mat2[row2][col2], M_TYPE result[row1][col2])
{
    int i, j, k;
    M_TYPE sum;
    for (i = 0; i < row1; i++)
    {
        for (j = 0; j < col2; j++)
        {
            sum = 0;
            for (k = 0; k < row2; k++)
            {
                sum += (mat1[i][k] * mat2[k][j]);
            }
            result[i][j] = sum;
        }
    }
}

int main()
{
    init_platform();

    //---------------------program_starting---------------------------------
    float sum_processor_PL_0 = 0;
    float sum_processor_PS_0 = 0;
    int index=0;
    for (int a = 1; a <= MAX; a++)
    {
        M_TYPE mat1[row1][col1], mat2[row2][col2], result[row1][col2], result_hw[row1][col2];
        M_TYPE intermediate[index];
        int i, j;


        XTime seed_value;
        XTime_GetTime(&seed_value);
        srand(seed_value);

        // populating the input
        for (i = 0; i < row1; i++)
        {
            for (j = 0; j < col1; j++)
                mat1[i][j] =((float)rand()/(RAND_MAX/6));
        }

        for (i = 0; i < row2; i++)
        {
            for (j = 0; j < col2; j++)
                mat2[i][j] =((float)rand()/(RAND_MAX/6));
        }

        // Declaring the timer variable
        XTime time_PS_start, time_PS_end;
        XTime_SetTime(0);
        XTime_GetTime(&time_PS_start);
        mat_mul(mat1, mat2, result); // calling software function
        XTime_GetTime(&time_PS_end);


        //----------------------This the input for Hardware function (IP)---------------------
        for (i = 0; i < row1; i++)
        {
            for (j = 0; j < col1; j++)
            {
                intermediate[index] = mat1[i][j];
                index=index+1;
            } // multiply by first 8 index
        }
        for (i = 0; i < row2; i++)
        {
            for (j = 0; j < col2; j++)
            {
            	 intermediate[index] = mat2[i][j];
                 index=index+1;
            }
        }

        //------------------------------DMA_1_in_action------------------------------------------//

        // creating config pointer and DMA instances
        XAxiDma_Config *DMA_confptr;
        XAxiDma DMAACP_instance;

        DMA_confptr = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);

        int status;
        status = XAxiDma_CfgInitialize(&DMAACP_instance, DMA_confptr);
        if (status != XST_SUCCESS)
        {
            printf("Failure of  DMA \n");
            return 1; // returning of failure
        }

        // timer variable for the ACP port to connected.
        XTime DMA_ACP_start, DMA_ACP_end;
        // starting the timer of DMA
        XTime_SetTime(0);
        XTime_GetTime(&DMA_ACP_start);
        status = XAxiDma_SimpleTransfer(&DMAACP_instance, (UINTPTR)result_hw, row1 * col2 * sizeof(M_TYPE), XAXIDMA_DEVICE_TO_DMA); // uses axi lite
        if (status != XST_SUCCESS)
        {
            return 1; // returning of failure
        }
        status = XAxiDma_SimpleTransfer(&DMAACP_instance, (UINTPTR)intermediate, (row2 *col2 + row1*col1) * sizeof(M_TYPE), XAXIDMA_DMA_TO_DEVICE); // uses axi lite
        if (status != XST_SUCCESS)
        {
            return 1; // returning of failure
        }

        //	  XAxiDMA_readreg(Base add. of dma)
        status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, 0x04) & 0x00000002;
        //	  status=status&0x00000002;
        while (status != 0x00000002)
        {
            status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, 0x04) & 0x00000002;
        }
        status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, 0x34) & 0x00000002;
        while (status != 0x00000002)
        {
            status = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, 0x34) & 0x00000002;
        }
        XTime_GetTime(&DMA_ACP_end);

        // Compare benchmark and hardware function output
        for (i = 0; j < row1; j++)
        {
            for (j = 0; j < col2; j++)
            {
                if (fabs(result_hw[i][j] - result[i][j]) > 0.01)
                {
                    printf("Error at row index %d and col index %d\n", i, j);
                    printf("Hardware output %f\n", result_hw[i][j]);
                    printf("software output %f\n", result[i][j]);
                    return 1;
                }
            }
        }

        printf("No Error!!");

        // PS time
        printf("for Iteration %d\n", a);
        printf("----------------------------------------------------------------\n");
        float time_processor_PS_0 = 0;
        time_processor_PS_0 = (float)1.0 * (time_PS_end - time_PS_start) / (COUNTS_PER_SECOND / 1000000);
        sum_processor_PS_0 = sum_processor_PS_0 + time_processor_PS_0;
        printf("Execution Time for PS in Micro-Seconds : %f\n", time_processor_PS_0);


        float time_processor_PL_0 = 0;
        time_processor_PL_0 = (float)1.0 * (DMA_ACP_end - DMA_ACP_start) / (COUNTS_PER_SECOND / 1000000);
        sum_processor_PL_0 = sum_processor_PL_0 + time_processor_PL_0;
        printf("\nExecution Time for PL in Micro-Seconds : %f\n", time_processor_PL_0);
        printf("\n----------------------------------------------------------------\n");


    } // end of for loop


    printf("===================In summary=========================================\n");
    printf("Average time of PS in us                      : %f\n", sum_processor_PS_0 / MAX);
    printf("Average time of PL with optimization in us    : %f\n", sum_processor_PL_0 / MAX);
    cleanup_platform();
    return 0;
}
