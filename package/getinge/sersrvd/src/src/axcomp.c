/*!***************************************************************************
*!
*! FILE NAME  : axcomp.c
*!
*! DESCRIPTION: Telnet COM Port Option according to Axis "standard"
*!
*! FUNCTIONS  : handle_axcomp_command
*! (EXTERNAL)   axcomp_send_notify_modemstate
*!              axcomp_send_int_command
*!              axcomp_send_str_command
*!
*! FUNCTIONS  : Some...
*! (LOCAL)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! $Log: axcomp.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.4  2000/10/05 14:05:25  johana
*! Review changes
*!
*! Revision 1.3  2000/09/18 09:08:35  johana
*! Clean up
*!
*! Revision 1.2  2000/09/15 15:54:12  johana
*! Looks like a working version.
*!
*! Revision 1.1  2000/09/11 16:35:46  johana
*! Merged with changes by lenamn. Still some things to do.
*!
*!
*! ---------------------------------------------------------------------------
*! (C) Copyright 2000, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: axcomp.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/


#include "rfc2217.h" /* All constant declaration, both to RFCstd and AXCOMPstd */
#include "axcomp.h"
#include "netutil.h"
#include "bfdutil.h"
#include "serutil.h"

/****************** CONSTANT AND MACRO SECTION ******************************/
#define TNET_AXCOMP_START  IAC, SB, TELOPT_AXCOM_PORT_OPTION
const unsigned char tnet_axcomp_start[3] = { TNET_AXCOMP_START };


/****************** TYPE DEFINITION SECTION *********************************/

typedef struct 
{
  int value;
  unsigned char *name;
} comp_map;

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

static const comp_map comp_id_strings[] = {
  { COMP_ID_BAUDRATE,"BAUDRATE" },
  { COMP_ID_BYTESIZE, "BYTESIZE" },
  { COMP_ID_PARITY, "PARITY" },
  { COMP_ID_STOPBITS, "STOPBITS" },
  { COMP_ID_XONCHAR, "XONCHAR" },
  { COMP_ID_XOFFCHAR, "XOFFCHAR" },
  { COMP_ID_EOFCHAR, "EOFCHAR" },
  { COMP_ID_FLOW_RTSCTS, "FLOW_RTSCTS" },
  { COMP_ID_FLOW_DSRDTR, "FLOW_DSRDTR" },
  { COMP_ID_FLOW_XONXOFF, "FLOW_XONXOFF" },
  { COMP_ID_RTSTOGGLE, "RTS_TOGGLE" },
  { COMP_ID_BINARY, "BINARY" },
  { COMP_ID_RI, "RI" },
  { COMP_ID_DSR, "DSR" },
  { COMP_ID_CD, "CD" },
  { COMP_ID_RTS, "RTS" },
  { COMP_ID_CTS, "CTS" },
  { COMP_ID_DTR, "DTR" },
  { COMP_ID_TX, "TX" },
  { COMP_ID_RX, "RX" },
  { COMP_P_NONE, "NONE" },
  { COMP_P_ODD, "ODD" },
  { COMP_P_EVEN, "EVEN" },
  { COMP_P_MARK, "MARK" },
  { COMP_P_SPACE, "SPACE" },
  { COMP_SB_ONESTOPBIT, "ONESTOPBIT" },
  { COMP_SB_TWOSTOPBIT, "TWOSTOPBITS" },
  { COMP_SB_ONE5STOPBIT, "ONE5STOPBITS" },
  { COMP_FALSE, "FALSE" },
  { COMP_TRUE, "TRUE" },
  { COMP_STATUSON, "STATUSON" },
  { COMP_STATUSOFF, "STATUSOFF" },
  { COMP_STATUS , "STATUS" },
  { COMP_CONTROL, "CONTROL" },
  { COMP_SETCONFIG, "SETCONFIG" },
  { 0x00, NULL } /* Must end this way */
};

static const char *comp_id_map[] = {
  "BAUDRATE",
  "BYTESIZE",
  "PARITY",
  "STOPBITS",
  "XONCHAR",
  "XOFFCHAR",
  "EOFCHAR",
  "FLOW_RTSCTS",
  "FLOW_DSRDTR",
  "FLOW_XONXOFF",
  "RTS_TOGGLE",
  "BINARY",
  "RI",
  "DSR",
  "CD",
  "RTS",
  "CTS",
  "DTR",
  "TX",
  "RX"
};
static const int comp_n_id = sizeof(comp_id_map) / sizeof (const char*);
                                        
static const char *comp_parity_map[] = {
  "EMPTY",
  "NONE",
  "ODD",
  "EVEN",
  "MARK",
  "SPACE"
};
static const int comp_n_parity = sizeof(comp_parity_map) / sizeof (const char*);

static const char *comp_stopbits_map[] = {
  "EMPTY",
  "ONESTOPBIT",
  "TWOSTOPBIT",
  "ONE5STOPBIT"
};
static const int comp_n_stopbits = sizeof(comp_stopbits_map) / sizeof (const char*);

static const char *comp_bool_map[] = {
  "FALSE",
  "TRUE"
};
static const int comp_n_bool = sizeof(comp_bool_map) / sizeof (const char*);

/****************** FUNCTION DEFINITION SECTION *****************************/

/*#***************************************************************************
*#
*# FUNCTION NAME: axcompstr2id
*#
*# PARAMETERS   : const char *comp_name
*#
*# RETURNS      : int 
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Convert str to value (according to comp_id_strings[],
*#                constants declaration in axcomp.h)
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
static int axcompstr2id(const char *comp_name)
{
  int i = 0;
  int len;
  
  len = strlen(comp_name);
  
  while (comp_id_strings[i].name != NULL)
  {    
    if (!strncmp(comp_id_strings[i].name, comp_name, len))
    {
      return (comp_id_strings[i].value); 
    }
    i++;
    
  }
  return 0;
} /* axcompstd2id */

/*#***************************************************************************
*#
*# FUNCTION NAME: axcompid2str
*#
*# PARAMETERS   : unsigned char comp_name
*#
*# RETURNS      : const char *
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Convert values to textstr (according to comp_id_map[],
*#                constants declaration in axcomp.h)
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
static const char *axcompid2str(unsigned char comp_name)
{
  int i = (int)comp_name;
  
  if (i >= comp_n_id)
  {
    return 0;
  }
  else
  {
    return (comp_id_map[i]);
  }
} /* axcompid2str */

/*#***************************************************************************
*#
*# FUNCTION NAME: axcompparam2str
*#
*# PARAMETERS   : char *commandstr, unsigned char comp_name
*#
*# RETURNS      : const char *
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Convert values to textstr (according to comp_parity_map[],
*#                comp_stopbits_map[] and comp_bool_map[] (constants
*#                declaration in axcomp.h).
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
static const char *axcompparam2str(unsigned char command, 
                                   unsigned char comp_name)
{
  int i = (int)comp_name;

  /* Value from Parity map */
  if (command == COMP_ID_PARITY)
  {      
    if (i >= comp_n_parity)
    {
      return 0;
    }
    else
    {
      return (comp_parity_map[i]);
    }
  }

  /* Value from Stopbits map */
  if (command == COMP_ID_STOPBITS)
  {          
    if (i >= comp_n_stopbits) 
    {
      return 0;
    } 
    else 
    {
      return (comp_stopbits_map[i]);
    }
  }

  /* Value from Bool map (All commands except Parity or Stopbits) */
  if (i >= comp_n_bool) 
  {
    return 0;
  }
  else
  {
    return (comp_bool_map[i]);
  }
} /* axcompparam2str */


/*#***************************************************************************
*#
*# FUNCTION NAME: axcomp_send_int_command
*#
*# PARAMETERS   : tnet_handler_type *tnet,
*#                unsigned char operation,
*#                unsigned char command,
*#                unsigned char param
*#
*# RETURNS      : 
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Send the telnet COM command using param as parameter
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
void 
axcomp_send_int_command(tnet_handler_type *tnet,
                        unsigned char operation, 
                        unsigned char command, 
                        unsigned long param)
{
  unsigned char s[50];
  int len;
  const char *commandstr  = axcompid2str(command);
  
  len = sprintf(s,"%c%c%c%c%s=%lu%c%c",IAC, SB, TELOPT_AXCOM_PORT_OPTION,
                operation, commandstr, param, IAC, SE);  
  tnet->write(tnet->cb, s, len);
} /* axcomp_send_int_command */

/*#***************************************************************************
*#
*# FUNCTION NAME: axcomp_send_str_command
*#
*# PARAMETERS   : tnet_handler_type *tnet,
*#                unsigned char operation,
*#                unsigned char command,
*#                unsigned char param
*#
*# RETURNS      : 
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Send the telnet command using param as parameter
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
void 
axcomp_send_str_command(tnet_handler_type *tnet,
                        unsigned char operation, 
                        unsigned char command, 
                        unsigned char param)
{
  unsigned char s[50];
  int len;
  const char *commandstr  = axcompid2str(command);
  const char *paramstr = axcompparam2str(command, param);
  
  len = sprintf(s,"%c%c%c%c%s=%s%c%c",IAC, SB, TELOPT_AXCOM_PORT_OPTION,
                operation, commandstr, paramstr, IAC, SE);  
  tnet->write(tnet->cb, s, len);
} /* axcomp_send_str_command */

/*#***************************************************************************
*#
*# FUNCTION NAME: axcomp_send_notify_modemstate
*#
*# PARAMETERS   : tnet_handler_type *tnet
*#                unsigned char prev_state
*#                unsigned char curr_state
*#
*# RETURNS      : 
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Send the telnet COM command using param as parameter
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
void 
axcomp_send_notify_modemstate (tnet_handler_type *tnet, 
                               unsigned char state, unsigned char changed)
{
  changed &= tnet->axcomp_status_on_mask;
  /* { } Brackets skipped for increased readability... */
  if (changed & TIOCM_RI)
  {
    if (state & TIOCM_RI)
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_RI, COMP_TRUE);
    else
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_RI, COMP_FALSE);
  }
  if (changed & TIOCM_DSR)
  {  
    if (state & TIOCM_DSR)
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_DSR, COMP_TRUE);
    else
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_DSR, COMP_FALSE);
  }
  if (changed & TIOCM_RTS)
  {
    if (state & TIOCM_RTS)
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_RTS, COMP_TRUE);
    else
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_RTS, COMP_FALSE);
  }
  if (changed & TIOCM_CTS)
  {
    if (state & TIOCM_CTS)
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_CTS, COMP_TRUE);
    else
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_CTS, COMP_FALSE);
  }
  if (changed & TIOCM_DTR)
  {
    if (state & TIOCM_DTR)
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_DTR, COMP_TRUE);
    else
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_DTR, COMP_FALSE);
  }
  if (changed & TIOCM_ST)
  {
    if (state & TIOCM_ST)
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_TX, COMP_TRUE);
    else
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_TX, COMP_FALSE);
  }
  if (changed & TIOCM_SR)
  {
    if (state & TIOCM_SR)
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_RX, COMP_TRUE);
    else 
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_RX, COMP_FALSE);
  }
} /* send_notify_modem_state */

/*#***************************************************************************
*#
*# FUNCTION NAME: handle_axcomp_command
*#
*# PARAMETERS   : tnet_handler_type *tnet
*#                unsigned char *cmd
*#                size_t size
*#
*# RETURNS      : int  nbr of chars processed
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Handling oc COM Port Control specific commands
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
int handle_axcomp_command(tnet_handler_type *tnet, 
                          unsigned char *cmd, size_t size)
{
  /* Get the net bfd */
  struct buffered_fd *bfd = (struct buffered_fd *) tnet->cb;
  
  int fd;
  unsigned long int BaudRate;
  unsigned char DataSize;
  unsigned char Parity;
  unsigned char StopSize;
  unsigned char FlowControl;
  unsigned char command = 0;
  unsigned char tempvalue;
  char *name;
  char *value;
  unsigned char *end_value;
  int name_id;
  int chars_read = 0;

  /* IAC SB AXCOM operation "name=value" IAC SE */
  /* Check wich command has been requested */
  if (size < 6)
  {
    /* Not enough data */
    return 0;
  }
  name = &cmd[4];
  value = memchr(name, '=', size - 4);
  
  if (value)
  {
    *value = '\0'; /* terminate name (=) */
    value++;
  }
  else
  {
    value = name;
  }
  
  end_value = memchr(value, IAC, size -((size_t)value-(size_t)cmd));

  if (!end_value)
  {
    return 0;
  }
  
  *end_value = '\0'; /* terminate value (IAC) */
  
  if (((size_t)end_value - (size_t)cmd) >= size )
  {
    return 0;
  }
  
  end_value++;
  if (*end_value != SE)
  {
    /* Strange */
    LOGMSG(LOG_DEBUG, "Strange, missing SE");
  }
  
  end_value++;
  chars_read = (int)end_value - (int)cmd;
  
  /* Get the port fd */
  fd = bfd->tobfd->fd;
  LOGMSG(LOG_DEBUG, "%s=%s\n",name, value);
  switch (cmd[3]) /* operation */
  {
   case COMP_SETCONFIG:
    name_id = axcompstr2id(name); 
    
    switch (name_id)
    {
     case COMP_ID_BAUDRATE:
      /* Retrieve the baud rate which is in network order */
      BaudRate = atol(value);
      
      if (BaudRate == 0)
      {
        /* Client is asking for current baud rate */
        LOGMSG(LOG_DEBUG, "Baud rate notification received");              
      }
      else
      {
        /* Change the baud rate */
        LOGMSG(LOG_DEBUG, "Port baud rate change to %lu requested", BaudRate);
        set_port_Speed(fd, BaudRate);          
      }
      /* Send confirmation */
      BaudRate = get_port_Speed(fd);
      axcomp_send_int_command(tnet, COMP_STATUS, COMP_ID_BAUDRATE, BaudRate);
      LOGMSG(LOG_DEBUG, "Port baud rate: %lu",BaudRate);
      break;
      
      /* Set serial data size */
     case COMP_ID_BYTESIZE:
      DataSize = atoi(value);
      
      if (DataSize == 0)
      {
        /* Client is asking for current data size */
        LOGMSG(LOG_DEBUG, "Data size notification requested");             
      }
      else
      {
        /* Set the data size */
        LOGMSG(LOG_DEBUG, "Port data size change to %u requested",
               (unsigned int) DataSize);
        set_port_DataSize(fd, DataSize);             
      }
      /* Send confirmation */
      DataSize = get_port_DataSize(fd);
      axcomp_send_str_command(tnet, COMP_STATUS, COMP_ID_BYTESIZE,  DataSize);
      LOGMSG(LOG_DEBUG, "Port data size: %u",(unsigned int) DataSize);
      break;
      
      /* Set the serial parity */
     case COMP_ID_PARITY:
      Parity = axcompstr2id(value);
      if (Parity == 0)
      {
        /* Client is asking for current parity */
        LOGMSG(LOG_DEBUG, "Parity notification requested");            
      }
      else
      {
        /* Set the parity */
        LOGMSG(LOG_DEBUG, "Port parity change to %u requested", Parity);
        set_port_Parity(fd, Parity);             
      }
      /* Send confirmation */
      Parity = get_port_Parity(fd);
      axcomp_send_str_command(tnet, COMP_STATUS, COMP_ID_PARITY, Parity); 
      LOGMSG(LOG_DEBUG, "Port parity: %u",(unsigned int) Parity);
      break;
     
      /* Set the serial stop size */
     case COMP_ID_STOPBITS:
      StopSize = axcompstr2id(value);
         
      if (StopSize == 0)
      {
        /* Client is asking for current stop size */
        LOGMSG(LOG_DEBUG, "Stop size notification requested");             
      }
      else
      {
        /* Set the stop size */
        LOGMSG(LOG_DEBUG, "Port stop size change to %u requested", 
               StopSize);
        set_port_StopSize(fd, StopSize);
      }
      /* Send confirmation */
      StopSize = get_port_StopSize(fd);
      axcomp_send_str_command(tnet, COMP_STATUS, COMP_ID_STOPBITS, 
                              StopSize);
      LOGMSG(LOG_DEBUG, "Port stop size: %u",(unsigned int) StopSize);
      break;
        
      /*  Not supported commands (yet..) */
      case COMP_ID_XONCHAR:
      case COMP_ID_XOFFCHAR:
      case COMP_ID_EOFCHAR:
      case COMP_ID_RTSTOGGLE: 
      case COMP_ID_BINARY:
       
       break;
         
       /* Flow control */ 
      case COMP_ID_FLOW_RTSCTS:
      case COMP_ID_FLOW_DSRDTR: /* ?????? */
      case COMP_ID_FLOW_XONXOFF:

      tempvalue = axcompstr2id(value);
         
      /* Set flow control, value = false/true */
      if (name_id == COMP_ID_FLOW_XONXOFF)
      {
        if (tempvalue == COMP_FALSE)
          command = TNET_COM_SET_CONTROL_FLOW_XONXOFF_OFF;
        else
          command = TNET_COM_SET_CONTROL_FLOW_XONXOFF;
      } 
      else if (name_id == COMP_ID_FLOW_RTSCTS)
      {
        if (tempvalue == COMP_FALSE)
          command = TNET_COM_SET_CONTROL_FLOW_RTSCTS_OFF;
        else
          command = TNET_COM_SET_CONTROL_FLOW_RTSCTS;
      }

/*    NOTE: Not supported (yet...)
      else if (name_id == COMP_ID_FLOW_DSRDTR)
      {
        if (tempvalue == COMP_FALSE)
          command = TNET_COM_SET_CONTROL_FLOW_DSRDTR_OFF;
        else
          command = TNET_COM_SET_CONTROL_FLOW_DSRDTR;
      } */
      
      /* Set the flow control */
      LOGMSG(LOG_DEBUG, "Port flow control change to %u requested",
             (unsigned int) command);
      set_port_FlowControl(fd, command);
      
      /* Flow control status confirmation */
      FlowControl = get_port_FlowControl(fd, 0);
            
      switch (FlowControl)
      {
       case TNET_COM_SET_CONTROL_FLOW_XONXOFF:
        command = COMP_ID_FLOW_XONXOFF;
        tempvalue = COMP_TRUE;
        break;
       case TNET_COM_SET_CONTROL_FLOW_XONXOFF_OFF:
        command = COMP_ID_FLOW_XONXOFF;
        tempvalue = COMP_FALSE;
        break;
       case TNET_COM_SET_CONTROL_FLOW_RTSCTS:
        command = COMP_ID_FLOW_RTSCTS;
        tempvalue = COMP_TRUE;
        break;
       case TNET_COM_SET_CONTROL_FLOW_RTSCTS_OFF:
        command = COMP_ID_FLOW_RTSCTS;
        tempvalue = COMP_FALSE;
        break;
       default:
        command = COMP_ID_FLOW_RTSCTS;
        tempvalue = COMP_FALSE;
        break;
      }
            
      axcomp_send_str_command(tnet, COMP_STATUS, command, tempvalue);
      LOGMSG(LOG_DEBUG, "Port flow control: %u",
             (unsigned int) FlowControl);
      break;
    }
    break;
      
   case COMP_CONTROL:
     {
       int mask = string2tiocmmask(name);
       if (strcmp(value, "TRUE") == 0){
         ioctl(fd, TIOCMBIS, &mask);
       }
       else{
         ioctl(fd, TIOCMBIC, &mask);
       }
     
       /* Set the Line state mask */
       LOGMSG(LOG_DEBUG, "COMP_CONTROL %s=%s",
	      name, value);
       /*axcomp_send_str_command(tnet, COMP_STATUS, command, value);*/
       
/*       break; */
     }
     break;

    case COMP_STATUSON:
      {
        int mask = string2tiocmmask(name);
        /* Set the Line state mask */
        LOGMSG(LOG_DEBUG, "COMP_STATUSON %s", name);
        tnet->axcomp_status_on_mask |= mask;
      }
      break;

   case COMP_STATUSOFF:
      {
        int mask = string2tiocmmask(name);
        /* Set the Line state mask */
        LOGMSG(LOG_DEBUG, "COMP_STATUSOFF %s", name);
        tnet->axcomp_status_on_mask &= ~mask;
      }
      break;
#if 0  /* Maybe axcomp should have something like this as well... */
    /* Port flush requested */
   case COMP_FLUSH:
    LOGMSG(LOG_DEBUG, "Port flush %u requested",(unsigned int) cmd[4]);
    switch (cmd[4])
    {
      /* Inbound flush */
     case 1:
      tcflush(fd, TCIFLUSH);
      break;
      /* Outbound flush */
     case 2:
      tcflush(fd, TCOFLUSH);
      break;
      /* Inbound/outbound flush */
     case 3:
      tcflush(fd, TCIOFLUSH);
      break;
    }
    
/*    axcomp_send_byte_command(tnet, COMS_PURGE_DATA, cmd[4]); */
    break;
    
#endif
    /* Unknown request */ 
   default:
    LOGMSG(LOG_DEBUG, "Unhandled request %u",(unsigned int) cmd[3]);
    break;
  }
  return chars_read;
  
} /*  handle_axcomp_command */

/************************ END OF FILE axcomp.c ****************************/
