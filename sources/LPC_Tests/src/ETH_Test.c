#include <eth.h>
#include <stdio.h>

uint8_t arprequest[]={
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x06, 0x23, 0x53, 0x45, 0x32, 0x23, 0x08, 0x06, 0x00, 0x01,
		0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0xd0, 0x27, 0x88, 0x0c, 0x25, 0x73, 0xc0, 0xa8, 0x00, 0xC8,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x02};

typedef struct ethernet{
	uint8_t dstMAC[6];
	uint8_t srcMAC[6];
	uint16_t type;
}Ethernet;

typedef struct arp{
	Ethernet ethernet;
	uint16_t htype;
	uint16_t ptype;
	uint8_t hsize;
	uint8_t psize;
	uint8_t opcodeh;
	uint8_t opcode;
	uint8_t srcMAC[6];
	uint8_t srcIP[4];
	uint8_t targetMAC[6];
	uint8_t targetIP[4];
}Arp;

typedef struct ip{
	Ethernet ethernet;
	//IP
	uint8_t ihl;
	uint8_t dsf;
	uint16_t len;
	uint16_t id;
	uint16_t flags;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t chksum;
	uint8_t srcIP[4];
	uint8_t dstIP[4];
	uint8_t opt[96];
}Ip	;

#define TYPE_ARP 0x0608 //0x0806 big-endian
#define TYPE_IP  0x0008 //0x0800
#define PROTOCOL_ICMP 1


void swapBytes(uint8_t *dst, uint8_t *src, uint32_t size){
uint8_t tmp;
	while(size--){
		tmp = *dst;
		*dst = *src;
		*src = tmp;
		src+=1;
		dst += 1;
	}
}

void PrintEthernetPacket(void *pk, uint32_t len){
uint8_t byte, i;

	for(i = 0; i < len; i++){
    	byte = *((uint8_t*)(pk+i));
        printf(" %.2X",byte);
    }
	printf("  ");
	for(i = 0; i < len; i++){
	    	byte = *((uint8_t*)(pk+i));
	        printf(" %c",(char)byte);
	}
    printf("\n");
}

void setMAC(uint8_t *dst){
uint8_t i;
	for(i = 0; i < 6; i++){
		dst[i]= 6 - i;
	}
}

void arpReply(Arp *arp){
	setMAC((uint8_t*)&arp->ethernet.dstMAC);
	swapBytes((uint8_t*)&arp->ethernet.dstMAC, (uint8_t*)&arp->ethernet.srcMAC, 6);
	arp->opcode = 2;  //reply
	setMAC((uint8_t*)&arp->targetMAC);
	swapBytes((uint8_t*)&arp->srcMAC, (uint8_t*)&arp->targetMAC, 6);
	swapBytes((uint8_t*)&arp->srcIP, (uint8_t*)&arp->targetIP,4);
	ETH_Send(arp,42);
	printf("ARP Reply to %u.%u.%u.%u\n",arp->targetIP[0],
			arp->targetIP[1],arp->targetIP[2],arp->targetIP[3]);
}

void icmpReply(Ip *ip, uint32_t size){
	swapBytes((uint8_t*)&ip->ethernet.dstMAC, (uint8_t*)&ip->ethernet.srcMAC, 6);
	swapBytes((uint8_t*)&ip->srcIP, (uint8_t*)&ip->dstIP,4);
	ETH_Send(ip,size);
}

//send arp
void ETH_Test_(void){
	ETH_Send(arprequest, sizeof(arprequest));
}

// ping reply
void ETH_Test(void){
uint32_t size;
uint8_t packet[256];

	size = ETH_Read(&packet);

	if(size){
		switch(((Ethernet*)&packet[0])->type){
			case TYPE_ARP:
				arpReply((Arp*)packet);
				break;

			case TYPE_IP:
				if(((Ip*)&packet[0])->protocol == PROTOCOL_ICMP){
					icmpReply((Ip*)&packet[0], size);
					break;
				}

			default:
				PrintEthernetPacket(&packet,14);
		}
	}else
		ETH_Send(arprequest, sizeof(arprequest));
}
