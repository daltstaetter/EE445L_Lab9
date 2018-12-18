// Timer.c
#include "lm3s1968.h"
#include "Timer.h"
#include "rit128x96x4.h"
#include <stdio.h>
#include "Output.h"
#include "calib.h"

extern void Delay(unsigned long ulCount);
extern unsigned short plotPoints[100];

#define CALIBRATION_OFFSET 30

// This debug function initializes Timer0A to request interrupts
// at a 10 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void)
{
  volatile unsigned long delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0;// activate timer0
  delay = SYSCTL_RCGC1_R;          // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAPR_R = 249;             // prescale value for 10us
  TIMER0_TAILR_R = 999;           // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 16-bit, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R |= NVIC_EN0_INT19;    // enable interrupt 19 in NVIC
}
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
void Timer0A_Handler(void)
{
	static int i = 0;
	static int k = 0;
	unsigned short interpData = 0;
	DisableInterrupts();
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  //PG2 = 0x04;                           // profile
  ADCvalue = ADC_InSeq3();
	ADC2Temp(ADCvalue, &i);
	interpData = interpolate(ADCvalue,i) + CALIBRATION_OFFSET;
	plotPoints[k] = interpData;
	RIT128x96x4UDecOut4(ADCvalue,50,10,12);
	RIT128x96x4FixOut2(interpData,75, 10, 15);
	
	RIT128x96x4PlotPoint(plotPoints[k]); 
	RIT128x96x4PlotNext(); // called 108 times
	RIT128x96x4ShowPlot();
	RIT128x96x4UDecOut4(ADCvalue,50,10,12);
	RIT128x96x4FixOut2(interpData,75, 10, 15);
	
	Delay(100000);
	PG2 ^= 0xFF;

	k++;
	if(k == 100)
	{
		k = 0;
		RIT128x96x4PlotReClear();
	}
	
	EnableInterrupts();
}

