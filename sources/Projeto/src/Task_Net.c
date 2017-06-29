#include <stdio.h>
#include <string.h>
#include "eth.h"
#include "tapdev.h"
#include "timer.h"
#include "uip-conf.h"
#include "uip.h"
#include "uiplib.h"
#include "uip_arp.h"
#include <Task_Common.h>


#define BUF ((struct uip_eth_hdr *)uip_buf)
static struct timer periodic_timer, arp_timer;


void Task_Net_Init(void){
uip_ipaddr_t ipaddr;

	uip_log("Starting");

	timer_set(&periodic_timer, CLOCK_SECOND / 2);
	timer_set(&arp_timer, CLOCK_SECOND * 10);

	uip_init();

	memcpy(&uip_ethaddr.addr, ETH_GetIF_MAC(), 6);   			//set physical address into stack
	uiplib_ipaddrconv(MICRO_IP_LOCAL_ADDR, (uint8_t *)ipaddr);	//Set Local IP address
	uip_sethostaddr(ipaddr);

	uiplib_ipaddrconv(MICRO_IP_REMOTE_ADDR, (uint8_t *)ipaddr);
	uip_setdraddr(ipaddr);							            //Set Router IP
	uip_ipaddr(ipaddr, 255, 255, 255, 0);
	uip_setnetmask(ipaddr);

	 webclient_init();
	 resolv_init();

	 MISC_PrintIp((uint32_t)uip_hostaddr);

	 uiplib_ipaddrconv(MICRO_IP_DNS_ADDR, (uint8_t *)ipaddr);
	 resolv_conf(ipaddr);										// Set DNS Server Address
	 resolv_query(RESOLVE_ADDR);								// resolve address
}

void Task_Net(void *ptr){
int i;
	Task_Net_Init();

	while (1) {
		uip_len = tapdev_read();
		if (uip_len > 0) {
			switch(BUF->type){
				case HTONS(UIP_ETHTYPE_IP):
					uip_input();
					if (uip_len > 0) {
						uip_arp_out();
						tapdev_send();
					}

					break;
				case  HTONS(UIP_ETHTYPE_ARP):
					uip_arp_arpin();
					if (uip_len > 0) {
						tapdev_send();
					}
					break;
			}
		}else {
			if (timer_expired(&periodic_timer)) {
				timer_reset(&periodic_timer);
				for (i = 0; i < UIP_CONNS; i++) {
					uip_periodic(i);
					if (uip_len > 0) {
						uip_arp_out();
						tapdev_send();
					}
				}
				#if UIP_UDP
				for(i = 0; i < UIP_UDP_CONNS; i++) {
					uip_udp_periodic(i);
					if(uip_len > 0) {
						uip_arp_out();
						tapdev_send();
					}
				}
				#endif /* UIP_UDP */

				if (timer_expired(&arp_timer)) {
					timer_reset(&arp_timer);
					uip_arp_timer();
				}
			}
		}
	}
}

void uip_log(char *m) {
#if MICRO_IP_DEBUG
	printf("uIP log message: %s\n", m);
#endif
}

void WEB_log(char *m) {
#if WEB_DEBUG
	printf("%s", m);
#endif
}

void resolv_found(char *name, u16_t *ipaddr) {

	if (ipaddr == NULL) {
		#if MICRO_IP_DEBUG
		printf("Host '%s' not found.\n", name);
		#else
		uip_log("");
		#endif
	} else {
		#if MICRO_IP_DEBUG
		printf("Found name '%s' = %d.%d.%d.%d\n", name, htons(ipaddr[0]) >> 8,
				htons(ipaddr[0]) & 0xff, htons(ipaddr[1]) >> 8,
				htons(ipaddr[1]) & 0xff);
		#else
		uip_log("");
		#endif
		//webclient_get("http://adeetc.thothapp.com", 80, "/api/v1/classes/634");
		//webclient_get("retro.hackaday.com",80,"/");
	}
}


void webclient_closed(void) {
	WEB_log("Webclient: connection closed\n");
}
void webclient_aborted(void) {
	WEB_log("Webclient: connection aborted\n");
}
void webclient_timedout(void) {
	WEB_log("Webclient: connection timed out\n");
}
void webclient_connected(void) {
	WEB_log("Webclient: connected, waiting for data...\n");
}

void webclient_datahandler(char *data, u16_t len) {
RtopScore itm;
static uint16_t idx = 0;
char c;

	if(len > 3 * MAX_TOPSCORE_LEN)		// too much data
		return;

	if(!len){
		#if WEB_DEBUG
		printf("Webclient: semaphore Given\n", len);
		#endif
		xSemaphoreGive(topscore_semaphore);
		return;
	}

#if WEB_DEBUG
	printf("Webclient: got %d bytes of data: \n", len);
#endif

	while(len--){
		c = *data++;
#if WEB_DEBUG
		putchar(c);
#endif
		if(c == '\n'){
			c = '\0';
			itm.name[ (idx) % MAX_TOPSCORE_LEN] = c;
			xQueueSendToBack(topscore_queue,&itm,0);
			idx = 0;
		}else{
			itm.name[ (idx++) % MAX_TOPSCORE_LEN] = c;
		}
	}

	itm.name[idx % MAX_TOPSCORE_LEN] = '\0';
	xQueueSendToBack(topscore_queue,&itm,0);

#if WEB_DEBUG
	putchar('\n');
#endif

}

void NET_SendScore(uint32_t score, char *pname){
	//TODO: Get server IP and implement post method
}
