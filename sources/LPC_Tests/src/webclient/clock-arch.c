
#include <util.h>
#include "timer.h"

/*
 * @brief
 */
int clock_time(void){
	return  TIME_GetValue();
}

int clock_elapsed(struct timer *t){
	return clock_time() - t->start;
}

