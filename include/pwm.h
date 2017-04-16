#ifndef _pwm_h_
#define _pwm_h_

#define PCLK1 1
#define PCLK2 2
#define PCLK4 0
#define PCLK8 3

#define PCPWM1 (1<<6)
#define PCLK_PWM1 12

#define TCR_TCEN  0
#define TCR_PWMEN 3

#define PWM_1 1
#define PWM_2 2
#define PWM_3 3
#define PWM_4 4

#define PWMMR0I 0
#define PWMMR0R 1
#define PWMMR0S 2

#define PWMMR1I 3
#define PWMMR1R 4
#define PWMMR1S 5

#define PWMMR2I 6
#define PWMMR2R 7
#define PWMMR2S 8

#define PWMMR3I 9
#define PWMMR3R 10
#define PWMMR3S 11

#define PWMMR4I 12
#define PWMMR4R 13
#define PWMMR4S 14

#define PWMENA1 9

/**
 * @brief initialyze PWM giving TC clocks for one PWM period (Ton + Toff)
 * 		ACTIVE Outputs: P2.1, P2.2, P2.3
 * */
void PWM_Init(uint32_t tcclk);

/**
 * @brief set the duty for the given pwm channel
 * */
void PWM_Set(uint8_t channel, uint32_t duty);
#endif
