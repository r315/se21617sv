#include <stdio.h>
#include <string.h>
#include "eth.h"
#include "tapdev.h"
#include "timer.h"
#include "uip-conf.h"
#include "uip.h"
#include "uiplib.h"
#include "micro_ip.h"
#include "uip_arp.h"


#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
static struct timer periodic_timer, arp_timer;


void MICRO_IP_PrintIp(uip_ipaddr_t ip){
	LCD_WriteString("IP: ");
	LCD_WriteInt((uint8_t)(HTONS(ip[0])>>8), 10); LCD_WriteChar('.');
	LCD_WriteInt((uint8_t)HTONS(ip[0]), 10); LCD_WriteChar('.');
	LCD_WriteInt((uint8_t)(HTONS(ip[1])>>8), 10); LCD_WriteChar('.');
	LCD_WriteInt((uint8_t)HTONS(ip[1]), 10); LCD_WriteChar('\n');
}

void MICRO_IP_Init(void){
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

	 MICRO_IP_PrintIp(uip_hostaddr);

	 uiplib_ipaddrconv(MICRO_IP_DNS_ADDR, (uint8_t *)ipaddr);
	 resolv_conf(ipaddr);										//Set DNS Server Address
	 //resolv_query("www.google.com");
	 //resolv_query("http://adeetc.thothapp.com");
	 resolv_query("retro.hackaday.com");
}

void MICRO_IP_Task(void){
int i;
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
		printf("Host '%s' not found.\n", name);
	} else {
		printf("Found name '%s' = %d.%d.%d.%d\n", name, htons(ipaddr[0]) >> 8,
				htons(ipaddr[0]) & 0xff, htons(ipaddr[1]) >> 8,
				htons(ipaddr[1]) & 0xff);
		//webclient_get("http://adeetc.thothapp.com", 80, "/api/v1/classes/634");
		webclient_get("retro.hackaday.com",80,"/");
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
	printf("Webclient: got %d bytes of data.\n %s", len,data);

}
