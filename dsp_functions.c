
// Copyright (c) 2016 Brian Urry.
//

// ----------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int SAMPLE_RATE = 20000;
int FFT_LENGTH = 1024;
int M = 10;
//TODO: Determine if this is the correct range--we may need to reverse time or look at the second half of our array for the math to work!
int ARRAY_START_POINT = 24;//TODO:define where to being checking freqs (should be close, check math)
int ARRAY_STOP_POINT = 60;//TODO:define where to stop checking freq (should be close, check math--freq_found should be around a note in desired range!)
int BIN_SIZE = 20;//SAMPLE_RATE/FFT_LENGTH;
int MIN_AMPLITUDE = 5;//TODO: Define according to input data/test


//int notes[13] = {523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99,
//		783.99, 830.61, 880.00, 932.33, 987.77, 1046.50};
int notes[13] = {523, 554, 587, 622, 659, 698, 740,
		784, 831, 880, 932, 988, 1047};
int num_notes = 13;


int getNote(int* data)
{
	//Do FFT
	int out = fix_fftr(data,M,0);

	//TODO: Determine if we need to do a "power" calculation (something like data = abs(data)^2)

	int note = -1;
	int i;
	int k = 0;
	int max = data[k];
	//pick start and stop point so we only look at the bins we're interested in (C5 to C6 on piano)
	for(i=ARRAY_START_POINT; i < ARRAY_STOP_POINT; i++){
		if(data[i] > max){
			max = data[i];
			k = i;//update max index
		}
	}

	if(max > MIN_AMPLITUDE){
		int freq_found = k * BIN_SIZE;

		//if we want to double check octaves/expand to multiple octaves:
		/*
		 if(data[i/2] > max/4)//harmonic is too high!
		 {
			 freq_found = freq_found/2;
		 }
		 */

		//find actual note!
		for(i=0; i < num_notes; i++){
			if(notes[i] < freq_found){
				break;
			}
		}
		//check which note we're closer to!
		if(i == 0)//corner case
		{
			note = 0;
		}
		else if(i == num_notes -1)//corner case
		{
			note = num_notes -1;
		}
		else{
			int diff_up = notes[i+1] - freq_found;
			int diff_down = freq_found - notes[i];
			if(diff_up > diff_down){//closest to note below
				note = i;
			}
			else{
				note = i+1;
			}
		}
	}

	return note;
}

/* EXTRA DSP STUFF TO USE ELSEWHERE!
  uint32_t N = 32;
  uint32_t inR[N];
  uint32_t inI[N];
  volatile int out;

  for (i = 0; i < N; i++)
    inR[i] = 128;
  for (i = 0; i < N; i++)
    inI[i] = 0;

  out = fix_fft(inR,inI,6,0);

  out = fix_fft(inR,inI,6,1);

  //clear all
  for (i = 0; i < N; i++)
    inR[i] = 0;
  for (i = 0; i < N/2; i++)
    inR[i] = 128;

  out = fix_fft(inR,inI,5,0);

  out = fix_fft(inR,inI,5,1);

  //clear all
  for (i = 0; i < N; i++)
    inR[i] = 0;
  for (i = 0; i < N; i+=2)
    inR[i] = 128;

  out = fix_fftr(inR,6,0);

  out = fix_fftr(inR,6,1);

  //clear all
  for (i = 0; i < N; i++)
    inR[i] = 0;
  for (i = 0; i < N; i+=2)
    inR[i] = 128;

  out = fix_fftr(inR,7,0);

  out = fix_fftr(inR,7,1);
 */








//#include "arm_math.h"

//#define TEST_LENGTH_SAMPLES 2048

//int
//main(int argc, char* argv[])
//{
//  // Send a greeting to the trace device (skipped on Release).
//  trace_puts("Hello ARM World!");
//
//  // At this stage the system clock should have already been configured
//  // at high speed.
//  trace_printf("System clock: %u Hz\n", SystemCoreClock);
//
//  timer_start();
//
//  blink_led_init();
//
//  uint32_t seconds = 0;
//
//  uint32_t i;
//  uint32_t N = 32;
//  uint32_t inR[N];
//  uint32_t inI[N];
//  volatile int out;
//
//  for (i = 0; i < N; i++)
//    inR[i] = 1;
//  for (i = 0; i < N; i++)
//    inI[i] = 1;
//
//
//  uint32_t refIndex = 213, testIndex = 0;
//  float32_t testInput_f32_10khz[2048];
//
//  for (i = 0; i < 2048; i++)
//    testInput_f32_10khz[i] = 1;
//
//  static float32_t testOutput[TEST_LENGTH_SAMPLES/2];
//
//  /* ------------------------------------------------------------------
//  * Global variables for FFT Bin Example
//  * ------------------------------------------------------------------- */
//  uint32_t fftSize = 1024;
//  uint32_t ifftFlag = 0;
//  uint32_t doBitReverse = 1;
//
//  arm_status status;
//  arm_cfft_radix4_instance_f32 S;
//  arm_rfft_instance_f32 Sr;
//  float32_t maxValue;
//
//  status = ARM_MATH_SUCCESS;
//
//  /* Initialize the CFFT/CIFFT module */
//  status = arm_rfft_init_f32(&Sr, &S, fftSize,
//    								ifftFlag, doBitReverse);
//
//  /* Process the data through the CFFT/CIFFT module */
//  	arm_rfft_f32(&Sr, testInput_f32_10khz, testOutput);
//
//
//  	/* Process the data through the Complex Magnitude Module for
//  	calculating the magnitude at each bin */
//  //	arm_cmplx_mag_f32(testInput_f32_10khz, testOutput,
//  //	  				fftSize);
//
//  	/* Calculates maxValue and returns corresponding BIN value */
//  	//arm_max_f32(testOutput, fftSize, &maxValue, &testIndex);
//
//
//
//
//
//
//
//

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
