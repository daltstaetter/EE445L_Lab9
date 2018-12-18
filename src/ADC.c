// ADC.c

#include "ADC.h"
#include "lm3s1968.h"

volatile unsigned long ADCvalue = 0;;

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

// 20-40
//---------------------------------------------------------------------------------------	 
unsigned short const ADCdata[SIZE]={0,4,8,21,34,48,62,76,90,104,119,					
     134,150,165,181,198,214,231,249,266,284,					
     302,321,340,359,379,399,419,440,462,483,					
     505,528,551,574,598,623,647,673,699,725,					
     752,779,807,836,865,895,925,956,987,1020,1023,1024};					


// 0-40		 
//unsigned short const Tdata[SIZE]={
//	4000,4000,3920,3840,3760,3680,3600,3520,3440,3360,3280,		
//     3200,3120,3040,2960,2880,2800,2720,2640,2560,2480,		
//     2400,2320,2240,2160,2080,2000,1920,1840,1760,1680,		
//     1600,1520,1440,1360,1280,1200,1120,1040,960,880,		
//     800,720,640,560,480,400,320,240,160,80,0,0};		

//20-40
//---------------------------------------------------------------------------------------	 
unsigned short const Tdata[SIZE]={4000,4000,3960,3920,3880,3840,3800,3760,3720,3680,3640,		
     3600,3560,3520,3480,3440,3400,3360,3320,3280,3240,		
     3200,3160,3120,3080,3040,3000,2960,2920,2880,2840,		
     2800,2760,2720,2680,2640,2600,2560,2520,2480,2440,		
     2400,2360,2320,2280,2240,2200,2160,2120,2080,2040,2000,2000};		


//0-40		 
//unsigned short const Rdata[SIZE]={
//	   512,512,530,548,567,587,608,629,652,675,700,725,
//	   751,779,808,838,869,901,935,971,1008,1046,1086,
//	   1128,1172,1218,1266,1316,1368,1423,1480,1540,1603,
//     1668,1737,1809,1884,1963,2046,2132,2223,2318,2418,
//     2523,2633,2748,2870,2997,3131,3271,3419,3574,3574};			
//20-40
//---------------------------------------------------------------------------------------	 
unsigned short const Rdata[SIZE]={517,517,526,534,543,552,561,570,580,589,599,								
     609,619,630,640,651,662,673,685,697,708,								
     721,733,746,759,772,785,799,813,827,841,								
     856,871,887,903,919,935,952,969,986,1004,								
     1022,1041,1060,1079,1099,1119,1140,1161,1182,1204,1226,1226};				
//---------------------------------------------------------------------------------------
// There are many choices to make when using the ADC, and many
// different combinations of settings will all do basically the
// same thing.  For simplicity, this function makes some choices
// for you.  When calling this function, be sure that it does
// not conflict with any other software that may be running on
// the microcontroller.  Particularly, ADC sample sequencer 3
// is used here because it only takes one sample, and only one
// sample is absolutely needed.  Sample sequencer 3 generates a
// raw interrupt when the conversion is complete, but it is not
// promoted to a controller interrupt.  Software triggers the
// ADC conversion and waits for the conversion to finish.  If
// somewhat precise periodic measurements are required, the
// software trigger can occur in a periodic interrupt.  This
// approach has the advantage of being simple.  However, it does
// not guarantee real-time.
//
// A better approach would be to use a hardware timer to trigger
// the ADC conversion independently from software and generate
// an interrupt when the conversion is finished.  Then, the
// software can transfer the conversion result to memory and
// process it after all measurements are complete.

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: programmable using variable 'channelNum' [0:7]
// SS3 interrupts: enabled but not promoted to controller
//---------------------------------------------------------------------------------------	 

void ADC_InitSWTriggerSeq3(unsigned char channelNum)
{
  if(channelNum > 7)
	{
    return;   // 0 to 7 are valid channels on the LM3S1968
  }
	
  SYSCTL_RCGC0_R |= 0x00010000;   // 1) activate ADC clock
  SYSCTL_RCGC0_R &= ~0x00000300;  // 2) configure for 125K
  ADC_SSPRI_R = 0x3210;           // 3) Sequencer 3 is lowest priority
  ADC_ACTSS_R &= ~0x0008;         // 4) disable sample sequencer 3
  ADC_EMUX_R &= ~0xF000;          // 5) seq3 is software trigger
  ADC_SSMUX3_R &= ~0x0007;        // 6) clear SS3 field
  ADC_SSMUX3_R += channelNum;     //    set channel
  ADC_SSCTL3_R = 0x0006;          // 7) no TS0 D0, yes IE0 END0
  ADC_IM_R &= ~0x0008;            // 8) disable SS3 interrupts
  ADC_ACTSS_R |= 0x0008;          // 9) enable sample sequencer 3
}
//---------------------------------------------------------------------------------------	 
//------------ADC_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 10-bit result of ADC conversion
unsigned long ADC_InSeq3(void)
{
	unsigned long result;
  ADC_PSSI_R = 0x0008;             // 1) initiate SS3
  while((ADC_RIS_R&0x08)==0){};    // 2) wait for conversion done
  result = ADC_SSFIFO3_R&0x3FF;    // 3) read result
  ADC_ISC_R = 0x0008;              // 4) acknowledge completion
  return result;
}
//---------------------------------------------------------------------------------------	 
unsigned short ADC2Temp(unsigned short adcSample, int* index)
{
	int i;
	
	for(i = 0; i < SIZE; i++)
	{
		if(adcSample < ADCdata[i])
		{
			break;
		}
	}	
	*index = i-1;
	return Tdata[i];
}
//---------------------------------------------------------------------------------------	 
unsigned short interpolate(unsigned short rawADC, int i)
{
	int deltaADC;
	int deltaT;
	int scaleADC;
	int percentChange;
	int tempDiff;
	int newTemp;
	
	deltaADC = ADCdata[i+1] - ADCdata[i];
	scaleADC = rawADC - ADCdata[i]; // should be a + number always
	percentChange = (scaleADC*1000+(deltaADC/2))/deltaADC;
	
	deltaT = Tdata[i] - Tdata[i+1];
	tempDiff = (deltaT*percentChange+500)/1000;
	newTemp = Tdata[i]-tempDiff;
	
	return newTemp;
}



