#include "dhcp_server.h"
#include <eth.h>
#include <time.h>
#include <uip.h>


static uip_ipaddr_t remote_ip;

static struct dhcpc_state server_state;

// echo "hello" >/dev/udp/169.254.9.161/68
void dhcp_server_init(void){
uip_ipaddr_t addr;
	  server_state.mac_addr = ETH_GetIF_MAC();
	  server_state.mac_len  = 6;

	  server_state.state = STATE_INITIAL;

	  uip_ipaddr(addr,0,0,0,0);

	  server_state.conn = uip_udp_new(&addr, HTONS(DHCPC_CLIENT_PORT));

	  if(server_state.conn != NULL) {
	    uip_udp_bind(server_state.conn, HTONS(DHCPC_SERVER_PORT));
	  }

	  PT_INIT(&server_state.pt);
}

void dhcp_server_send_offer(void){

}

void dhcp_server_send_acknowledge(void){

}

void dhcpc_server_appcall(void){



}

#ifdef DHCP_LOG
void dhcp_server_log(char *log){
	printf("DHCP_Server: %s\n",log);
}
#endif

uint8_t dhcp_server_input(void)
{
struct uip_udp_conn *c = (struct uip_udp_conn *)(uip_buf+30);
/*
	if(c->rport == HTONS(DHCPC_SERVER_PORT) &&
			uip_ipaddr_cmp(c->ripaddr,all_ones_addr) ){
		DHCP_LOG("Discovery received");
	}

/*
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

  if(m->op == DHCP_REPLY &&
     memcmp(m->xid, xid, sizeof(xid)) == 0 &&
     memcmp(m->chaddr, s.mac_addr, s.mac_len) == 0) {
    memcpy(s.ipaddr, m->yiaddr, 4);
    return parse_options(&m->options[4], uip_datalen());
  }*/
  return 0;
}
