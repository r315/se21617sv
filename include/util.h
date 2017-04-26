#ifndef _util_h_
#define _util_h_

#ifndef ON
#define ON 1
#define OFF 0
#endif

#if defined(__LPC17XX__)
#include <time_m3.h>
#define TIMER0_GetValue() TIME_GetValue()
#define TIMER0_Elapse(x) TIME_Elapsed(x)
#define TIMER0_TicksToMs(x) x
#elif defined(__LPC2106__)
#include <timer.h>
#elif defined(__EMU__)
#include <time_emu.h>
#define TIMER0_GetValue() TIME_GetValue()
#define TIMER0_Elapse(x) TIME_Elapsed(x)
#define TIMER0_TicksToMs(x) x
#endif

uint16_t rand(void);
#endif /* _util_h_ */
