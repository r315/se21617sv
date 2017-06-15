//#include <clock-arch.h>

#include <timer.h>
#include <util.h>

int clock_time(void){
	return  TIME_GetValue();
}

int clock_elapsed(struct timer *t){
	return clock_time() - t->start;
}

