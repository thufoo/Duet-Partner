//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include <math.h>

#include "Timer.h"
#include "BlinkLed.h"

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 3 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int16_t whichArray = 1;
int16_t dataArray1[1024];
int16_t dataArray2[1024];
int16_t arrayPosition = 0;//initialize to zero
int16_t arrayComplete = 0;//initialize to zero



// Initialize the ADC peripheral
void ADC_init() {

	/// TODO: Configure an ADC input pin

	/* Hint: Note that PA0 is already used by the button, don't try to use that channel
	 *       You will need to remember what ADC channel your pin uses so you can
	 *       configure the ADC to use it.
	 *
	 *       Remember that the ADC and DAC are analog peripherals
	 */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable peripheral clock to GPIOB
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // Enable ADC 1 clock
	GPIOB->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER0_1); // Set PB0 to analog

    // Configure the ADC
	// PB0 = ADC12_IN8 (p. 49 of Datasheet)
	ADC1->SQR3 |= (ADC_SQR3_SQ1 & 0x00000008); // Assign channel 8 as the first conversion
	ADC1->CR1 &= ~ADC_CR1_RES; // Set resolution to 12-bits (bits [25:24] = 00)
	ADC1->CR2 &= ~ADC_CR2_EXTEN; // Set free-run mode (no trigger) (bits [29:28] = 00)
	ADC1->CR2 &= ~ADC_CR2_CONT; // Set to single conversion mode
	// ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE; // Set asynchronous clock

	// Enable the ADC and wait until it's ready to run
	ADC1->CR2 |= ADC_CR2_ADON; // Turn it on!
	// Wait for the ADC to turn on?

	SysTick_Config(20000); // SystemCoreClock = 16 MHz; set to 20 kHz
	NVIC_SetPriority(SysTick_IRQn, 1); // Set priority of SysTick.
}


void SysTick_Handler(void)
{
	// Write the ADC value into the current array
	if(whichArray==1){
		dataArray1[arrayPosition] = ADC1->DR;
	}
	else if(whichArray==2){
		dataArray2[arrayPosition] = ADC1->DR;
	}

	arrayPosition++; // Increment arrayPosition
	if(arrayPosition==1024){
		arrayPosition  = 0; // Reset array position

		// Set whichArray to fill the other array
		if(whichArray==1){
			whichArray = 2;
		}
		else if(whichArray==2){
			whichArray = 1;
		}

		// Set the flag to indicate the we are ready to process an array
		arrayComplete = 1;
	}

	// Signal/Trigger one ADC conversion
	ADC1->CR2 |= ADC_CR2_SWSTART;
}



int
main(int argc, char* argv[])
{

	int16_t note = -1;

	ADC_init();	// Initialize the ADC

	while (1) {
		// Check if a data array is ready for FFT processing
		if(arrayComplete==1){
			arrayComplete = 0; // Reset arrayComplete

			// Send the other array out for processing
			if(whichArray==1){
				int16_t out = fix_fftr(dataArray2,10,0);
//				for(int i = 512; i < 1024; i++){
//					trace_printf("%d, ", dataArray2[i]);
//				}
				note = getNote(dataArray2);

			}
			else if(whichArray==2) {
				int16_t out = fix_fftr(dataArray1,10,0);
				note = getNote(dataArray1);
			}

		}
	}
}






//int
//main(int argc, char* argv[])
//{
//  // Send a greeting to the trace device (skipped on Release).
//  trace_puts("Hello ARM World!");
//
//  timer_start();
//
//  blink_led_init();

//  int size = 1024;
//  double two_pi = 3.14159265*2;
//  int16_t samples[size];
//  for(int i = 0; i < size; i++){
//	  samples[i] = (int16_t)(32000*sin(two_pi*i*0.1));
//  }
//
//  uint32_t seconds = 0;
//
//  uint32_t i;
//  uint32_t N = 128;
//  uint32_t M = 7;//log2(N)+2
//  int16_t inR[N];
//  int16_t inI[N];
//  volatile int out;
//
//  for (i = 0; i < N; i++)
//    inR[i] = 128;
////  for (i = 0; i < N; i++)
////    inI[i] = 0;
////
//  out = fix_fftr(inR,M,0);
//
//  out = fix_fftr(inR,M,1);
//
//  out = fix_fftr(samples,M,0);
//
//  out = fix_fftr(samples,M,1);
//
//  //clear all
//  for (i = 0; i < N; i++)
//    inR[i] = 0;
//  for (i = N/4; i < 3*N/4; i++)
//    inR[i] = 128;
//
//  out = fix_fftr(inR,M,0);
//
//  out = fix_fftr(inR,M,1);
//
//  //clear all
//  for (i = 0; i < N; i++)
//    inR[i] = 0;
//  for (i = 0; i < N; i+=2)
//    inR[i] = 128;
//
//  out = fix_fftr(inR,M,0);
//
//  out = fix_fftr(inR,M,1);

//  // Infinite loop
//  while (1)
//    {
//      blink_led_on();
//      timer_sleep(seconds == 0 ? TIMER_FREQUENCY_HZ : BLINK_ON_TICKS);
//
//      blink_led_off();
//      timer_sleep(BLINK_OFF_TICKS);
//
//      ++seconds;
//      // Count seconds on the trace device.
//      trace_printf("Second %u\n", seconds);
//    }
//  // Infinite loop, never return.
//}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
