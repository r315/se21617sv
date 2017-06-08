#ifndef __microip_h__
#define __microip_h__

//#include <uip.h>

#define MICRO_IP_LOCAL_ADDR  "10.42.0.50"   // LPC Board Local IP address
#define MICRO_IP_REMOTE_ADDR "10.42.0.1"   // Other cable End IP
#define MICRO_IP_DNS_ADDR    "8.8.8.8"         // DNS Server

#define MICRO_IP_DEBUG 1
#define WEB_DEBUG 1

/**
 * @brief inicializa a stack uip
 */
void MICRO_IP_Init(void);

/**
 * @brief Função que deve se ser chamada periodicamente para processar ligações
 * 			http, pode ser chamada através de uma task do freeRtos
 */
void MICRO_IP_Task(void);

#endif
