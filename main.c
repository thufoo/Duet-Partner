#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f407xx.h"
#include "diag/Trace.h"
#include <math.h>
//#include "stm32f4discovery.h"
//#include "Timer.h"
//#include "BlinkLed.h"

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

int16_t index = 0;
//E, A, E, A, E, A, E... (620, 880, ...)
int16_t primo[9] = {0, 4, 10, 4, 10, 4, 10, 4, 3, 4};
//A, C, E, C, A, C, E... (698, 988, 392, ...  (466, 554, 330, 554, ...
int16_t secondo[9] = {0, 9, 12, 4, 12, 9, 12, 4, 4, 4};

// Initialize the ADC peripheral
void ADC_init() {

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
 NVIC_SetPriority(SysTick_IRQn, 2); // Set priority of SysTick.
}


//void LED_GREEN_AF_init(){
//
//    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;                 // Enable peripheral clock to GPIOC
//    GPIOC->MODER |= GPIO_MODER_MODER9_1;             // Set PC9 to AF mode
//    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_9);            // Set to push-pull output type
//    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR9_0     // Set to low speed
//        | GPIO_OSPEEDER_OSPEEDR9_1);
//    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR9_0             // Set to no pull-up/down
//        | GPIO_PUPDR_PUPDR9_1);
//    GPIOC->AFR[1] |= (0x20);//GPIO_AFRH_AFR9);                //alternate function 0 on pin 9
//}
//
//void TIM3_PWM_mode(){
///*     TODO: Connect TIM3 to the appropriate bus */
//    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
//
///*    TODO: Set the appropriate time base registers
//          like you did for TIM2
//*/
///* Algorithm to use to set to "desired_Hz":
// * PSC = (SystemCoreClock/10^6) -1 //this gives tick every 1MHz
// * ARR = 10^6/desired_Hz
// */
//
//    TIM3->PSC = (SystemCoreClock/1000000) - 1;
//    TIM3->ARR = 1000000/1000; // 1000Hz desired frequency
//
///* Create a new OCInitTypeDef object.
//    The TIM_OCInitTypeDef struct is used
//    as a parameter to the TIM_OCxInit()
//    functions. Every member needs a value
//    before it can be sent through the function. */
//    TIM_OCInitTypeDef  TIM_OCInitStructure;
//
///*    TODO: Fill in the values of the members in the
//           TIM_OCINitTypeDef struct. Looking at the
//             comments for this struct in the stm32f0xx_tim.h
//          file will be very helpful. I've filled in several
//          of the less useful members for you.
//
//          By defining these members we are able to instantiate
//          PWM on multiple channels with minimal effort. If you
//          look at the TIM_OCxInit() functions in the
//          stm32f0xx_tim.c file, you'll notice that the bits for
//          some of the registers are shifted in reference to
//          the channel you'd like to configure. This is a very
//          useful feature.
// */
//
///*    The type of output compare mode. This member is analagous
//    to Bits [6:4] in the CCMR2. We'd like to use
//    PWM Mode 1 here. */
//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;    /* TODO */
//
///*    Bits for CCER. The Output Compare signal is output on the
//    corresponding output pin, but is shifted for you in the
//    initiation function. Check out the function if you need
//    a better idea. */
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* TODO */
//
///*    Maps to CCRx. This is the value that will control your
//    duty cycle in relation to ARR and PSC */
//    TIM_OCInitStructure.TIM_Pulse = (uint32_t) TIM3->ARR/2;    /* TODO */
//
///*    Bit 2 in CCER. Reserved, always read as zero. */
//    TIM_OCInitStructure.TIM_OutputNState = (uint16_t) 0;
//
///*     Output polarity- We want an active low polarity.
//    You don't need to do anything here. The value will be
//    shifted left accordingly when you call the initiation function */
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
//
///*     This bit is only used in TIM1 - just keep it zero */
//    TIM_OCInitStructure.TIM_OCNPolarity = (uint16_t) 0;
//
///*    This bit is only used in TIM1 - just keep it zero */
//    TIM_OCInitStructure.TIM_OCIdleState = (uint16_t) 0;
//
///*     This bit is only used in TIM1 - just keep it zero */
//    TIM_OCInitStructure.TIM_OCNIdleState = (uint16_t) 0;
//
///*     TODO: Call the corresponding TIM_OCxInit() function.
//                Remember to include all parameters and to
//                dereference your struct pointer parameter. */
//    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
//
///*    TODO: Finally let's enable the TIM3 counter */
//    TIM3->CR1 |= TIM_CR1_CEN;
//
//}

void TIM2_IRQHandler() {
 TIM2->SR &= ~(0x1); // Clear Status Register
 GPIOE->ODR ^= GPIO_ODR_ODR_2; // Toggle pulse
  //TIM_ClearITPendingBIT(TIM2, TIM_IT_Update);

}

void SysTick_Handler(void) {
 // Write the ADC value into the current array
 if(whichArray==1) {
 dataArray1[arrayPosition] = ADC1->DR;
 }
 else if(whichArray==2) {
 dataArray2[arrayPosition] = ADC1->DR;
 }

 arrayPosition++; // Increment arrayPosition
 if(arrayPosition==1024) {
 arrayPosition  = 0; // Reset array position

  // Set whichArray to fill the other array
  if(whichArray==1) {
  whichArray = 2;
  }
  else if(whichArray==2) {
  whichArray = 1;
  }

  // Set the flag to indicate the we are ready to process an array
  arrayComplete = 1;
 }

 // Signal/Trigger one ADC conversion
 ADC1->CR2 |= ADC_CR2_SWSTART;

}

void Timer2_init() {
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2->PSC = (SystemCoreClock/1000000) - 1;
  TIM2->ARR = 1000000/1000; // 1000 Hz desired frequency
  TIM2->DIER |= TIM_DIER_UIE;
  TIM2->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(TIM2_IRQn);
  NVIC_SetPriority(TIM2_IRQn,1);
}

void outputNote(int16_t tone) {
//TODO: put audio output here!
}

//int16_t processNote(int16_t note) {
//	if(note == primo[index]){
//		//outputNote(secondo[index]);
//		//trace_printf("Right note! index: %d\n", index);
//		return -1;//do nothing
//	}
//	else if(note == primo[index+1]){
//		index++;
//		return secondo[index];
//	}
//	else{
//		//outputNote(-1);//for no tone
//		return -2;//silence!
//	}
//}

int16_t processNote(int16_t note) {
 if(note == primo[index]) {
  //index++;
  //return secondo[index];
  //outputNote(secondo[index]);
  //trace_printf("Right note! index: %d\n", index);
  return -1;
 }
 else if(note == primo[index+1]){
	 index++;
	 return secondo[index];
 }
 else {
  return -1;
  //outputNote(-1);//for no tone
 }
}

void Output_init() {
 //Initialize PE2 for audio out wave
 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // Enable peripheral clock to GPIOE
 GPIOE->MODER |= GPIO_MODER_MODER2_0; // Set PE2 to general purpose output mode
 GPIOE->OTYPER &= ~(GPIO_OTYPER_OT_2); // Set to push-pull mode
 GPIOE->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR2); // Set to low speed
 GPIOE->PUPDR &= ~(GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR2_1); // Set to no pull-up/down
}



int
main(int argc, char* argv[]) {

 volatile int16_t note = -1;

 Output_init();
 ADC_init(); // Initialize the ADC
 Timer2_init(); // Initialize Tim2

 while (1) {
  // Check if a data array is ready for FFT processing
  if(arrayComplete==1){
  arrayComplete = 0; // Reset arrayComplete

   // Send the other array out for processing
   if(whichArray==1){
    int16_t out = fix_fftr(dataArray2,10,0);
    // for(int i = 512; i < 1024; i++){
    // trace_printf("%d, ", dataArray2[i]);
    // }
    note = getNote(dataArray2);
   }
   else if(whichArray==2) {
    int16_t out = fix_fftr(dataArray1,10,0);
    note = getNote(dataArray1);
   }

   int16_t selectedNote = processNote(note);

   switch(selectedNote) {
    case -2:
     // Pause (actually play a frequency so high that no one but dogs can hear it)
     TIM2->ARR = 1000000/40000; // Set frequency (40 kHz)
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case -1:
     // Continue playing the same note
     break;
    case 0:
     // Generate C4 262 Hz
     TIM2->ARR = 1000000/262/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 1:
     // Generate C#4 277 Hz
     TIM2->ARR = 1000000/277/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 2:
     // Generate D4 294 Hz
     TIM2->ARR = 1000000/294/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 3:
     // Generate D#4 311 Hz
     TIM2->ARR = 1000000/311/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 4:
     // Generate E4 330 Hz
     TIM2->ARR = 1000000/330/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 5:
     // Generate F4 349 Hz
     TIM2->ARR = 1000000/349/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 6:
     // Generate F#4 370 Hz
     TIM2->ARR = 1000000/370/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 7:
     // Generate G4 392 Hz
     TIM2->ARR = 1000000/392/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 8:
     // Generate G#4 415 Hz
     TIM2->ARR = 1000000/415/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 9:
     // Generate A4 440 Hz
     TIM2->ARR = 1000000/440/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 10:
     // Generate A# 466 Hz
     TIM2->ARR = 1000000/466/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 11:
     // Generate B4 494 Hz
     TIM2->ARR = 1000000/494/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 12:
     // Generate C5 523 Hz
     TIM2->ARR = 1000000/523/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    case 13:
     // Generate C#6 554 Hz
     TIM2->ARR = 1000000/554/4; // Set frequency
     //TIM2->CCR3 = TIM2->ARR/2; // Set duty cycle
     break;
    default:
    	break;
   }
  }
 }
}
