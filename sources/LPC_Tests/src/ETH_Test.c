#include <eth.h>

uint8_t msg[]={"Mensagem"};
uint8_t pk[ETH_FRAG_SIZE];
void ETH_Test(void){

	ETH_Send(msg, sizeof(msg));
	ETH_Read(pk);

}
