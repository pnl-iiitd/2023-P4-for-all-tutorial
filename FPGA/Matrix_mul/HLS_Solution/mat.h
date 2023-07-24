#include<stdio.h>
#include <hls_stream.h>	//to used stream
#include <ap_int.h>    //to used 1 bit signal

#define solution1

#ifdef solution1
typedef float M_TYPE;
#endif



#ifdef solution2
typedef float M_TYPE;
#endif


//Matrix order i.e no. of row and col in input matrix
#define row1 8
#define col1 8
#define row2 8
#define col2 8

//stream size along with last signal
struct axis_data{
	M_TYPE data;
	ap_uint<1> last;
};


//hardware function declaration
void mat_mul_hw_1(hls::stream<axis_data> &input, hls::stream<axis_data> &output);
void mat_mul_hw_2(hls::stream<axis_data> & input, hls::stream<axis_data> &output);


