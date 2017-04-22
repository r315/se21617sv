#include <eth.h>

uint8_t msg[]={"Mensagem"};

void ETH_Test(void){

	ETH_Send(msg, sizeof(msg));


}
