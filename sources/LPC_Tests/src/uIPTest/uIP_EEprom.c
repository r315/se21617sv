/*
<!DOCTYPE html>
<html>

<body>
	<h2>EEprom content</h2>
	<textarea name="Page0" rows="16" cols="50" style = "resize:none">ADR: +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F 000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    </textarea>

</body>

</html>

*/


#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"

#include <lcd.h>
#include "timer.h"
#include <stdio.h>

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])


int uIP_EEprom(void){

int i;
	uip_ipaddr_t ipaddr;
	struct timer periodic_timer, arp_timer;

	timer_set(&periodic_timer, CLOCK_SECOND / 2);
	timer_set(&arp_timer, CLOCK_SECOND * 10);

	//tapdev_init();
	uip_init();

	uip_ethaddr.addr[0] = 06;
	uip_ethaddr.addr[1] = 0x23;
	uip_ethaddr.addr[2] = 0x53;
	uip_ethaddr.addr[3] = 0x45;
	uip_ethaddr.addr[4] = 0x32;
	uip_ethaddr.addr[5] = 0x23;

	uip_ipaddr(ipaddr, 169,254,9,161);
	uip_sethostaddr(ipaddr);

	uip_ipaddr(ipaddr, 169,254,9,160);
	uip_setdraddr(ipaddr);
	uip_ipaddr(ipaddr, 255, 255, 255, 0);
	uip_setnetmask(ipaddr);

	uip_listen(HTONS(80));

	UIP_UTIL_PrintIp(ipaddr);


	while (1) {
			uip_len = tapdev_read();
			if (uip_len > 0) {
				if (BUF->type == htons(UIP_ETHTYPE_IP)) {
					uip_arp_ipin();
					uip_input();
					/* If the above function invocation resulted in data that
					 should be sent out on the network, the global variable
					 uip_len is set to a value > 0. */
					if (uip_len > 0) {
						uip_arp_out();
						tapdev_send();
					}
				} else if (BUF->type == htons(UIP_ETHTYPE_ARP)) {
					uip_arp_arpin();
					/* If the above function invocation resulted in data that
					 should be sent out on the network, the global variable
					 uip_len is set to a value > 0. */
					if (uip_len > 0) {
						tapdev_send();
					}
				}

			} else if (timer_expired(&periodic_timer)) {
				timer_reset(&periodic_timer);
				for (i = 0; i < UIP_CONNS; i++) {
					uip_periodic(i);
					/* If the above function invocation resulted in data that
					 should be sent out on the network, the global variable
					 uip_len is set to a value > 0. */
					if (uip_len > 0) {
						uip_arp_out();
						tapdev_send();
					}
				}

				/* Call the ARP timer function every 10 seconds. */
				if (timer_expired(&arp_timer)) {
					timer_reset(&arp_timer);
					uip_arp_timer();
				}
			}
		}
	return 0;
}
