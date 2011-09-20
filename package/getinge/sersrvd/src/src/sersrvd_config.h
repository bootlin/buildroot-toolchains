/*!***************************************************************************
*!
*! FILE NAME  : sersrvd_config.h
*!
*! DESCRIPTION: Configuration of sersrvd
*!
*! FUNCTIONS  : sersrvd_config_read
*! (EXPORTED)   sersrvd_config_free
*!              set_initial_config
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Aug 19 1999  Johan Adolfsson    Initial version
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: sersrvd_config.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef SERSRVD_CONFIG_H
#define SERSRVD_CONFIG_H

/****************** INCLUDE FILES SECTION ***********************************/
#include "serinit.h"
/****************** CONSTANT AND MACRO SECTION ******************************/

/****************** TYPE DEFINITION SECTION *********************************/

#if 0 /* This is the original TIM1 code */
typedef struct webctrl_config_type
{
  uword mode;                    /* the configured mode */
  bool def_echo_interpret_mode;  /* The default value */
  bool echo_interpret_mode;      /* local copy */
  bool def_handle_telnet_option; /* The default value */
  bool handle_telnet_option;     /* local copy */
  uword par_base;
  char* rx_init;
  char* tx_init;

  char* def_close_str;
  byte* decoded_close_str; /* for fast access */
  uword decoded_close_str_len;

  char* vsp_server;
  uword vsp_port;
  uword idle_timeout; /* timeout in seconds */
  char* user_pass;

  char* before_connect_msg;
  char* connect_ok_msg;
  char* connect_failed_msg;
  char* connect_close_msg;
  char* client_msg;
  char* server_msg;
  char* poll_string;
  uword poll_timer; /* in 1/10 th seconds */

  bool webctrl_clear_RTS_when_data_sent; /* used for half duplex com. */
  bool webctrl_set_RTS_when_data_pending; /* used for half duplex com. */

/*  bool use_http_proxy;
  char http_proxy[22];*/ /* 123.123.123.123:65000 */

}webctrl_config_type;
#endif

struct sersrvd_config_type;
struct buffered_fd;
typedef struct sersrvd_config_type
{
  port_mode_type port_mode;
  char device[MAX_DEVICE_LEN + 1];       /* e.g. "dev/ttyS0" */
  char port_name[MAX_PORTNAME_LEN + 1];  /* e.g. "COM1" */
  unsigned long baudrate;

  unsigned char datasize;
  unsigned char parity;
  unsigned char stopsize;
  unsigned char flowcontrol_rtscts;

  unsigned char flowcontrol_xonxoff;
  unsigned char flowcontrol_dtrdsr;
  unsigned char rs485mode; /* Half-duplex mode */
  unsigned short listener_port;
  char *connect_to; /* allocated by config */

  unsigned long timeout;
  int num_allowed_connections;
  int num_connections;
  int use_telnet_options;
  int echo_command_mode_net;
  int echo_command_mode_port;
  char *allowed_users;        /* user1 user2, user3;user4 */
  char *allowed_ip_addresses; /* 1.2.3.4 1.2.3.5-8, 1.3.5.*; 2.3.4.5 */
  
  struct buffered_fd *portbfd;
  struct sersrvd_config_type *next;
} sersrvd_config_type;


/****************** GLOBAL VARIABLE DECLARATION SECTION *******************/

extern sersrvd_config_type *sersrvd_config_cbs;

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/

/* Free all config */
void sersrvd_config_free(void);

int sersrvd_config_read(const char *configfile);

void set_initial_config(sersrvd_config_type *config);


#endif /* SERSRVD_CONFIG_H */
/****************** END OF FILE sersrvd_config.h ****************************/
