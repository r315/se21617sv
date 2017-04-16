#if defined(__USE_CMSIS)
#include <LPC17xx.h>
#else
#include <lpc1768.h>
#endif

#include <pwm.h>

#ifndef _EMU_

void PWM_Init(uint32_t tcclk){
	LPC_SC->PCONP |= PCPWM1;
	
	//LPC_SC->PCLKSEL0 &= ~(PCLK8<<PCLK_PWM1);
	LPC_SC->PCLKSEL0 |= (PCLK2<<PCLK_PWM1);
		
	LPC_PINCON->PINSEL4 |= 0x55; // Select PWM function for P2.3-0
	
	LPC_PWM1->TCR = (1<<TCR_TCEN) | (1<<TCR_PWMEN);
	
	LPC_PWM1->PR = 50; //1Khz
	
	LPC_PWM1->MCR = (1<<PWMMR0R); // reset TC on MR0 match	
		
	LPC_PWM1->MR0 = tcclk;  //set all outputs
	
	/*LPC_PWM1->MR0 = 100;
	LPC_PWM1->MR1 = 50;
	LPC_PWM1->MR2 = 50;
	LPC_PWM1->MR3 = 50;
	LPC_PWM1->MR4 = 50;
	LPC_PWM1->LER = 0x1F;*/
	
	LPC_PWM1->PCR = (0x0F<<PWMENA1); //enable pwm, single edge	
}

void PWM_Set(uint8_t channel, uint32_t duty){
	switch(channel){
		case PWM_1: LPC_PWM1->MR1 = duty; break;
		case PWM_2: LPC_PWM1->MR2 = duty; break;
		case PWM_3: LPC_PWM1->MR3 = duty; break;
		case PWM_4: LPC_PWM1->MR4 = duty; break;
		default:
			break;
	}
	LPC_PWM1->LER = (1<<channel);
}

#else
void PWM_Init(uint32_t tcclk){}
void PWM_Set(uint8_t channel,uint32_t duty){}
#endif
