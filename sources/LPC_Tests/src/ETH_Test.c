#include <eth.h>

uint8_t msg[]={0xD0,0x27,0x88,0x0C,0x25,0x73,0xAA,0x55,0xAA,0x55,0xAA,0x55,0x08,0x00};
uint8_t pk[ETH_FRAG_SIZE];


void ETH_PrintPacket(void *pk, uint32_t len){
uint32_t i;
    while(len){
        printf("%4X:",i);
        for(i=0; i < 16 && len > 0;i++){
            printf(" %X",pk++);
            len--;
        }
        printf("\n");
    }
}

void ETH_Test(void){

	ETH_Send(msg, sizeof(msg));
	if(ETH_Read(pk))
		ETH_PrintPacket(pk,16);

}
