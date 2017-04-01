#include <util.h>   

uint16_t rand(void){
	return ((TIME_GetValue() ^ 0x1244654) >> (TIME_GetValue() ^ 0xa)) %0xFFFF ;
}
