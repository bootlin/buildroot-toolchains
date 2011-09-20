/*!***************************************************************************
*!
*! FILE NAME  : tnetopt.c
*!
*! DESCRIPTION: Handling of telnet options.
*!              
*!
*! FUNCTIONS  : <The names of the external functions in this file.>
*! (EXTERNAL)
*!
*! FUNCTIONS  : <The names of the local functions in this file.>
*! (LOCAL)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Aug  5 1999  Johan Adolfsson    Initial versioni
*! $Log: tnetopt.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.11  2001/02/01 20:34:56  johana
*! Updated debugging and telnet option handling
*!
*! Revision 1.10  2001/01/15 14:31:35  johana
*! Call rfc2217_init_connection(). (Sends initial NOTIFY-MODEMSTATE)
*!
*! Revision 1.9  2001/01/11 16:56:49  johana
*! Use neg_state instead of the *_will_sent.
*! tnet_send_option() keeps track if an option is sent or not.
*! Only allow RFC2217 commands if authorisation is complete.
*!
*! Revision 1.8  2001/01/09 19:00:15  johana
*! * Modified log messages.
*! * Init rfc2217_flow_from_net_ok
*! * Use more *_will_sent.
*! * Handle BINARY option better.
*! * Send WILL as reply to DO SGA.
*!
*! Revision 1.7  2000/11/13 18:17:23  johana
*! Reject echo option, and rfc2217_will_sent handling
*!
*! Revision 1.6  2000/10/05 14:05:26  johana
*! Review changes
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: tnetopt.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/

#include "tnetopt.h"
#include "netutil.h"
#include "rfc2217.h"
#include "axcomp.h"

#include <string.h>

/****************** CONSTANT AND MACRO SECTION ******************************/

/****************** TYPE DEFINITION SECTION *********************************/

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

const unsigned char tnet_end[2] = { IAC, SE };
const unsigned char IAC_str[2] = {IAC, '\0' };

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

/****************** FUNCTION DEFINITION SECTION *****************************/

/* Typical telnet sequences: 
IAC SB option x x x x IAC SE
IAC WILL option
IAC WONT option
IAC DO option
IAC DONT option

IAC option

*/








void tnet_handler_init(tnet_handler_struct_type *tnet, 
                       void *cb, 
                       tnet_write_function_type *write_func)
{
  memset(tnet, 0, sizeof(*tnet));
  tnet->cb = cb;
  tnet->write = write_func;
  tnet->opt_state = IAC_normal;
  tnet->buf_len = 0;
  tnet->rfc2217_enabled = FALSE;
  tnet->axcomp_enabled = FALSE;
  tnet->echo_opt = FALSE;
  tnet->rfc2217_flow_to_net_ok = TRUE;
  tnet->rfc2217_flow_from_net_ok = TRUE;
  tnet->axcomp_status_on_mask = 0;
  tnet->rfc2217_line_state_mask = 0;
  tnet->rfc2217_modem_state_mask = 255;
}

void tnet_handler_destroy(tnet_handler_struct_type *tnet)
{
  /* No resources to free */
}


/* Return 1 if success, 0 if there's not room */
int tnet_add_to_buffer(tnet_handler_struct_type *tnet, 
                       unsigned char *buf, 
                       ssize_t len)
{
  if (len < (TNET_BUFFER_SIZE - tnet->buf_len))
  {
    /* There is room, copy data to it */
    memcpy(&tnet->buffer[tnet->buf_len], buf, len);
    tnet->buf_len += len;
    return 1;
  }
  else
  {
    /* No room! - drop whole frame */
    fprintf(stderr, "To long telnet string!\n");
    return 0;
  }
}
void tnet_clear_buffer(tnet_handler_struct_type *tnet)
{
  tnet->buf_len = 0;
  tnet->buffer[0]='\0';
}

void tnet_used_buffer(tnet_handler_struct_type *tnet, ssize_t len)
{
  if (len < tnet->buf_len)
  {
    tnet->buf_len -= len;
    memmove(tnet->buffer, &tnet->buffer[len], tnet->buf_len);
  }
  else
  {
    tnet->buf_len = 0;
  }
}



void tnet_send_IAC_escaped(tnet_handler_type *tnet, 
                           unsigned char *buf, 
                           int len)
{
  unsigned char *pos;
  int chunk_len;
  pos = memchr(buf, IAC, len);
  while (pos != NULL)
  {
    chunk_len = pos - buf + 1;
    tnet->write(tnet->cb, buf, chunk_len); /* Write including IAC */
    tnet->write(tnet->cb, IAC_str, 1);   /* Write extra IAC */
    len -= chunk_len;
    
    buf = pos + 1;
    pos = memchr(buf, IAC, len);
  }
  /* Write remaining data */
  if (len > 0)
  {
    tnet->write(tnet->cb, buf, len); 
  }
} /* tnet_send_IAC_escaped */



void tnet_send_string_IAC_escaped(tnet_handler_type *tnet, 
                                  unsigned char *s)
{
  tnet_send_IAC_escaped(tnet, s, strlen(s));
#if 0  
  unsigned char *pos = s;
  while (*pos != '\0')
  {
    /* 01234x678 */
    if (*pos == IAC)
    {
      write(fd, s, pos - s + 1);
      s = pos;
      write(fd, pos, 1);
    }
    pos++;
  }
#endif
} /* tnet_send_string_IAC_escaped */

static const char * tnet_cmd_strings[] = {
  "WILL", "WONT", "DO", "DONT" 
};
static const char * option_to_string(char option)
{
  switch (option)
  {
   case TELOPT_BINARY:
    return "BINARY";
    break;
   case TELOPT_ECHO:
    return "ECHO";
    break;
   case TELOPT_SGA:
    return "SGA";
    break;
   case TELOPT_COM_PORT_OPTION:
    return "COMP(RFC2217)";
    break;
   case TELOPT_AXCOM_PORT_OPTION:
    return "AXCOMP";
    break;
   default:
    return "?";
  }
}



/* Send the specific telnet command + option to tnet */
void tnet_send_option(tnet_handler_type *tnet, 
                      unsigned char command, char option)
{
  unsigned char s[3] = {IAC, command, option };
  if (option < MAX_TELOPT && ((WILL <= command && command <= DONT)))
  {
    /* Use the fact that WILL, WONT, DO, DONT is 251,252,253,254 
     * and that BW_WILL_SENT is 1
     */
    unsigned char bw = 1 << (command - WILL);
    if (!(tnet->neg_state[(int)option] & bw))
    {
      /* Not sent already */     
      tnet->write(tnet->cb, s, 3);
      tnet->neg_state[(int)option] |= bw;
      LOGMSG(LOG_INFO, "Sent %s 0x%02X (%s)\n", 
             tnet_cmd_strings[command-WILL], 
             option, option_to_string(option));
      
    }
  }
  else
  {
    tnet->write(tnet->cb, s, 3);
  }
} /* tnet_send_option */

/* Common telnet IAC commands handling
 * Return number of bytes processed (0 means to little data available)
 */
ssize_t 
tnet_handle_IAC(tnet_handler_struct_type *tnet, 
                unsigned char *buf, 
                ssize_t len,
                int only_allow_negotiation)
{
  ssize_t chars_read = 0;
  /* Check which command */
  tnet->opt_state = TOS_IAC; /* When completed command, 
                              *  set to IAC_normal or TOS_PRINT_IAC
                              */
  if (len < 2 )
  {
    return 0;
  }
  

  switch(buf[1])
  {
    /* Suboptions */
   case SB:
    if (len < 5) /* room for at least: IAC SB option IAC SE */
    {
      tnet->opt_state = TOS_IAC;
      return 0;
    }
#if 1
    /* DialOut/IP sends SB before authorised and times out if it doesn't 
     * get a response.
     */
    if (only_allow_negotiation)
    {
      /* Find end of message: IAC SE */
      unsigned char *tmp = memchr(&buf[2], IAC, len - 2);
      if (tmp && tmp[1] == SE)
      {
        /* IAC SB xx yy IAC SE */
        /* buf          tmp    */
        LOGMSG(LOG_INFO, "Unathorized SB 0x%02X 0x%02X, skipping %lu bytes\n",
               buf[2], buf[3], tmp-buf+2);
        tnet->opt_state = IAC_normal;
        return tmp-buf+2;
      }
      return 0;
    }
#endif    
    switch (buf[2])
    {
      /* RFC 2217 COM Port Control Protocol option */
     case TELOPT_COM_PORT_OPTION:
      chars_read = handle_rfc2217_command(tnet, buf, len);
      break;
      /* AXIS AXCOMP protocol handling */
     case TELOPT_AXCOM_PORT_OPTION:
      chars_read = handle_axcomp_command(tnet, buf, len);
      break;
      
     default:
      {
        int i = 3;
        
        LOGMSG(LOG_DEBUG,"Unknown suboption received: %u",
               (unsigned int) buf[2]);
      
      /* Parse until IAC SE */
        while (buf[i] != IAC && i < len)
        {
          i++;
        }
        if (buf[i] == IAC && len > i)
        {
          i++;
          if (buf[i] == SE)
          {
            LOGMSG(LOG_DEBUG,"Skipping %i bytes.", i);
            chars_read = i;
          }
        }
      }
      break;
    }
    tnet->opt_state = IAC_normal;
    return chars_read;
    break;
    
    /* Requests for options */
   case WILL:
    if (len < 3) /* room for at least: IAC WILL option */
    {
      tnet->opt_state = TOS_IAC;
      return 0;
    }
    if (buf[2] < MAX_TELOPT)
    {
      tnet->neg_state[buf[2]] |= BW_WILL_RECV;
    }
    
    switch (buf[2])
    {
      /* COM Port Control Option */
     case TELOPT_COM_PORT_OPTION:
      if (only_allow_negotiation)
      {
        LOGMSG(LOG_INFO,"Ignore Telnet COM Port Control Enabled (WILL)");
      }
      else
      {
        LOGMSG(LOG_INFO,"Telnet COM Port Control Enabled (WILL/DO)");
        tnet->rfc2217_enabled = TRUE;
        tnet_send_option(tnet, DO,TELOPT_COM_PORT_OPTION);
      }
      
      break;
      /* AXIS AXCOMP COM Port Control Option */
     case TELOPT_AXCOM_PORT_OPTION:
      LOGMSG(LOG_INFO,"Telnet AXCOM Port Control Enabled (WILL/DO)");
      tnet->axcomp_enabled = TRUE;  
      tnet_send_option(tnet, DO, TELOPT_AXCOM_PORT_OPTION);
      break;      
      /* Telnet Binary mode */
     case TELOPT_BINARY:
      LOGMSG(LOG_INFO,"Telnet Binary Transfer Enabled (WILL/DO)");
      tnet_send_option(tnet, DO, TELOPT_BINARY);
      break;
      
      /* Echo request not handled */
     case TELOPT_ECHO:
      LOGMSG(LOG_INFO,"Rejecting Telnet Echo Option (WILL/DONT)");
      tnet_send_option(tnet, DONT, TELOPT_ECHO);
      break;
      
      /* No go ahead needed */
     case TELOPT_SGA:
      LOGMSG(LOG_INFO,"Suppressing Go Ahead characters (WILL/DO)");
      tnet_send_option(tnet, DO, TELOPT_SGA);
      break;
      
      /* Reject everything else */
     default:
      LOGMSG(LOG_DEBUG, "Rejecting option WILL: %u",
                         (unsigned int) buf[2]);
      tnet_send_option(tnet, DONT, buf[2]);
      break;
    }
    chars_read = 3;
    tnet->opt_state = IAC_normal;
    return chars_read;
    break;
    
    /* Confirmations for options */
   case DO:
    if (len < 3) /* room for at least: IAC DO option */
    {
      tnet->opt_state = TOS_IAC;
      return 0;
    }
    if (buf[2] < MAX_TELOPT)
    {
      tnet->neg_state[buf[2]] |= BW_DO_RECV;
    }
    switch (buf[2])
    {
      /* COM Port Control Option */
     case TELOPT_COM_PORT_OPTION:
      if (only_allow_negotiation)
      {
        LOGMSG(LOG_INFO,"Ignore Telnet COM Port Control Enabled (DO)");
      }
      else
      {
        LOGMSG(LOG_INFO,"Telnet COM Port Control Enabled (DO/WILL)");
        tnet->rfc2217_enabled = TRUE;
        tnet_send_option(tnet, WILL, TELOPT_COM_PORT_OPTION);
        rfc2217_init_connection(tnet);
      }
      break;
      /* COM Port Control Option */

     case TELOPT_AXCOM_PORT_OPTION:
      LOGMSG(LOG_INFO,"Telnet AXCOMP Port Control Enabled (DO)");
      tnet->axcomp_enabled = TRUE;
      tnet_send_option(tnet, WILL, TELOPT_COM_PORT_OPTION);
      break;
      
      /* Telnet Binary mode */
     case TELOPT_BINARY:
      LOGMSG(LOG_INFO,"Telnet Binary Transfer Enabled (DO/WILL)");
      tnet_send_option(tnet, WILL, TELOPT_BINARY);
      break;
      
      /* Echo request not handled */
     case TELOPT_ECHO:
      LOGMSG(LOG_INFO,"Rejecting Telnet Echo Option (DO/WONT)");
      /*tnet->echo_opt = TRUE;*/
      tnet_send_option(tnet, WONT, TELOPT_ECHO);
      break;
      
      /* No go ahead needed */
     case TELOPT_SGA:
      LOGMSG(LOG_INFO,"Suppressing Go Ahead characters (DO/WILL)");
      tnet_send_option(tnet, WILL, TELOPT_SGA);
      break;
      
      /* Reject everything else */
     default:
      LOGMSG(LOG_DEBUG, "Rejecting option DO: %u",
                         (unsigned int) buf[2]);
      tnet_send_option(tnet, WONT, buf[2]);
      break;
    }
    chars_read = 3;
    tnet->opt_state = IAC_normal;
    return chars_read;
    break;
    
    /* Notifications of rejections for options */
   case WONT:
   case DONT:
    if (len < 3) /* room for at least: IAC WILL option */
    {
      tnet->opt_state = TOS_IAC;
      return 0;
    }
    if (buf[2] < MAX_TELOPT && buf[1] == WONT)
    {
      tnet->neg_state[buf[2]] |= BW_WONT_RECV;
    }
    else if (buf[2] < MAX_TELOPT && buf[1] == DONT)
    {
      tnet->neg_state[buf[2]] |= BW_DONT_RECV;
    }
    if (buf[2] == TELOPT_COM_PORT_OPTION)
    {
      LOGMSG(LOG_ERR,"Client doesn't support Telnet COM Port "
             "Protocol Option (RFC 2217)");
      tnet->rfc2217_enabled = FALSE;
    }
    else if (buf[2] == TELOPT_AXCOM_PORT_OPTION)
    {
      LOGMSG(LOG_ERR,"Client doesn't support Telnet AXCOM Port "
             "Protocol Optiong");
      tnet->axcomp_enabled = FALSE;
    }
    else
    {
      LOGMSG(LOG_DEBUG, "Received rejection for option: %u",
                         (unsigned int) buf[2]);
    }
    chars_read = 3;
    tnet->opt_state = IAC_normal;
    return chars_read;
    break;
    
   case IAC:  /* Double IAC - print IAC */
    tnet->opt_state = TOS_PRINT_IAC;
    return 1;
    break;
   case GA:  /* you may reverse the line */
   case EL:  /* erase the current line */
   case EC:  /* erase the current character */

   case AO: /* abort output - but let prog finish */
   case IP:  /* interrupt process - permanently */
   case BREAK: /* break */
   case DM: /* data mark - for connect. cleaning */
   case NOP:/* no operation */
    tnet->opt_state = IAC_normal;
    return 2;
    break;
   case SE: /* end of subnegotiation - should not happen */
    tnet->opt_state = IAC_normal;
    return 2;
    break;
   case AYT: /* are you there */
    tnet->opt_state = IAC_normal;
    tnet->write(tnet->cb, "[YES]\r\n", 7);
    tnet->opt_state = IAC_normal;    
    return 2;
    break;
   default:
    fprintf(stderr, "Unhandled telnet command: %i\n", (int)buf[1]);
    LOGMSG(LOG_INFO, "Unhandled telnet command: %i\n", (int)buf[1]);
    tnet->opt_state = IAC_normal;
    return 2;
    break;
  }
  return 0;
}


/****************** END OF FILE tnetopt.c ***********************************/
