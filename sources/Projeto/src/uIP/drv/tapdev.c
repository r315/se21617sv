#include <eth.h>
#include "uip.h"

void tapdev_init(void){
	ETH_Init();
}
void tapdev_send(void){
	ETH_Send(uip_buf, uip_len);
}
void tapdev_read(void){
	ETH_Read(uip_buf);
}

