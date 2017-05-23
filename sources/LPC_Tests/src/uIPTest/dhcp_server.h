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

#define UIP_UDP_APPCALL dhcpc_server_appcall

void dhcpc_server_appcall(void);

#endif
