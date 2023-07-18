#include<stdio.h>
#include "head.h"

void square_hw(hls::stream<axis_data> &input, hls::stream<axis_data> & output);
void square_sw(int input[SIZE], int output[SIZE])
{
	for(int i=0;i<SIZE;i++)
	{
		output[i]=input[i]*input[i];
	}
}

int main()
{
	int input[SIZE],output_sw[SIZE],output_hw[SIZE];
	for(int i=0;i<SIZE;i++)
	{
		input[i]=i;
	}
//	square_hw(input,output_hw);
	square_sw(input,output_sw);

	hls::stream<axis_data> in_vec,out_vec;
	axis_data local_read,local_write;

	for(int i=0;i<SIZE;i++)
	{
		local_read.data=input[i];
		if(i==SIZE-1)
		{
			local_read.last=1;
		}
		else
		{
			local_read.last=0;
		}
		in_vec.write(local_read);
	}
	square_hw(in_vec,out_vec);


	for(int i=0;i<SIZE;i++)
	{
		local_write=out_vec.read();
		if(output_sw[i]!=local_write.data)
		{
			printf("Error at %d",i);
			return 1;
		}
	}
	printf("No error\n");
	return 0;

}
