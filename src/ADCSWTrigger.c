// ADCSWTrigger.c
// Runs on LM3S1968
// Provide functions that initialize ADC SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Daniel Valvano
// May 21, 2012

/* This example accompanies the book
   "Embedded Systems: Interfacing to the Arm Cortex M3",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2012

 Copyright 2012 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdio.h>
#include "lm3s1968.h"
#include "Output.h"
#include "rit128x96x4.h"
#include "driverlib/adc.h"
#include "ADC.h"
#include "PLL.h"
#include "Timer.h"
/*
//unsigned short const ADCdata[SIZE]={
//	   0,2,27,53,79,107,135,165,196,228,262,							
//     296,332,370,409,449,491,535,581,628,677,							
//     728,781,837,894,954,1016,1023,1023,1023,1023,							
//     1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,							
//     1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1024};	

//		 unsigned short const ADCdata[SIZE]={
//	   0,2,27,53,79,107,135,165,196,228,262,							
//     296,332,370,409,449,491,535,581,628,677,							
//     728,781,837,894,954,1016,1,2,3,4,							
//     5,6,7,8,9,10,11,12,13,14,							
//     15,16,17,18,19,20,21,22,23,24,25,1024};	

//////// 20-40
//////unsigned short const ADCdata[53]={0,4,8,21,34,48,62,76,90,104,119,					
//////     134,150,165,181,198,214,231,249,266,284,					
//////     302,321,340,359,379,399,419,440,462,483,					
//////     505,528,551,574,598,623,647,673,699,725,					
//////     752,779,807,836,865,895,925,956,987,1020,1023,1024};					


// 0-40		 
//unsigned short const Tdata[SIZE]={
//	4000,4000,3920,3840,3760,3680,3600,3520,3440,3360,3280,		
//     3200,3120,3040,2960,2880,2800,2720,2640,2560,2480,		
//     2400,2320,2240,2160,2080,2000,1920,1840,1760,1680,		
//     1600,1520,1440,1360,1280,1200,1120,1040,960,880,		
//     800,720,640,560,480,400,320,240,160,80,0,0};		

////////20-40
//////unsigned short const Tdata[53]={4000,4000,3960,3920,3880,3840,3800,3760,3720,3680,3640,		
//////     3600,3560,3520,3480,3440,3400,3360,3320,3280,3240,		
//////     3200,3160,3120,3080,3040,3000,2960,2920,2880,2840,		
//////     2800,2760,2720,2680,2640,2600,2560,2520,2480,2440,		
//////     2400,2360,2320,2280,2240,2200,2160,2120,2080,2040,2000,2000};		


//0-40		 
//unsigned short const Rdata[SIZE]={
//	   512,512,530,548,567,587,608,629,652,675,700,725,
//	   751,779,808,838,869,901,935,971,1008,1046,1086,
//	   1128,1172,1218,1266,1316,1368,1423,1480,1540,1603,
//     1668,1737,1809,1884,1963,2046,2132,2223,2318,2418,
//     2523,2633,2748,2870,2997,3131,3271,3419,3574,3574};	

////////20-40
//////unsigned short const Rdata[53]={517,517,526,534,543,552,561,570,580,589,599,								
//////     609,619,630,640,651,662,673,685,697,708,								
//////     721,733,746,759,772,785,799,813,827,841,								
//////     856,871,887,903,919,935,952,969,986,1004,								
//////     1022,1041,1060,1079,1099,1119,1140,1161,1182,1204,1226,1226};								



		 

//debug code
//
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.

//void DisableInterrupts(void); // Disable interrupts
//void EnableInterrupts(void);  // Enable interrupts
//long StartCritical (void);    // previous I bit, disable interrupts
//void EndCritical(long sr);    // restore I bit to previous value
//void WaitForInterrupt(void);  // low power mode

//---------------------------------------------------------------------------------------
*/
// delay function for testing from sysctl.c
// which delays 3*ulCount cycles
#ifdef __TI_COMPILER_VERSION__
	//Code Composer Studio Code
	void Delay(unsigned long ulCount){
	__asm (	"    subs    r0, #1\n"
			"    bne     Delay\n"
			"    bx      lr\n");
}

#else
//Keil uVision Code
__asm void
Delay(unsigned long ulCount)
{
	subs    r0, #1
	bne     Delay
	bx      lr
}
#endif
//---------------------------------------------------------------------------------------
void PortG_Init(void)
{
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOG; // activate port G
	
	delay = SYSCTL_RCGC2_R;
	delay = SYSCTL_RCGC2_R;
	delay = SYSCTL_RCGC2_R;
	
	GPIO_PORTG_DIR_R |= 0x04;             // make PG2 out (built-in LED)
  GPIO_PORTG_AFSEL_R &= ~0x04;          // disable alt func on PG2
  GPIO_PORTG_DEN_R |= 0x04;             // enable digital I/O on PG2
  PG2 = 0;                              // turn off LED
  GPIO_PORTG_DATA_R |= 0x04;
	
}
//---------------------------------------------------------------------------------------

unsigned short plotPoints[100] = {2000};


int main(void)
{
  PLL_Init();      // 25 MHz Clock
	PortG_Init();    // Initialize the Heartbeat
	
	Output_Init();
  Output_Color(15);	
  Delay(4000000); 
	PG2 = 1;
	Delay(4000000); 
	PG2 = 0;
	
  ADC_InitSWTriggerSeq3(0);     // allow time to finish activating ADC0
  Timer0A_Init100HzInt();       // set up Timer0A for 100 Hz interrupts
	//ADCvalue = 0;
	
	RIT128x96x4PlotClear(2000,4000,20,27,34,40);
	EnableInterrupts();
  while(1)
	{
    WaitForInterrupt();
  }
}
//---------------------------------------------------------------------------------------
