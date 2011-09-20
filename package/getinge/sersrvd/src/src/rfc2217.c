/*!***************************************************************************
*!
*! FILE NAME  : rfc2217.c
*!
*! DESCRIPTION: Telnet COM Port Option according to RFC 2217
*!
*! FUNCTIONS  : handle_rfc2217_command
*! (EXTERNAL)   rfc2217_send_byte_command
*!
*! FUNCTIONS  : <The names of the local functions in this file.>
*! (LOCAL)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Dec 20 1999  Johan Adolfsson    Initial version
*! $Log: rfc2217.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.10  2002/02/25 13:41:04  johana
*! Removed const to get rid of strange behaviour with gcc-cris R37.
*! Changed send_baudrate so it only makes one call to write.
*!
*! Revision 1.9  2001/02/20 12:57:36  johana
*! Always set DTR, CTS and CD delta bits in initial notify.
*! Set the RI_TRAIL bit if RI = 1.
*! Ignore PURGE_DATA commands.
*!
*! Revision 1.8  2001/01/15 14:05:07  johana
*! Added rfc2217_init_connection()
*!
*! Revision 1.7  2001/01/09 19:02:24  johana
*! Added rfc2217_send_flow_command and fixed RESUME/SUSPEND
*!
*! Revision 1.6  2000/10/05 14:05:25  johana
*! Review changes
*!
*! Revision 1.5  2000/09/18 09:08:35  johana
*! Clean up
*!
*! Revision 1.4  2000/09/15 15:59:47  johana
*! masks now in tnet.
*!
*! Revision 1.3  2000/09/11 16:35:46  johana
*! Merged with changes by lenamn. Still some things to do.
*!
*! Revision 1.2  2000/02/23 11:51:56  johana
*! Clean up
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: rfc2217.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/


#include "rfc2217.h"
#include "netutil.h"
#include "bfdutil.h"
#include "serutil.h"

/****************** CONSTANT AND MACRO SECTION ******************************/
#define TNET_RFC2217_START  IAC, SB, TELOPT_COM_PORT_OPTION
const unsigned char tnet_rfc2217_start[3] = { TNET_RFC2217_START };

#define MAX_SIGNATURE_LEN 255

/****************** TYPE DEFINITION SECTION *********************************/

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

/****************** FUNCTION DEFINITION SECTION *****************************/


/*#***************************************************************************
*#
*# FUNCTION NAME: rfc2217_init_connection
*#
*# PARAMETERS   : tnet_handler_type *tnet
*#
*# RETURNS      : 
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : initialise the connection
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Jan 15 2001  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
void 
rfc2217_init_connection(tnet_handler_type *tnet)
{
  struct buffered_fd *bfd = (struct buffered_fd *) tnet->cb;
  unsigned char curr_state;
  /* Send initial NOTIFY-MODEM-STATE */
  curr_state = get_port_state(bfd->tobfd->fd, bfd->tobfd->port_state);
  bfd->tobfd->port_state = curr_state;
  /* Always set the deltas in the initial notify */
  curr_state |= TNET_COM_MODEMSTATE_CD_DELTA | TNET_COM_MODEMSTATE_DSR_DELTA | TNET_COM_MODEMSTATE_CTS_DELTA;

  /* RI_TRAIL should really be set when RI goes from low to high, 
   * we set it if the RI is high: 
   */
  if (curr_state & TNET_COM_MODEMSTATE_RI)
  {
    curr_state |= TNET_COM_MODEMSTATE_RI_TRAIL;
  }
  
  LOGMSG(LOG_INFO,"Send initial NOTIFY-MODEMSTATE: 0x%02X", curr_state);
  rfc2217_send_byte_command( tnet, 
                             COMS_NOTIFY_MODEMSTATE,
                             (curr_state & 
                              tnet->rfc2217_modem_state_mask));
}


/*#***************************************************************************
*#
*# FUNCTION NAME: rfc2217_send_signature
*#
*# PARAMETERS   : tnet_handler_type *tnet
*#                char *signature
*#
*# RETURNS      : 
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Send the signatur to the fd
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 20 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/

static void 
rfc2217_send_signature(tnet_handler_type *tnet, char *signature)
{
  char c;
  tnet->write(tnet->cb, tnet_rfc2217_start, 3) ;
  c = COMS_SIGNATURE;
  tnet->write(tnet->cb, &c, 1);
  tnet_send_string_IAC_escaped(tnet, signature);
  tnet->write(tnet->cb, tnet_end, 2) ;
}

/*#***************************************************************************
*#
*# FUNCTION NAME: rfc2217_send_baudrate
*#
*# PARAMETERS   : tnet_handler_type *tnet
*#                unsigned long baudrate
*#
*# RETURNS      : 
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Send the baudrate to the tnet connection
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 20 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/

static void 
rfc2217_send_baudrate(tnet_handler_type *tnet, 
                      unsigned long baudrate)
{
  unsigned char s[4+4+2] = {IAC, SB, 
                            TELOPT_COM_PORT_OPTION, COMS_SET_BAUDRATE, 
                            0,0,0,0,
                            IAC, SE};

  s[4] = (baudrate >> (8+8+8)) &0xFF;
  s[5] = (baudrate >> (8+8)) & 0xFF;
  s[6] = (baudrate >> (8)) & 0xFF;
  s[7] = (baudrate >> (0)) & 0xFF;
  tnet->write(tnet->cb, s, 4+4+2);
}


/*#***************************************************************************
*#
*# FUNCTION NAME: rfc2217_send_flow_command
*#
*# PARAMETERS   : tnet_handler_type *tnet
*#                unsigned char cmd
*#
*# RETURNS      : 
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Send flow control command cmd
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 20 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
void 
rfc2217_send_flow_command(tnet_handler_type *tnet, unsigned char cmd)
{
  unsigned char s[6] = {IAC, SB, TELOPT_COM_PORT_OPTION, 
                        cmd, 
                        IAC, SE};
  tnet->write(tnet->cb, s, 6);
  if (cmd == COMS_FLOWCONTROL_SUSPEND ||
      cmd == COMC_FLOWCONTROL_SUSPEND)
  {
    LOGMSG(LOG_DEBUG,"Sent flow control suspend command");
  }
  else
  {
    LOGMSG(LOG_DEBUG,"Sent flow control resume command");
  }
} /* rfc2217_send_flow_command */


/*#***************************************************************************
*#
*# FUNCTION NAME: rfc2217_send_byte_command
*#
*# PARAMETERS   : tnet_handler_type *tnet
*#                unsigned char command
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
*# Dec 20 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/

void 
rfc2217_send_byte_command(tnet_handler_type *tnet, 
                          unsigned char command, 
                          unsigned char param)
{
  unsigned char s[7] = {IAC, SB, TELOPT_COM_PORT_OPTION, 
                        command, param, 
                        IAC, SE};
  tnet->write(tnet->cb, s, 7);
} /* rfc2217_send_byte_command */

/*#***************************************************************************
*#
*# FUNCTION NAME: handle_rfc2217_command
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
*# Dec 20 1999  Johan Adolfsson    Initial version
*# Sep 11 2000  Johan Adolfsson    Added range check and check ending IAC SE
*# 
*#***************************************************************************/
int handle_rfc2217_command(tnet_handler_type *tnet, 
                            unsigned char *cmd, size_t size)
{
  /* Get the net bfd */
  struct buffered_fd *bfd = (struct buffered_fd *) tnet->cb;
  char SigStr[MAX_SIGNATURE_LEN];
  int fd;
  unsigned long int BaudRate;
  unsigned char DataSize;
  unsigned char Parity;
  unsigned char StopSize;
  unsigned char FlowControl;
  int chars_read = 0;
  
  /* Get the port fd */
  fd = bfd->tobfd->fd;
  if (size < 6)
  {
    return 0;
  }
  /* IAC SB COM-PORT-OPTION cmd [data] IAC SE */
  /* Check wich command has been requested */
  switch (cmd[3])
  {
    /* Signature */
   case COMC_SIGNATURE:
    if (cmd[4] == IAC && cmd[5] == SE)
    {
      /* Void signature, client is asking for our signature */
      sprintf(SigStr,"Axis sersrvd %s", bfd->config->port_name);
      rfc2217_send_signature(tnet, SigStr);
      LOGMSG(LOG_INFO, "Sent signature: %s", SigStr);
      return 6;
    }
    else
    {
      /* Received client signature */
      unsigned char *iac = memchr(&cmd[4], IAC, size - 4);
      if (iac && ((iac-cmd + 1) <= size) && iac[1] == SE)
      {
        strncpy(SigStr, &cmd[4], iac-cmd - 4);
        LOGMSG(LOG_INFO, "Received client signature: %s", SigStr);
        return iac-cmd+2;
      }
    }
    break;
    
    /* Set serial baud rate */
   case COMC_SET_BAUDRATE:
    /* IAC SB COM-PORT-OPTION SET-BAUD <value(4)> IAC SE */
 
    if (size >= 10 && (cmd[8] == IAC && cmd[9] == SE))
    {
      /* Retrieve the baud rate which is in network order */
      BaudRate = ntohl(*((unsigned long int *) &cmd[4]));
      if (BaudRate == 0)
      {
        /* Client is asking for current baud rate */
        LOGMSG(LOG_DEBUG, "Baud rate notification received");
      }
      else
      {
        /* Change the baud rate */
        LOGMSG(LOG_DEBUG, "Port baud rate change to %lu requested",
               BaudRate);
        set_port_Speed(fd, BaudRate);          
      }
      
      /* Send confirmation */
      BaudRate = get_port_Speed(fd);
      rfc2217_send_baudrate(tnet, BaudRate);
      LOGMSG(LOG_DEBUG, "Port baud rate: %lu",BaudRate);
      return 10;
    }
    else if (size >= 10)
    {
      LOGMSG(LOG_DEBUG, "Baud strange - skip data");
      return 10;
    }
    break;
   default:
    break;
  }
  
  /* Check 7 byte commands */
  /* IAC SB COM-PORT-OPTION cmd[3] <value> IAC SE  */
  if (size >= 7 && cmd[5] == IAC && cmd[6] == SE)
  {
    switch(cmd[3])
    {
      /* Set serial data size */
     case COMC_SET_DATASIZE:
      
      if (cmd[4] == 0)
      {
        /* Client is asking for current data size */
        LOGMSG(LOG_DEBUG, "Data size notification requested");
      }
      else
      {
        /* Set the data size */
        LOGMSG(LOG_DEBUG, "Port data size change to %u requested",
               (unsigned int) cmd[4]);
        set_port_DataSize(fd, cmd[4]);             
      }
      
      /* Send confirmation */
      DataSize = get_port_DataSize(fd);
      rfc2217_send_byte_command(tnet, COMS_SET_DATASIZE, DataSize);
      LOGMSG(LOG_DEBUG, "Port data size: %u",(unsigned int) DataSize);
      break;
      
      /* Set the serial parity */
     case COMC_SET_PARITY:
      if (cmd[4] == 0)
      {
        /* Client is asking for current parity */
        LOGMSG(LOG_DEBUG, "Parity notification requested");
      }
      else
      {
        /* Set the parity */
        LOGMSG(LOG_DEBUG, "Port parity change to %u requested",
               (unsigned int) cmd[4]);
        set_port_Parity(fd, cmd[4]);             
      }
      
      /* Send confirmation */
      Parity = get_port_Parity(fd);
      rfc2217_send_byte_command(tnet, COMS_SET_PARITY, Parity);
      LOGMSG(LOG_DEBUG, "Port parity: %u",(unsigned int) Parity);
      break;
      
      /* Set the serial stop size */
     case COMC_SET_STOPSIZE:
      if (cmd[4] == 0)
      {
        /* Client is asking for current stop size */
        LOGMSG(LOG_DEBUG, "Stop size notification requested");
      }
      else
      {
        /* Set the stop size */
        LOGMSG(LOG_DEBUG, "Port stop size change to %u requested",
               (unsigned int) cmd[4]);
        set_port_StopSize(fd, cmd[4]);
      }
      
      /* Send confirmation */
      StopSize = get_port_StopSize(fd);
      rfc2217_send_byte_command(tnet, COMS_SET_STOPSIZE, StopSize);
      LOGMSG(LOG_DEBUG, "Port stop size: %u",(unsigned int) StopSize);
      break;
      
      /* Flow control and DTR/RTS handling */
     case COMC_SET_CONTROL:
      if (cmd[4] == 0 || cmd[4] == 4 || cmd[4] == 7 ||
          cmd[4] == 10 || cmd[4] == 13)
      {
        /* Client is asking for current flow control or DTR/RTS status */
        LOGMSG(LOG_DEBUG, "Flow control notification requested");
        FlowControl = get_port_FlowControl(fd, cmd[4]);
        rfc2217_send_byte_command(tnet, COMS_SET_CONTROL, FlowControl);
        LOGMSG(LOG_DEBUG, "Port flow control: %u",
               (unsigned int) FlowControl);
      }
      else
      {
        /* Set the flow control */
        LOGMSG(LOG_DEBUG, "Port flow control change to %u requested",
               (unsigned int) cmd[4]);
        set_port_FlowControl(fd, cmd[4]);
        
        /* Flow control status confirmation */
        if (cmd[4] <= 16)
          FlowControl = cmd[4];
        else
          FlowControl = get_port_FlowControl(fd, 0);
        rfc2217_send_byte_command(tnet, COMS_SET_CONTROL, FlowControl);
        LOGMSG(LOG_DEBUG, "Port flow control: %u",
               (unsigned int) FlowControl);
      }
      break;
      
      /* Set the line state mask */
     case COMC_SET_LINESTATE_MASK:          
      LOGMSG(LOG_DEBUG, "Line state set to %u",(unsigned int) cmd[4]);
      /* Only break notification supported */
      tnet->rfc2217_line_state_mask = cmd[4] & (unsigned char) 16;
      rfc2217_send_byte_command(tnet, COMS_SET_LINESTATE_MASK,
                                (unsigned char) tnet->rfc2217_line_state_mask);
      break;
      
      /* Set the modem state mask */
     case COMC_SET_MODEMSTATE_MASK:
      LOGMSG(LOG_DEBUG, "Modem state mask set to %u",(unsigned int) cmd[4]);
      tnet->rfc2217_modem_state_mask = cmd[4];
      rfc2217_send_byte_command(tnet, COMS_SET_MODEMSTATE_MASK, 
                                tnet->rfc2217_modem_state_mask);
      break;
      
      /* Port flush requested */
     case COMC_PURGE_DATA:
      /* With purge do we mean discard (tcflush()) all data in the buffers 
       * or does it mean that we should send all that we have buffered? 
       * Traces from a modem detection seems to indicate that it should mean
       * send what we got now and not that we should discard the data!
       * (It sends AT\n and expects an \r\nOK\r\n back but sends a PURGE_DATA 1
       *  and sometimes this comes before the OK string is read.
       */
      LOGMSG(LOG_DEBUG, "Port flush %u requested",(unsigned int) cmd[4]);
#if 0 /* PURGE don't mean flush! */
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
      /* Don't send any response to the PURGE command */
      /* rfc2217_send_byte_command(tnet, COMS_PURGE_DATA, cmd[4]); */
#endif
      break;

     case COMC_NOTIFY_MODEMSTATE:
      {
        /* We are notified of the state on the other side,
         * use it to control our own output signals and
         * simulate a NULL-modem: 
         * CTS ->RTS
         * DSR ->DTR
         * (RI ->RI)
         * (CD ->CD)
         */
        unsigned char state;
        LOGMSG(LOG_DEBUG, "Notify modemstate, change controlpins");

        state = cmd[4];
        /* Set RTS ON/OFF */      
        if (state & TNET_COM_MODEMSTATE_CTS)
        {
          set_port_FlowControl(fd, TNET_COM_SET_CONTROL_RTS_ON);
        }
        else
        {
          set_port_FlowControl(fd, TNET_COM_SET_CONTROL_RTS_OFF);
        }

        /* Set DTR ON/OFF */      
        if (state & TNET_COM_MODEMSTATE_DSR)
        {
          set_port_FlowControl(fd, TNET_COM_SET_CONTROL_DTR_ON);
        }
        else
        {
          set_port_FlowControl(fd, TNET_COM_SET_CONTROL_DTR_OFF);
        }
        /* This only works for hardware with RI/CD as outputs */
        /* Set RI ON/OFF */      
        if (state & TNET_COM_MODEMSTATE_RI)
        {
          set_port_FlowControl(fd, TNET_COM_SET_CONTROL_RI_ON);
        }
        else
        {
          set_port_FlowControl(fd, TNET_COM_SET_CONTROL_RI_OFF);
        }
        /* Set CD ON/OFF */      
        if (state & TNET_COM_MODEMSTATE_CD)
        {
          set_port_FlowControl(fd, TNET_COM_SET_CONTROL_CD_ON);
        }
        else
        {
          set_port_FlowControl(fd, TNET_COM_SET_CONTROL_CD_OFF);
        }      
        break;
        /* Unknown request */
       default:
        LOGMSG(LOG_DEBUG, "Unhandled request %u",(unsigned int) cmd[3]);
        break;
      }
      
    } /* switch */
    return 7;
  } /* if 7 char*/

  /* Check 6 byte commands */
  if (size >= 6 && cmd[4] == IAC && cmd[5] == SE)
  {
    switch (cmd[3])
    {
      /* Suspend output to the client */
     case COMC_FLOWCONTROL_SUSPEND:
     case COMS_FLOWCONTROL_SUSPEND:
      LOGMSG(LOG_DEBUG, "Flow control suspend requested");
      tnet->rfc2217_flow_to_net_ok = FALSE;
      /* No ack for this command */
      break;
      
      /* Resume output to the client */
     case COMC_FLOWCONTROL_RESUME:
     case COMS_FLOWCONTROL_RESUME:
      LOGMSG(LOG_DEBUG, "Flow control resume requested");
      tnet->rfc2217_flow_to_net_ok = TRUE;
      /* No ack for this command */
      break;

      /* Unknown request */
     default:
      LOGMSG(LOG_DEBUG, "Unhandled request %u",(unsigned int) cmd[3]);
      break;   
    }
    return 6;
  }
  return chars_read;
} /*  handle_rfc2217_command */


/****************** END OF FILE rfc2217.c ***********************************/

