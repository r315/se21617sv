#ifndef __web_h__
#define __web_h_

#define WEB_DEBUG 1
/*
#define WEBCLIENT_CONF_MAX_URLLEN 100

struct webclient_state {
  u8_t timer;
  u8_t state;
  u8_t httpflag;

  u16_t port;
  char host[40];
  char file[WEBCLIENT_CONF_MAX_URLLEN];
  u16_t getrequestptr;
  u16_t getrequestleft;

  char httpheaderline[200];
  u16_t httpheaderlineptr;

  char mimetype[32];
};

typedef struct webclient_state uip_tcp_appstate_t;

#define UIP_APPCALL webclient_appcall
*/

/**
 * @inicializa o modulo de interface com a web
 */
void WEB_Init(void);


#endif
