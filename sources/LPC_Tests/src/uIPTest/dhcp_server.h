#ifndef _dhcp_server_h_
#define _dhcp_server_h_

#include "stdint.h"
#include "timer.h"
#include "pt.h"

typedef struct dhcpc_state {
  struct pt pt;
  char state;
  struct uip_udp_conn *conn;
  struct timer timer;
  uint16_t ticks;
  const void *mac_addr;
  int mac_len;

  uint8_t serverid[4];

  uint16_t lease_time[2];
  uint16_t ipaddr[2];
  uint16_t netmask[2];
  uint16_t dnsaddr[2];
  uint16_t default_router[2];
}uip_udp_appstate_t;

struct dhcp_msg {
  uint8_t op, htype, hlen, hops;
  uint8_t xid[4];
  uint16_t secs, flags;
  uint8_t ciaddr[4];
  uint8_t yiaddr[4];
  uint8_t siaddr[4];
  uint8_t giaddr[4];
  uint8_t chaddr[16];
#ifndef UIP_CONF_DHCP_LIGHT
  uint8_t sname[64];
  uint8_t file[128];
#endif
  uint8_t options[312];
};


enum{
	STATE_INITIAL,
	STATE_SENDING,
	STATE_OFFER_RECEIVED,
	STATE_CONFIG_RECEIVED
};

#define IP_POOL 169,254,9,0

#define DHCPC_SERVER_PORT  67
#define DHCPC_CLIENT_PORT  68

#define UIP_UDP_APPCALL dhcpc_server_appcall

#define DHCP_DEBUG

#ifdef DHCP_DEBUG
#define DHCP_LOG(m) dhcp_server_log(m)
void dhcp_server_log(char *log);
#else
#define DHCP_LOG(m)
#endif


void dhcpc_server_appcall(void);
uint8_t dhcp_server_input(void);

#endif
