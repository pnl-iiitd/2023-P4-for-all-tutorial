#include"mat.h"

void mat_mul(M_TYPE mat1[row1][col1], M_TYPE mat2[row2][col2], M_TYPE result[row1][col2]);

int main()
{
	M_TYPE mat1[row1][col1], mat2[row2][col2],result[row1][col2],result_hw[row1][col2];
	int i,j,k;

	//populating the input
	for(i=0;i<row1;i++)
	{
		for(j=0;j<col1;j++)
		mat1[i][j]=i;
	}

	for(i=0;i<row2;i++)
	{
		for(j=0;j<col2;j++)
		mat2[i][j]=i;
	}


	//calling software function of matrix mul.
	mat_mul(mat1,mat2,result);

//-------------------Hardware function---------------------
hls::stream<axis_data> input,output;      //defining the stream
axis_data local_stream;					//defining the struct to read and write from the stream

//Generating the input for stream from pre-defined data types
for(i=0;i<row1;i++)
	{
		for(j=0;j<col1;j++)
		{
			local_stream.data=mat1[i][j];
			//Generating the last signal
			if(i==2 && j==2)
					{
						local_stream.last=1;
					}
			else
			{
				local_stream.last=0;
			}
			input.write(local_stream);
		}
	}

//Generating the input for stream from pre-defined data types
for(i=0;i<row2;i++)
	{
		for(j=0;j<col2;j++)
		{
			local_stream.data=mat2[i][j];

			if(i==row2-1 && j==col2-1)
			{
				local_stream.last=1;
			}
			else
			{
				local_stream.last=0;
			}
			input.write(local_stream);
		}
	}
//calling the hardware function
#ifdef solution1
mat_mul_hw_1(input,output);
#endif

#ifdef solution2
mat_mul_hw_2(input,output);
#endif





// converting the output received from the stream to per-defined data type
for(i=0; i<row1;i++)
{
	for(j=0;j<col1;j++)
	{
		local_stream=output.read();
		result_hw[i][j]=local_stream.data;
	}
}

//Compare benchmark and hardware function output
for(i=0;j<row1;j++)
{
	for(j=0;j<col2;j++)
	{
		if (fabs(result_hw[i][j]-result[i][j])>0.01)
		{
			printf("Error at row index %d and col index %d\n",i,j);
			printf("Hardware output %f\n",result_hw[i][j]);
			printf("software output %f\n",result[i][j]);
			return 1;

		}
	}
}

	printf("No Error!!");




	return 0;
}


void mat_mul(M_TYPE mat1[row1][col1], M_TYPE mat2[row2][col2], M_TYPE result[row1][col2])
{
	int i,j,k;
	M_TYPE sum;
	for( i=0;i<row1;i++)
	{
		for( j=0;j<col2;j++)
		{
			sum=0;
			for( k=0;k<row2;k++)
				{
					sum+=(mat1[i][k]*mat2[k][j]);
				}
			result[i][j]=sum;
		}
	}

}

