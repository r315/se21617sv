#if 1
#include <string.h>

#include "uip.h"
#include "uiplib.h"
#include "resolv.h"
#include "tapdev.h"
#include "web.h"

#define WEBCLIENT_TIMEOUT 100

#define WEBCLIENT_STATE_STATUSLINE 0
#define WEBCLIENT_STATE_HEADERS    1
#define WEBCLIENT_STATE_DATA       2
#define WEBCLIENT_STATE_CLOSE      3

#define HTTPFLAG_NONE   0
#define HTTPFLAG_OK     1
#define HTTPFLAG_MOVED  2
#define HTTPFLAG_ERROR  3


#define ISO_nl       0x0a
#define ISO_cr       0x0d
#define ISO_space    0x20


static struct webclient_state s;

/*-----------------------------------------------------------------------------------*/
char *
webclient_mimetype(void)
{
  return s.mimetype;
}
/*-----------------------------------------------------------------------------------*/
char *
webclient_filename(void)
{
  return s.file;
}
/*-----------------------------------------------------------------------------------*/
char *
webclient_hostname(void)
{
  return s.host;
}
/*-----------------------------------------------------------------------------------*/
unsigned short
webclient_port(void)
{
  return s.port;
}
/*-----------------------------------------------------------------------------------*/
void
webclient_init(void)
{

}
/*-----------------------------------------------------------------------------------*/
static void init_connection(void)
{
  s.state = WEBCLIENT_STATE_STATUSLINE;

  s.getrequestleft = sizeof(http_get) - 1 + 1 +
    sizeof(http_10) - 1 +
    sizeof(http_crnl) - 1 +
    sizeof(http_host) - 1 +
    sizeof(http_crnl) - 1 +
    strlen(http_user_agent_fields) +
    strlen(s.file) + strlen(s.host);
  s.getrequestptr = 0;

  s.httpheaderlineptr = 0;
}
/*-----------------------------------------------------------------------------------*/
void
webclient_close(void)
{
  s.state = WEBCLIENT_STATE_CLOSE;
}
/*-----------------------------------------------------------------------------------*/
unsigned char webclient_get(char *host, u16_t port, char *file)
{
  struct uip_conn *conn;
  uip_ipaddr_t *ipaddr;
  static uip_ipaddr_t addr;

  /* First check if the host is an IP address. */
  ipaddr = &addr;
  if(uiplib_ipaddrconv(host, (unsigned char *)addr) == 0) {
    ipaddr = (uip_ipaddr_t *)resolv_lookup(host);

    if(ipaddr == NULL) {
      return 0;
    }
  }

  conn = uip_connect(ipaddr, htons(port));

  if(conn == NULL) {
    return 0;
  }

  s.port = port;
  strncpy(s.file, file, sizeof(s.file));
  strncpy(s.host, host, sizeof(s.host));

  init_connection();
  return 1;
}
/*-----------------------------------------------------------------------------------*/
static unsigned char *
copy_string(unsigned char *dest,
	    const unsigned char *src, unsigned char len)
{
  strncpy(dest, src, len);
  return dest + len;
}
/*-----------------------------------------------------------------------------------*/
static void
senddata(void)
{
  u16_t len;
  char *getrequest;
  char *cptr;

  if(s.getrequestleft > 0) {
    cptr = getrequest = (char *)uip_appdata;

    cptr = copy_string(cptr, http_get, sizeof(http_get) - 1);
    cptr = copy_string(cptr, s.file, strlen(s.file));
    *cptr++ = ISO_space;
    cptr = copy_string(cptr, http_10, sizeof(http_10) - 1);

    cptr = copy_string(cptr, http_crnl, sizeof(http_crnl) - 1);

    cptr = copy_string(cptr, http_host, sizeof(http_host) - 1);
    cptr = copy_string(cptr, s.host, strlen(s.host));
    cptr = copy_string(cptr, http_crnl, sizeof(http_crnl) - 1);

    cptr = copy_string(cptr, http_user_agent_fields,
		       strlen(http_user_agent_fields));

    len = s.getrequestleft > uip_mss()?
      uip_mss():
      s.getrequestleft;
    uip_send(&(getrequest[s.getrequestptr]), len);
  }
}
/*-----------------------------------------------------------------------------------*/
static void
acked(void)
{
  u16_t len;

  if(s.getrequestleft > 0) {
    len = s.getrequestleft > uip_mss()?
      uip_mss():
      s.getrequestleft;
    s.getrequestleft -= len;
    s.getrequestptr += len;
  }
}
/*-----------------------------------------------------------------------------------*/
static u16_t
parse_statusline(u16_t len)
{
  char *cptr;

  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
    s.httpheaderline[s.httpheaderlineptr] = *(char *)uip_appdata;
    ++((char *)uip_appdata);
    --len;
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {

      if((strncmp(s.httpheaderline, http_10,
		  sizeof(http_10) - 1) == 0) ||
	 (strncmp(s.httpheaderline, http_11,
		  sizeof(http_11) - 1) == 0)) {
	cptr = &(s.httpheaderline[9]);
	s.httpflag = HTTPFLAG_NONE;
	if(strncmp(cptr, http_200, sizeof(http_200) - 1) == 0) {
	  /* 200 OK */
	  s.httpflag = HTTPFLAG_OK;
	} else if(strncmp(cptr, http_301, sizeof(http_301) - 1) == 0 ||
		  strncmp(cptr, http_302, sizeof(http_302) - 1) == 0) {
	  /* 301 Moved permanently or 302 Found. Location: header line
	     will contain thw new location. */
	  s.httpflag = HTTPFLAG_MOVED;
	} else {
	  s.httpheaderline[s.httpheaderlineptr - 1] = 0;
	}
      } else {
	uip_abort();
	webclient_aborted();
	return 0;
      }

      /* We're done parsing the status line, so we reset the pointer
	 and start parsing the HTTP headers.*/
      s.httpheaderlineptr = 0;
      s.state = WEBCLIENT_STATE_HEADERS;
      break;
    } else {
      ++s.httpheaderlineptr;
    }
  }
  return len;
}
/*-----------------------------------------------------------------------------------*/
static char
casecmp(char *str1, const char *str2, char len)
{
  static char c;

  while(len > 0) {
    c = *str1;
    /* Force lower-case characters. */
    if(c & 0x40) {
      c |= 0x20;
    }
    if(*str2 != c) {
      return 1;
    }
    ++str1;
    ++str2;
    --len;
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
static u16_t
parse_headers(u16_t len)
{
  char *cptr;
  static unsigned char i;

  while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) {
    s.httpheaderline[s.httpheaderlineptr] = *(char *)uip_appdata;
    ++((char *)uip_appdata);
    --len;
    if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) {
      /* We have an entire HTTP header line in s.httpheaderline, so
	 we parse it. */
      if(s.httpheaderline[0] == ISO_cr) {
	/* This was the last header line (i.e., and empty "\r\n"), so
	   we are done with the headers and proceed with the actual
	   data. */
	s.state = WEBCLIENT_STATE_DATA;
	return len;
      }

      s.httpheaderline[s.httpheaderlineptr - 1] = 0;
      /* Check for specific HTTP header fields. */
      if(casecmp(s.httpheaderline, http_content_type,
		     sizeof(http_content_type) - 1) == 0) {
	/* Found Content-type field. */
	cptr = strchr(s.httpheaderline, ';');
	if(cptr != NULL) {
	  *cptr = 0;
	}
	strncpy(s.mimetype, s.httpheaderline +
		sizeof(http_content_type) - 1, sizeof(s.mimetype));
      } else if(casecmp(s.httpheaderline, http_location,
			    sizeof(http_location) - 1) == 0) {
	cptr = s.httpheaderline +
	  sizeof(http_location) - 1;

	if(strncmp(cptr, http_http, 7) == 0) {
	  cptr += 7;
	  for(i = 0; i < s.httpheaderlineptr - 7; ++i) {
	    if(*cptr == 0 ||
	       *cptr == '/' ||
	       *cptr == ' ' ||
	       *cptr == ':') {
	      s.host[i] = 0;
	      break;
	    }
	    s.host[i] = *cptr;
	    ++cptr;
	  }
	}
	strncpy(s.file, cptr, sizeof(s.file));
	/*	s.file[s.httpheaderlineptr - i] = 0;*/
      }


      /* We're done parsing, so we reset the pointer and start the
	 next line. */
      s.httpheaderlineptr = 0;
    } else {
      ++s.httpheaderlineptr;
    }
  }
  return len;
}
/*-----------------------------------------------------------------------------------*/
static void
newdata(void)
{
  u16_t len;

  len = uip_datalen();

  if(s.state == WEBCLIENT_STATE_STATUSLINE) {
    len = parse_statusline(len);
  }

  if(s.state == WEBCLIENT_STATE_HEADERS && len > 0) {
    len = parse_headers(len);
  }

  if(len > 0 && s.state == WEBCLIENT_STATE_DATA &&
     s.httpflag != HTTPFLAG_MOVED) {
    webclient_datahandler((char *)uip_appdata, len);
  }
}
/*-----------------------------------------------------------------------------------*/
void WEB_appcall(void)
{
  if(uip_connected()) {
    s.timer = 0;
    s.state = WEBCLIENT_STATE_STATUSLINE;
    senddata();
    webclient_connected();
    return;
  }

  if(s.state == WEBCLIENT_STATE_CLOSE) {
    webclient_closed();
    uip_abort();
    return;
  }

  if(uip_aborted()) {
    webclient_aborted();
  }
  if(uip_timedout()) {
    webclient_timedout();
  }


  if(uip_acked()) {
    s.timer = 0;
    acked();
  }
  if(uip_newdata()) {
    s.timer = 0;
    newdata();
  }
  if(uip_rexmit() ||
     uip_newdata() ||
     uip_acked()) {
    senddata();
  } else if(uip_poll()) {
    ++s.timer;
    if(s.timer == WEBCLIENT_TIMEOUT) {
      webclient_timedout();
      uip_abort();
      return;
    }
        /*    senddata();*/
  }

  if(uip_closed()) {
    if(s.httpflag != HTTPFLAG_MOVED) {
      /* Send NULL data to signal EOF. */
      webclient_datahandler(NULL, 0);
    } else {
      if(resolv_lookup(s.host) == NULL) {
	resolv_query(s.host);
      }
      webclient_get(s.host, s.port, s.file);
    }
  }
}


void WEB_Init(void){
	webclient_init();
	resolv_init();
	uip_ipaddr(ipaddr, 195,54,122,204);  //local adress
	resolv_conf(ipaddr);
	resolv_query("www.sics.se");
}


/*---------------------------------------------------------------------------*/
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
		/*    webclient_get("www.sics.se", 80, "/~adam/uip");*/
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
	WEB_log("Webclient: got %d bytes of data.\n", len);
}



#endif
