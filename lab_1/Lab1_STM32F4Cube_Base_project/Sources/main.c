/*
ECSE 426 LAB-1
Author: Chuan Qin, chuan.qin2@mail.mcgill.ca
				Wei Wang, wei.wang18@mail.mcgill.ca
Date: 2016-02-05
Note: Te Testbnch for C is written in int main()
 */
#include <stdio.h>
#include "arm_math.h"
//#include "testc.h"
#include "demo.h"

struct kalman_state
{
	float q;
	float r;
	float x;
	float p;
	float k;
};

extern int Kalmanfilter(float* InputArray, float* OutputArray, int Length, struct kalman_state* kstate);

int Kalmanfilter_C(const float* InputArray, float* OutputArray, struct kalman_state* kstate, int Length){
	int i;
	for (i = 0; i < Length; i++) {
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (InputArray[i] - kstate->x);
		kstate->p = (1 - kstate->k) * kstate->p;
		
		OutputArray[i] = kstate->x;
		
		if (OutputArray[i] == NAN || OutputArray[i] == INFINITY) {
		return 1;
		}
		//printf("OutputArray[%d] = %f\n", i, OutputArray[i]);
	}
	return 0;
}

//Initialize kalman state
int Kstateinitialization(struct kalman_state* kstate){
	kstate->q = DEF_q;
	kstate->r = DEF_r;
	kstate->x = DEF_x;
	kstate->p = DEF_p;
	kstate->k = DEF_k;
	
	return 0;
}

//Subtraction of original data stream and data obtained by Kalman filter tracking
int Subtraction(const float* InputArray, float* OutputArray, float* Difference, int Length){
	int i;
	for (i = 0; i < Length; i++) {
		Difference[i] = InputArray[i] - OutputArray[i];
	}
	return 0;
}

//Calculation of the average of a set of elements
float Average(float* Array, int Length){
	float sum = 0;
	int i;
	for (i = 0; i < Length; i++){
		sum += Array[i];
	}	
	return (sum / Length);
}

//Calculation of the standard deviation of a set of elements
float StdDeviation(float* Array, int Length){
	float sumstd = 0;
	float avg = Average(Array, Length);
	int i;
	for (i = 0; i < Length; i++){
		sumstd +=((Array[i] - avg) * (Array[i] - avg));
		}
	return sqrt(sumstd / (Length - 1));
}

//Calculation of the correlation between the original and tracked vectors
int CrossCorrelation(const float* InputArray, float* OutputArray, float* Correlation, int Length){
	int size = 2 * Length - 1;
	int i, j;
	for (i = 0; i < size; i++){
		Correlation[i] = 0;
		for (j = 0; j < size - i; j++){
			if (j < Length && (i + j - Length + 1) >= 0){
				Correlation[i] += OutputArray[j] * InputArray[j + i - Length + 1];
			}
		}
		//printf("Correlation[%d] = %f\n", i, Correlation[i]);
	}
	return 0;
}

//Calculation of the convolution between two vectors
int Convolution(const float* InputArray, float* OutputArray, float* Convol, int Length){
	int size = 2 * Length - 1;		
	int i, j;
	for (i = 0; i < size; i++){
		Convol[i] = 0;
		for (j = 0; j <= i; j++){
			if (j < Length && (i - j) < Length){
				Convol[i] += InputArray[j] * OutputArray[i - j];
			}
		}
		//printf("Convolution[%d] = %f\n", i, Convol[i]);
	}
	return 0;
}

//Comparison between two arrays
int Comparison(float* Array1, float* Array2, int Length){
	int i, res;
	//acceptable error margin for the difference between two float numbers
	float error = 0.00001;
	for (i = 0; i < Length; i++) {
		if (fabsf(Array1[i] - Array2[i]) > error) {
			printf("Error at index [%d] of set {%x, %x} in IEEE format\n", i, *(unsigned int*)(&Array1[i]), *(unsigned int*)(&Array2[i]));
			res = 1;
		}
	}
	return res;
}

int main(){	

	float OutputArray[sizeof(measurements) / sizeof(float)];
	struct kalman_state kstate;
	
	float InputArray1[sizeof(measurements) / sizeof(float)];
	float OutputArray1[sizeof(measurements) / sizeof(float)];
	struct kalman_state kstate1;

	float Difference[sizeof(measurements) / sizeof(float)];	
	float average;
	float stddev;
	float Correlation[(sizeof(measurements) + sizeof(OutputArray)) / sizeof(float) - 1];
	float Convol[(sizeof(measurements) + sizeof(OutputArray)) / sizeof(float) - 1];	

	float Difference1[sizeof(measurements) / sizeof(float)];
	float average1;
	float stddev1;
	float Correlation1[(sizeof(measurements) + sizeof(OutputArray)) / sizeof(float) - 1];
	float Convol1[(sizeof(measurements) + sizeof(OutputArray)) / sizeof(float) - 1];

	//acceptable error margin for the difference between two float numbers
	float error = 0.00001;
	
	Kstateinitialization(&kstate);
	if (!Kalmanfilter_C(measurements, OutputArray, &kstate, length)){
		printf("Kalman Filter Works Properly!\n");
	}
	else {
	printf("Error ocurred due to NaN or Infinity"); 
	}
	
	//memcpy const float* InputArray to float* InputArray1 for assembly
	memcpy(InputArray1, measurements, sizeof(measurements));
	//Assebmly Function for Kalman Filter	
	if (!Kalmanfilter(InputArray1, OutputArray1, length, &kstate1)){
		printf("Assembly Function Works Properly!\n");
	}
	else {
		printf("Error occureed due to overflow exception, division exception, or invalid by zero exception\n");
	}

	
	//5 operations using C implementation above
	Subtraction(measurements, OutputArray, Difference, length);
	average = Average(Difference, length);
	stddev = StdDeviation(Difference, length);
	CrossCorrelation(measurements, OutputArray, Correlation, length);
	Convolution(measurements, OutputArray, Convol, length);

	//5 operations using the CMSIS-DSP library
	arm_sub_f32(InputArray1, OutputArray1, Difference1, length);
	arm_mean_f32(Difference1, length, &average1);
	arm_std_f32(Difference1, length, &stddev1);
	arm_correlate_f32(InputArray1, length, OutputArray, length, Correlation1);
	arm_conv_f32(InputArray1, length, OutputArray, length, Convol1);

	if (Comparison(OutputArray, OutputArray1, length)){
		printf("The output calculated using C and the one calculated using assembly are not the same.\n");
	}
	//Compare the results of all 4 different operations calculated using C with those using CMSIS-DSP library
	if (fabsf(average - average1) > error || fabsf(stddev - stddev1) > error){
		printf("The average or the standard deviation obtain from C implementation is not equal to those from CMSIS-DSP library.\n");
	}
	if (Comparison(Correlation, Correlation1, 2 * length - 1)){
		printf("The cross correaltion calculated using C and the one calculated using CMSIS-DSP library are not the same.\n");
	}
	if (Comparison(Convol, Convol1, 2 * length - 1)){
		printf("The convolution calculated using C and the one calculated using CMSIS-DSP library are not the same.\n");
	}

	return 0;
}
