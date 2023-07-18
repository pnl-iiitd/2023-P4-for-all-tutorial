#include"head.h"

void square_hw(hls::stream<axis_data> &input, hls::stream<axis_data> & output)
{
#pragma HLS INTERFACE axis register both port=output
#pragma HLS INTERFACE axis register both port=input
#pragma HLS INTERFACE ap_ctrl_none port=return

	axis_data local_read,local_write;
	for(int i=0;i<SIZE;i++)
	{
		local_read=input.read();
		local_write.data=(local_read.data)*(local_read.data);
		if(i==SIZE-1)
		{
			local_write.last=1;
		}
		else
		{
			local_write.last=0;
		}
		output.write(local_write);
	}
}
