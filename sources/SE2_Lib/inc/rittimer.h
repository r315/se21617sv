 /** 
 @file rittimer.h
 @brief  Function prototypes for Repetitive timer driver
 @author Hugo Reis
 @date 25-01-2017
 **/
#ifndef _rittimer_h_
#define _rittimer_h_

// default PCLK_RIT = CCLK/4
// 

#define RIT_CCLK_USDIV 4000000                  // default RIT CLK = CCLK/4
#define RIT_CCLK_MSDIV 4000
#define RIT_CCLK1 LPC_SC->PCLKSEL1 = (1<<26);	// RIT CLK = CCLK/1
#define RIT_PEN (1<<16)

//RICTRL Bits
#define RITINT 1
#define RITENCLR (1<<1)
#define RITENBR  (1<<2)
#define RITEN    (1<<3)

#define RIT_ONE_MS 1000

 /**
 @brief configure rit timer for calling a handler in intervlas of
      the specified time
 **/
void RIT_InitHandler(unsigned int us, void *handler);
/**
 @brief configure rit timer as free timer counting the specified time
 **/
void RIT_Init(unsigned int us);
/**
 @brief get the current value of rit ticks counter
 **/
int RIT_GetTicks(void);

 /**
 @brief ms active wait
 @note for this function works propertly RIT must be configures to count ms
 **/
 void RIT_DelayMs(unsigned int ms);

#endif /* _rittimer_h_*/
