/*!***************************************************************************
*!
*! FILE NAME  : sersrvd_config.c
*!
*! DESCRIPTION: Configuration of sersrvd
*!
*! FUNCTIONS  : sersrvd_config_read
*! (EXTERNAL)   sersrvd_config_free
*!
*! FUNCTIONS  : <The names of the local functions in this file.>
*! (LOCAL)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Aug 19 1999  Johan Adolfsson    Initial version
*! $Log: sersrvd_config.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.15  2001/07/06 15:08:16  johana
*! Support for using RS-485 driver in kernel if it exists (and works).
*!
*! Revision 1.14  2001/07/05 13:47:02  johana
*! Added rs485mode parameter.
*!
*! Revision 1.13  2001/01/09 19:03:25  johana
*! Added set_initial_config
*!
*! Revision 1.12  2000/11/20 15:14:41  johana
*! Added num_connections and num_allowed_connections.
*! For now num_allowed_connections is set to 1 and not configurable.
*! Additional connections are closed directly.
*!
*! Revision 1.11  2000/11/14 14:33:55  johana
*! Added support for user authentication and IP number verification.
*!
*! Revision 1.10  2000/11/13 18:20:50  johana
*! Handle "value" with "'s around it.
*!
*! Revision 1.9  2000/10/05 14:05:25  johana
*! Review changes
*!
*! Revision 1.8  2000/09/18 09:54:46  johana
*! Fixed defaultvalue for parity, reread of config, debugprintouts.
*!
*! Revision 1.7  2000/09/15 17:16:04  johana
*! Handle change of Mode in config file.
*!
*! Revision 1.6  2000/09/11 11:37:20  johana
*! Handle change of config. Added flowcontrol stuff.
*!
*! Revision 1.5  2000/08/17 16:00:29  johana
*! Reduced debug printouts
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: sersrvd_config.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/

#include "sersrvd_config.h"
#include "sersrvd.h"
#include "serinit.h"
#include "serutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "bfdutil.h"
#include "listener.h"
#include "netutil.h"

/****************** CONSTANT AND MACRO SECTION ******************************/

#define D(x)
#define DCONF(x)
#define DINFO(x) if (conf_verbose){ x; }

#define MAX_GROUP_LENGTH 100
#define MAX_LINE_LENGTH 512

/****************** TYPE DEFINITION SECTION *********************************/

/* FIXME - That asm/termios.h stuff in the kernel needs to be fixed. Until then
 * create 2 dummy structures here in order to make this program compile with
 * uC-libc Beta2.
 */
struct rs485_ctrl {
	unsigned short rts_on_send;
	unsigned short rts_after_sent;
	unsigned int  delay_rts_before_send;
 	unsigned short enabled;
};

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/
static void do_config(const char *group, const char *key, char *value);

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

int conf_verbose = 0;

sersrvd_config_type *sersrvd_config_cbs = NULL;

#if 0
struct webctrl_config_type webctrl_default_config=
{
  WEBCTRL_IO_INTERPRET, /* mode */
  TRUE, /* def_echo_interpret_mode */
  TRUE, /* echo_interpret_mode */
  FALSE, /* def_handle_telnet_option */
  FALSE, /* handle_telnet_option */
  PAR_WEBCTRL_COM1_HEADER, /* base of the parameters */
  "",
  "",
  "^D", /*CLOSE_STR Ctrl-D */
  NULL, /* decoded_close_str */
  0,    /* decoded_close_str_len */

  "",   /* VSP Server */
  2000,   /* VSP port */
  0,  /* idle timeout */
  "", /* user_pass */
  /* before connect */

  "\\r\\nConnecting to $_client:$_portnr ..\\r\\n",
  "\\r\\nConnection OK \\r\\n",
  "\\r\\nConnection FAILED! \\r\\n",
  "\\r\\nConnection CLOSED\\r\\n",
  "", /* client_msg */
  ""  /* server_msg */

  ,"" /* poll_string */
  ,0  /* poll_timeout */
  ,FALSE /*webctrl_clear_RTS_when_data_sent  used by lonworks */
  ,FALSE /*webctrl_set_RTS_when_data_pending used by lonworks */

#if 0
  , FALSE /* use_http_proxy */
  ,"193.13.178.10:3128" /* http_proxy */
#endif
};
#endif

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

static int config_read = 0;


/****************** FUNCTION DEFINITION SECTION *****************************/

static void 
sersrvd_config_destroy(sersrvd_config_type *config)
{
  free(config->connect_to);
  free(config->allowed_users);
  free(config->allowed_ip_addresses);
}



/* Free all config */
void 
sersrvd_config_free(void)
{
  sersrvd_config_type *config = sersrvd_config_cbs;
  
  while (config != NULL)
  {
    
    sersrvd_config_cbs = sersrvd_config_cbs->next;
    sersrvd_config_destroy(config);
    free(config);
    config = sersrvd_config_cbs;
  }
}


int sersrvd_config_read(const char *configfile)
{
  int fd;
  int i, len;
  int quit = 0;
  char group[MAX_GROUP_LENGTH + 1];
  char line[MAX_LINE_LENGTH + 1];
  if (configfile == NULL)
  {
    configfile = "./sersrvd.conf";
  }

  fd = open(configfile, O_RDONLY);
  if (fd < 0)
  {
    /* Try ../etc/sersrvd.conf */
    configfile = "../etc/sersrvd.conf";
    fd = open(configfile, O_RDONLY);
  }  
  if (fd < 0)
  {
    /* Try /etc/sersrvd.conf */
    configfile = "/etc/sersrvd.conf";
    fd = open(configfile, O_RDONLY);
  }
  if (fd < 0)
  {
    return 0;
  }
  len = 1;
  DINFO(printf("Reading configfile: %s fd=%i\n", configfile, fd));
  
  while (!quit)
  {
    i = 0;
    line[i] = '\0';
    /* Read a line ending with \n or \r\n */
    do
    {
      len = read(fd, &line[i], 1);
      if (len == 1)
      {
        i++;
      }
      else
      {
        quit = TRUE;
      }
    } while (!quit && i < MAX_LINE_LENGTH && line[i-1] != '\n');
    

    
    /* Remove trailing \r and \n */
    line[i] = '\0';
    D(printf("line: %s\n", line));
    i--;
    while (i > 0 && (line[i] == '\r' || line[i] == '\n'))
    {
      line[i] = '\0';
      i--;
    }
    if (i > 0)
    {
      if (line[0] == '[')
      {
        char *end_bracket;
        
        strncpy(group, &line[1], MAX_GROUP_LENGTH);
        group[MAX_GROUP_LENGTH] = '\0';
        end_bracket = strchr(group, ']');
        if (end_bracket != NULL)
        {
          *end_bracket = '\0';
        }
        DCONF(printf("Found group: %s\n", group));
      }
      else if (line[0] == ';' || line[0] == '#')
      {
        /* Comment, ignore */
      }
      else
      {
        char *key = line;
        char *value = strchr(key, '=');
        if (value != NULL)
        {
          /* Terminate key and move to value */
          char *tmp = value;
          tmp--;
          while (tmp > key && *tmp == ' ')
          {
            *tmp = '\0';
            tmp--;
          }
          *value = '\0';
          
          value++;
          /* Skip whitespace and first '"' */
          while (*value == ' ')
          {
            value++;
          }
          if (*value == '\"')
          {
            value++;
          }
          /* Skip trailing whitespace and any ending '"' */
          tmp = value + strlen(value);
          tmp--;
          while (*tmp == ' ' && tmp > value)
          {
            *tmp='\0';
            tmp--;
          }
          if (*tmp == '\"')
          {
            *tmp = '\0';
          }          

          D(printf("Found key: '%s' and value: '%s'\n", key, value));
          do_config(group, key, value);
        }
        else
        {
          printf("Invalid config row: '%s'\n", line);
        }
      }
    }
  } /* while */
  close(fd);
  DINFO(printf("Reading configfile done.\n"));
  config_read++;
  return 1;
}

typedef int config_function_type(sersrvd_config_type *config,
                                 const char *key, char *value);


typedef struct config_key_function
{
  const char* key;
  config_function_type *func;
} config_key_function;

static char* 
first_word(char *value)
{
  char *space;
  
  while (isspace(*value))
  {
    value++;
  }
  space = strchr(value, ' ');
  if (space != NULL)
  {
    *space = '\0';
  }
  return value;
}


static char* 
skip_spaces(char *value)
{
  while (isspace(*value))
  {
    value++;
  }
  return value;
}


static int 
get_yes_no(char *value)
{
  value = first_word(value);
  if (strcasecmp(value, "YES") == 0 || 
      strcasecmp(value, "TRUE") == 0 ||
      strcasecmp(value, "ON") == 0 )
  {
    return 1;
  }
  else
  {
    return 0;
  }
} /* get_yes_no */

static int 
get_enum(char *value, const char **enum_str)
{
  int num = 0;
  value = first_word(value);
  while (enum_str[num] != NULL)
  {
    if (strcasecmp(value, enum_str[num]) == 0)
    {
      D(printf("Found enum: %s = %i\n", value, num));
      return num;
    }
    num++;
  }
  return -1;
} /* get_enum */


static int 
set_string_value(char **s, char *value)
{
  int len;
  value = skip_spaces(value);
  len = strlen(value);
  if (*s != NULL)
  {
    free(*s);
    *s = NULL;
  }
  if (len > 0)
  {
    *s = malloc(len + 1);
    if (*s)
    {
      strcpy(*s, value);
    }
    else
    {
      return 1;
    }
  }
  return 0;
} /* set_string_value */



/* --------------------------------------------------------------------- */

static int 
set_device(sersrvd_config_type *config, const char *key, char *value)
{
  strncpy(config->device, value, MAX_DEVICE_LEN);
  config->device[MAX_DEVICE_LEN] = '\0';
  D(printf("Device: '%s'\n", config->device));
  return 0;
}

static int 
set_mode(sersrvd_config_type *config, const char *key, char *value)
{
  int old_mode = config->port_mode;
  config->port_mode = get_enum(value, port_mode_type_strings);
  if (config->port_mode == -1)
  {
    fprintf(stderr, "Unrecognised param: %s = %s\n", key, value);
    config->port_mode = PORT_OFF;
  }
  if (config_read && old_mode != config->port_mode)
  {
    if (config->portbfd && config->port_mode == PORT_OFF)
    {
      buffered_fd *netbfd = netbfd_cbs;
      listener_cb_type *lcb = listener_cb;
      listener_cb_type *prev_ptr = NULL;
      DCONF(printf("config close port %s\n", config->port_name));
      
      /* Close listener for this port */
      while (lcb != NULL)
      {
        if (lcb->config == config)
        {
          listener_cb_type *tmp = lcb;
          DCONF(printf("config Closing listener: %i\n", lcb->listenerfd));
          close(lcb->listenerfd);
          if (prev_ptr)
          {
            prev_ptr->next = lcb->next;
          }
          else
          {
            listener_cb = lcb->next;
          }
          lcb = lcb->next;
          free(tmp);
        }
        else
        {
          prev_ptr = lcb;
          lcb = lcb->next;
        }
      }
      
      /* Close all network connections to this port */
      while (netbfd != NULL)
      {
        if (netbfd->config == config)
        {
          buffered_fd *tmp = netbfd;
          DCONF(printf("config Closing netbfd %i\n", netbfd->fd));
          netbfd = netbfd->next;
          net_bfd_close(tmp);
        }
        else
        {
          netbfd = netbfd->next;
        }
      }
      /* close port */
      DCONF(printf("config Closing port %i\n", config->portbfd->fd));
      port_bfd_close(config->portbfd);
      config->portbfd = NULL;
    }
    else if (config->portbfd == NULL && config->port_mode != PORT_OFF)
    {
      DCONF(printf("config create port %s\n", config->port_name));
      init_port(config);
    }
  }
  return 0;
}

static int 
set_baudrate(sersrvd_config_type *config, const char *key, char *value)
{
  unsigned long new_baudrate = atol(first_word(value));

  if (config->portbfd && config->portbfd->fd && 
      config->baudrate != new_baudrate)
  {
    set_port_Speed(config->portbfd->fd, new_baudrate);
  }
  config->baudrate = new_baudrate;
  return 0;
}

static int 
set_datasize(sersrvd_config_type *config, const char *key, char *value)
{
  int new_datasize = atoi(first_word(value));
  if (config->portbfd && config->portbfd->fd && 
      config->datasize != new_datasize)
  {
    set_port_DataSize(config->portbfd->fd, new_datasize);
  }
  config->datasize = new_datasize;
  return 0;
}

static int 
set_parity(sersrvd_config_type *config, const char *key, char *value)
{
  int old_parity = config->parity;
  value = first_word(value);
  if (strcasecmp(value, "NONE") == 0)
  {
    config->parity = TNET_COM_PARITY_NONE;
  }
  else if (strcasecmp(value, "ODD") == 0)
  {
    config->parity = TNET_COM_PARITY_ODD;
  }
  else if (strcasecmp(value, "EVEN") == 0)
  {
    config->parity = TNET_COM_PARITY_EVEN;
  }
  else if (strcasecmp(value, "MARK") == 0)
  {
    config->parity = TNET_COM_PARITY_MARK;
  }
  else if (strcasecmp(value, "SPACE") == 0)
  {
    config->parity = TNET_COM_PARITY_SPACE;
  }
  else
  {
    config->parity = TNET_COM_PARITY_NONE;
    return 1;
  }
  if (config->portbfd && config->portbfd->fd && 
      config->parity != old_parity)
  {
    set_port_Parity(config->portbfd->fd, config->parity);
  }

  return 0;
}

static int 
set_stopbits(sersrvd_config_type *config, const char *key, char *value)
{
  int old_stopsize = config->stopsize;
  config->stopsize = atoi(first_word(value));
  if (config->portbfd && config->portbfd->fd && 
      config->stopsize != old_stopsize)
  {
    set_port_StopSize(config->portbfd->fd, config->stopsize);
  }
  return 0;
}


static int 
set_flowcontrol_rtscts(sersrvd_config_type *config, 
                       const char *key, char *value)
{
  int old = config->flowcontrol_rtscts;
  config->flowcontrol_rtscts = get_yes_no(value);
  if (config->portbfd && config->portbfd->fd && 
      config->flowcontrol_rtscts != old)
  {
    struct termios port_info;
    int fd = config->portbfd->fd;
    tcgetattr(fd, &port_info);
    port_info.c_cflag = port_info.c_cflag & ~ CRTSCTS;
    if (config->flowcontrol_rtscts)
    {
      port_info.c_cflag = port_info.c_cflag | CRTSCTS;
    }
    tcsetattr(fd, TCSANOW, &port_info);
  }
  return 0;
}

#ifdef CDTRDSR
static int 
set_flowcontrol_dtrdsr(sersrvd_config_type *config, 
                       const char *key, char *value)
{
  int old = config->flowcontrol_dtrdsr;
  config->flowcontrol_dtrdsr = get_yes_no(value);
  if (config->portbfd && config->portbfd->fd && 
      config->flowcontrol_dtrdsr != old)
  {
    struct termios port_info;
    tcgetattr(config->portbfd->fd, &port_info);
    port_info.c_cflag = port_info.c_cflag & ~ CDTRDSR;
    if (config->flowcontrol_dtrdsr)
    {
      port_info.c_cflag = port_info.c_cflag | CDTRDSR;
    }
    tcsetattr(config->portbfd->fd, TCSANOW, &port_info);
  }
  return 0;
}
#endif



static int 
set_flowcontrol_xonxoff(sersrvd_config_type *config, 
                        const char *key, char *value)
{
  int old = config->flowcontrol_xonxoff;
  config->flowcontrol_xonxoff = get_yes_no(value);
  if (config->portbfd && config->portbfd->fd && 
      config->flowcontrol_xonxoff != old)
  {
    struct termios port_info;
    tcgetattr(config->portbfd->fd, &port_info);
    port_info.c_iflag = port_info.c_iflag & ~(IXON | IXOFF);
    if (config->flowcontrol_xonxoff)
    {
      port_info.c_cflag = port_info.c_cflag | IXON |IXOFF;
    }
    tcsetattr(config->portbfd->fd, TCSANOW, &port_info);
  }
  return 0;
}

static int 
set_flowcontrol(sersrvd_config_type *config, 
                const char *key, char *value)
{
  value = first_word(value);  
  if (strncasecmp(value, "NONE", 4) == 0)
  {
    set_flowcontrol_xonxoff(config, NULL, "NO");
    set_flowcontrol_rtscts(config, NULL, "NO");
#ifdef CDTRDSR
    set_flowcontrol_dtrdsr(config, NULL, "NO");
#endif
    value += 4;
  }
  value = first_word(value);  
  if (strncasecmp(value, "RTS/CTS",7) == 0 ||
      strncasecmp(value, "CTS/RTS",7) == 0 )
  {
    set_flowcontrol_rtscts(config, NULL, "YES");
    value += 7;
  }
  value = first_word(value);
  if (strncasecmp(value, "XON/XOFF",8) == 0)
  {
    set_flowcontrol_xonxoff(config, NULL, "YES");
    value += 7;
  }
  return 0;
}


static void do_set_rs485mode(sersrvd_config_type *config)
{
  if (config->rs485mode)
  {
    int fd = config->portbfd->fd;
    int lines = TIOCM_RTS;
    /* Set RTS to enable RX mode */
    ioctl(fd, TIOCMBIS, &lines);
#ifdef TIOCSERSETRS485
    {
      struct rs485_ctrl ctrl485;
      int status;
      ctrl485.rts_on_send = 0;
      ctrl485.rts_after_sent = 1;
      ctrl485.delay_rts_before_send = 0;
      ctrl485.enabled = 0; /* Only enabled if ordinary write should 
                            *  toggle RTS */
      /* set RS485 mode and delays */
      status = ioctl(fd, TIOCSERSETRS485, &ctrl485);
      if (status != 0)
      {
        fprintf(stderr, "Kernel RS485 support seems broken!\n");
      }
    }
#endif /* TIOCSERSETRS485 */
  }
}

static int 
set_rs485mode(sersrvd_config_type *config, 
              const char *key, char *value)
{
  int old = config->rs485mode;
  config->rs485mode = get_yes_no(value);
  if (config->portbfd && config->portbfd->fd && 
      config->rs485mode != old && config->rs485mode)
  {
    do_set_rs485mode(config);
  }
  return 0;
}



static int 
set_connect_to(sersrvd_config_type *config, const char *key, char *value)
{
  return set_string_value(&config->connect_to, value);
}

static int 
set_listener_port(sersrvd_config_type *config, 
                  const char *key, char *value)
{
  int old = config->listener_port;
  value = first_word(value);
  config->listener_port = atol(value);
  if (config->portbfd && config->portbfd->fd && 
      config->listener_port != old)
  {
    /* Change listener port: 
     * kill current listener and create new
     */
    listener_cb_type *cb = listener_cb;
    while (cb != NULL && cb->config != config)
    {
      cb = cb->next;
    }
    if (cb && cb->config == config)
    {
      close(cb->listenerfd);
      cb->listenerfd = -1;
      cb->portnbr = config->listener_port;
      if (cb->portnbr > 0)
      {
        cb->listenerfd = server_listen_tcp_udp(cb->portnbr, SOCK_STREAM);
      }
    }
    else
    {
      printf("Didn't find listener! \n");
      create_listener(config, listener_process_function);
    }
  }  
  return 0;
}

static int 
set_timeout(sersrvd_config_type *config, 
            const char *key, char *value)
{
  value = first_word(value);
  config->timeout = atol(value);
  return 0;
}

static int 
set_telnet_options(sersrvd_config_type *config, 
                   const char *key, char *value)
{
  config->use_telnet_options = get_yes_no(value);
  return 0;
}

static int 
set_echo_command_mode_port(sersrvd_config_type *config, 
                           const char *key, char *value)
{
  config->echo_command_mode_port = get_yes_no(value);
  return 0;
}

static int 
set_echo_command_mode_net(sersrvd_config_type *config, 
                          const char *key, char *value)
{
  config->echo_command_mode_net = get_yes_no(value);
  return 0;
}


static int 
set_allowed_users(sersrvd_config_type *config, 
                  const char *key, char *value)
{
  return set_string_value(&config->allowed_users, value);
}

static int 
set_allowed_ip_addresses(sersrvd_config_type *config, 
                         const char *key, char *value)
{
  return set_string_value(&config->allowed_ip_addresses, value);
}


const struct config_key_function config_keys[]=
{
/* Be backward compatible */
  {"Device", set_device },
  {"Mode", set_mode },
  {"BaudRate", set_baudrate },
  {"Datasize", set_datasize },
  {"DataBits", set_datasize },
  {"StopBits", set_stopbits },
  {"Parity", set_parity },
  {"FlowControlRTSCTS", set_flowcontrol_rtscts },
  {"Handshake RTS/CTS", set_flowcontrol_rtscts },
  {"FlowControlXON/XOFF", set_flowcontrol_xonxoff },
  {"Handshake XON/XOFF", set_flowcontrol_xonxoff },
#ifdef CDTRDSR
  {"FlowControlDTRDSR", set_flowcontrol_dtrdsr },
#endif
  {"FlowControl", set_flowcontrol },
  {"RS485Mode", set_rs485mode },
  {"ConnectTo", set_connect_to },
  {"Connect to", set_connect_to },
  {"ListenerPort", set_listener_port },
  {"Listener port", set_listener_port },
  {"Timeout", set_timeout },
  {"TelnetOptions", set_telnet_options },
  {"Telnet options", set_telnet_options },
  {"AllowedUsers", set_allowed_users },
  {"AllowedIPAddresses", set_allowed_ip_addresses },

  {"Echo COMMAND_MODE port side", set_echo_command_mode_port },
  {"Echo COMMAND_MODE network side", set_echo_command_mode_net },
  {NULL, NULL }
};



void set_default_config(sersrvd_config_type *config)
{
  config->port_mode = PORT_OFF;
  strcpy(config->device, "/dev/ttySX");
  config->device[9] = config->port_name[3]; /* Assume COMX */
  
  config->baudrate = 38400;
  config->datasize = 8;
  config->parity = TNET_COM_PARITY_NONE;
  config->stopsize = 1;
  config->flowcontrol_rtscts = 0;
  config->flowcontrol_xonxoff = 0;
  config->flowcontrol_dtrdsr = 0;
  config->rs485mode = 0;
  config->listener_port = 0;
  config->connect_to = NULL;

  config->timeout = 0;
  config->num_allowed_connections = 1;
  config->num_connections = 0;
  config->use_telnet_options = 0;
  config->echo_command_mode_net = 0;
  config->echo_command_mode_port = 0;
  config->portbfd = NULL;
  config->next = NULL;
  DCONF(printf("Added config: %s\n", config->port_name));
}

void set_initial_config(sersrvd_config_type *config)
{
  int fd = config->portbfd->fd;
  struct termios port_info;
  set_port_Speed(fd, config->baudrate);
  set_port_DataSize(fd, config->datasize);
  set_port_Parity(fd, config->parity);
  set_port_StopSize(fd, config->stopsize);
  set_port_StopSize(fd, config->stopsize);
  tcgetattr(fd, &port_info);
  port_info.c_cflag = port_info.c_cflag & ~ CRTSCTS;
  if (config->flowcontrol_rtscts)
  {
    port_info.c_cflag = port_info.c_cflag | CRTSCTS;
  }
#ifdef CDTRDSR
  port_info.c_cflag = port_info.c_cflag & ~ CDTRDSR;
  if (config->flowcontrol_dtrdsr)
  {
    port_info.c_cflag = port_info.c_cflag | CDTRDSR;
  }
#endif
  port_info.c_iflag = port_info.c_iflag & ~(IXON | IXOFF);
  if (config->flowcontrol_xonxoff)
  {
    port_info.c_cflag = port_info.c_cflag | IXON |IXOFF;
  }
  tcsetattr(fd, TCSANOW, &port_info);
  do_set_rs485mode(config);
}



sersrvd_config_type *get_config_by_name(const char *name)
{
  sersrvd_config_type *config = sersrvd_config_cbs;
  while (config != NULL && strcmp(config->port_name, name) != 0)
  {
    config = config->next;
  }
  
  if (config == NULL)
  {
    /* Not found, add */
    config = malloc(sizeof(*config));
    if (config)
    {
      memset(config, 0, sizeof(*config));
      strncpy(config->port_name, name, MAX_PORTNAME_LEN);
      config->port_name[MAX_PORTNAME_LEN] = '\0';
      set_default_config(config);
      /* Put first in list */
      config->next = sersrvd_config_cbs;
      sersrvd_config_cbs = config;
    }
    else
    {
      fprintf(stderr, "Failed to alloc config!\n");
    }
  }
  return config;
}



static void do_config(const char *group, const char *key, char *value)
{
  static int prev_ix = -1;
  int i = prev_ix + 1;
  int found = 0;
  int err = 0;
  sersrvd_config_type *config = get_config_by_name(group);
  if (config == NULL)
  {
    return;
  }
  
  while (!found && i != prev_ix)
  {
    if (config_keys[i].key != NULL)

    {
      /* Ignore case from parameter names here, but they must be right for 
         parhand to work */
      if (strcasecmp(config_keys[i].key, key) == 0)
      {
        DCONF(printf("Config %s to %s\n", key, value));
        err += config_keys[i].func(config, key, value);
        found = TRUE;
      }
      else
      {
        i++;
      }
    }
    else
    {
      if (prev_ix == -1)
      {
        /* Not found! */
        printf("# Not found: [%s] '%s'\n", group,key);
        
        i = prev_ix;
      }
      else
      {
        /* Start from beginning */
        DCONF(printf("Start from beginning!\n"));
        i = 0;
      }
    }
  }
  
  prev_ix = i;
} /* do_config */



/****************** END OF FILE sersrvd_config.c ****************************/
