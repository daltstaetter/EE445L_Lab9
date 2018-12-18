// PLL.c
#include "PLL.h"


void PLL_Init(void)
{    
  // program 4.6 volume 1
  // 1) bypass PLL and system clock divider while initializing
  SYSCTL_RCC_R |=  0x00000800;   // PLL bypass
  SYSCTL_RCC_R &= ~0x00400000;   // do not use system divider
  // 2) select the crystal value and oscillator source
  SYSCTL_RCC_R &= ~0x000003C0;   // clear XTAL field, bits 9-6
  SYSCTL_RCC_R +=  0x00000380;   // 0x0E, configure for 8 MHz crystal
  SYSCTL_RCC_R &= ~0x00000030;   // clear oscillator source field
  SYSCTL_RCC_R +=  0x00000000;   // configure for main oscillator source
  // 3) activate PLL by clearing PWRDN and OEN
  SYSCTL_RCC_R &= ~(0x00002000|0x00001000);
  // 4) set the desired system divider and the USESYSDIV bit
  SYSCTL_RCC_R &= ~0x07800000;   // system clock divider field
  SYSCTL_RCC_R +=  0x03800000;   // configure for 25 MHz clock
  SYSCTL_RCC_R |=  0x00400000;   // Enable System Clock Divider
  // 5) wait for the PLL to lock by polling PLLLRIS
  while((SYSCTL_RIS_R&0x00000040)==0){};  // wait for PLLRIS bit
  // 6) enable use of PLL by clearing BYPASS
  SYSCTL_RCC_R &= ~0x00000800;
}




