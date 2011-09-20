/*!***************************************************************************
*!
*! FILE NAME  : serutil.c
*!
*! DESCRIPTION: Serial port functions.
*!
*! FUNCTIONS  : Many...
*! (EXTERNAL)
*!
*! FUNCTIONS  : 
*! (LOCAL)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
* $Log: serutil.c,v $
* Revision 1.1  2003/12/18 22:25:39  gsiftar
* Initial version from Axis
*
* Revision 1.9  2001/02/20 12:59:28  johana
* Only set RI_TRAIL bit if status changed and RI = 1.
*
* Revision 1.8  2000/10/05 14:05:26  johana
* Review changes
*
* Revision 1.7  2000/09/18 09:54:46  johana
* Fixed defaultvalue for parity, reread of config, debugprintouts.
*
* Revision 1.6  2000/09/15 16:00:57  johana
* Added string2tiocmmask
*
* Revision 1.5  2000/09/11 16:35:46  johana
* Merged with changes by lenamn. Still some things to do.
*
* Revision 1.4  2000/08/17 15:59:24  johana
* Removed debug
*
* Revision 1.3  2000/02/23 11:53:12  johana
* Clena up
*
*! Aug  5 1999  Johan Adolfsson    Initial version
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: serutil.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <termios.h>
#include <termio.h>

#include "serutil.h"

/****************** CONSTANT AND MACRO SECTION ******************************/

#define DLOGMSG(level, format...) fprintf(stderr, format)
#define D(x)

#define FALSE 0
#define TRUE !FALSE

/*#define TCSA_MODE TCSADRAIN*/
#define TCSA_MODE TCSANOW
/****************** TYPE DEFINITION SECTION *********************************/

struct state_str
{
  int state;
  const char *str;
  
};

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

const struct state_str control_state_str[]={
  {TIOCM_DTR, "DTR" },
  {TIOCM_RTS, "RTS"},
  {TIOCM_ST, "TX" },
  {TIOCM_SR, "RX" },
  {TIOCM_CTS, "CTS" },
  {TIOCM_CD, "CD" },
  {TIOCM_RI, "RI" },
  {TIOCM_DSR, "DSR" },
  {0, NULL }
};
#define TIOCM_OUTPUT_MASK (TIOCM_RTS | TIOCM_DTR) 
#define TIOCM_INPUT_MASK (TIOCM_CD | TIOCM_RI | TIOCM_CTS | TIOCM_DSR) 


/****************** LOCAL VARIABLE DECLARATION SECTION **********************/


/****************** FUNCTION DEFINITION SECTION *****************************/


int string2tiocmmask(const char *name)
{
  int i = 0;
  while (control_state_str[i].str != NULL)
  {
    if (strcasecmp(name, control_state_str[i].str)== 0)
    {
      return control_state_str[i].state;
    }
    i++;
  }
  return 0;
}

struct tcspeed{
	const unsigned long baud;
	const speed_t code;
};

static const struct tcspeed tcspeeds[] = {
#ifdef B50
	{50, B50},
#endif
#ifdef B75
	{75, B75},
#endif
#ifdef B110
	{110, B110},
#endif
#ifdef B134
	{134, B134},
#endif
#ifdef B150
	{150, B150},
#endif
#ifdef B200
	{200, B200},
#endif
#ifdef B300
	{300, B300},
#endif
#ifdef B600
	{600, B600},
#endif
#ifdef B1200
	{1200, B1200},
#endif
#ifdef B1800
	{1800, B1800},
#endif
#ifdef B2400
	{2400, B2400},
#endif
#ifdef B4800
	{4800, B4800},
#endif
#ifdef B9600
	{9600, B9600},
#endif
#ifdef B19200
	{19200, B19200},
#endif
#ifdef B38400
	{38400, B38400},
#endif
#ifdef B57600
	{57600, B57600},
#endif
#ifdef B115200
	{115200, B115200},
#endif
#ifdef B230400
	{230400, B230400},
#endif
#ifdef B460800
	{460800, B460800},
#endif
#ifdef B921600
	{921600, B921600},
#endif
#ifdef B1843200
	{1843200, B1843200},
#endif
#ifdef B6250000
	{6250000, B6250000},
#endif
#ifdef B0
	{0, B0},
#endif
	{0, 0}
};



/* Retrieves the port speed from port_fd (in true value) */
unsigned long int get_port_Speed(int port_fd)
{
  struct termios port_info;
  speed_t speed;
  int i = 0;
  
  tcgetattr(port_fd, &port_info);
  speed = cfgetospeed(&port_info);
  while (tcspeeds[i].code != 0 && tcspeeds[i].code != speed)
  {
    i++;
  }
  if (tcspeeds[i].code == speed)
  {
    return tcspeeds[i].baud;
  }
  D(printf("get_port_Speed: speed: %i\n", speed));
  DLOGMSG(LOG_WARNING,"Unknown baud rate retreived. Returning 0");
  return 0L;
}

/* Retrieves the data size from port_fd (in num bits) */
unsigned char get_port_DataSize(int port_fd)
{
  struct termios port_info;
  tcflag_t datasize;

  tcgetattr(port_fd, &port_info);
  datasize = port_info.c_cflag & CSIZE;
  /* It's a straight mapping here */
  switch (datasize)
  {
   case CS5:
    return (unsigned char) 5;
   case CS6:
    return (unsigned char) 6;
   case CS7:
    return (unsigned char) 7;
   case CS8:
    return (unsigned char) 8;
   default:
    return (unsigned char) 0;
  }
}

/* Retrieves the parity settings from port_fd (in TNET_COM_xx flag) */
unsigned char get_port_Parity(int port_fd)
{
  struct termios port_info;

  tcgetattr(port_fd, &port_info);

  if ((port_info.c_cflag & PARENB) == 0)
  {
    return (unsigned char) TNET_COM_PARITY_NONE;
  }
  else  if ((port_info.c_cflag & PARENB) != 0 &&
            (port_info.c_cflag & PARODD) != 0)
  {
    return (unsigned char) TNET_COM_PARITY_ODD;
  }
  else
  {
/* NOTE: How handle TNET_COM_PARITY_MARK  and TNET_COM_PARITY_SPACE ? */
    return (unsigned char) TNET_COM_PARITY_EVEN;
  }
}

/* Retrieves the stop bits size from port_fd (in TNET_COM_STOPSIZEx flag) */
unsigned char get_port_StopSize(int port_fd)
{
  struct termios port_info;

  tcgetattr(port_fd, &port_info);

  if ((port_info.c_cflag & CSTOPB) == 0)
  {
    return (unsigned char) TNET_COM_STOPSIZE1;
  }
  else
  {
    return (unsigned char) TNET_COM_STOPSIZE2;
  }
  
/* NOTE: How handle TNET_COM_STOPSIZE1_5 */
} /* get_port_StopSize */
  
/* Retrieves the flow control status, including DTR and RTS status.
 * The RFC2217 doesn't support both RTS/CTS and XON/XOFF at the same time
 * but we the AxCOMP does.
 */

unsigned char get_port_FlowControl(int port_fd, unsigned char which)
{
  struct termios port_info;
  int line;

  /* Get the port info and line status */
  tcgetattr(port_fd, &port_info);
  ioctl(port_fd, TIOCMGET, &line);

  /* Check wich kind of information is requested */
  switch (which)
  {
    /* Flow Control Setting (outbound/both) */
   case TNET_COM_SET_CONTROL_REQUEST_FLOW:
    /* XON/XOFF can really be at the same time as RTSCTS */
    if ((port_info.c_iflag & IXON))
      return (unsigned char) TNET_COM_SET_CONTROL_FLOW_XONXOFF;
    if ((port_info.c_cflag & CRTSCTS))
      return (unsigned char) TNET_COM_SET_CONTROL_FLOW_RTSCTS;
    return (unsigned char) TNET_COM_SET_CONTROL_FLOW_NONE;
    break;
    
    /* BREAK state  */
   case TNET_COM_SET_CONTROL_REQUEST_BREAK:
    /* This isn't really a state is it? - break lasts a certain time */
      return (unsigned char) TNET_COM_SET_CONTROL_BREAK_OFF;
    break;
        
    /* DTR Signal state */
   case TNET_COM_SET_CONTROL_REQUEST_DTR:
    if ((line & TIOCM_DTR) == 0)
      return (unsigned char) TNET_COM_SET_CONTROL_DTR_OFF ;
    else
      return (unsigned char) TNET_COM_SET_CONTROL_DTR_ON ;
    break;

    /* RTS Signal state */
   case TNET_COM_SET_CONTROL_REQUEST_RTS:
    if ((line & TIOCM_RTS) == 0)
      return (unsigned char) TNET_COM_SET_CONTROL_RTS_OFF;
    else
      return (unsigned char) TNET_COM_SET_CONTROL_RTS_ON;
    break;

    /* Flow Control Setting (inbound) */
   case TNET_COM_SET_CONTROL_REQUEST_FLOWIN:
    if ((port_info.c_iflag & IXOFF))
      return (unsigned char) TNET_COM_SET_CONTROL_FLOWIN_XOFF ;
    if ((port_info.c_cflag & CRTSCTS))
      return (unsigned char) TNET_COM_SET_CONTROL_FLOWIN_RTSCTS ;
    return (unsigned char) TNET_COM_SET_CONTROL_FLOWIN_NONE;
    break;

   default:
    if ((port_info.c_iflag & IXON))
      return (unsigned char) TNET_COM_SET_CONTROL_FLOW_XONXOFF;
    if ((port_info.c_cflag & CRTSCTS))
      return (unsigned char) TNET_COM_SET_CONTROL_FLOW_RTSCTS;
    return (unsigned char) TNET_COM_SET_CONTROL_FLOW_NONE;
    break;
  }
} /* get_port_FlowControl */

/* Return the status of the modem control lines (CD, CTS, DSR, RI) */
unsigned char get_port_state(int port_fd, unsigned char state_mask)
{
  int line;
  unsigned char state = 0;
    
  ioctl(port_fd, TIOCMGET, &line);

  if ((line & TIOCM_CAR) != 0)
  {
    state |= (unsigned char) TNET_COM_MODEMSTATE_CD;
  }
  if ((line & TIOCM_RNG) != 0)
  {
    state |= (unsigned char) TNET_COM_MODEMSTATE_RI;
  }
  if ((line & TIOCM_DSR) != 0)
  {
    state |= (unsigned char) TNET_COM_MODEMSTATE_DSR;
  }
  if ((line & TIOCM_CTS) != 0)
  {
    state |= (unsigned char) TNET_COM_MODEMSTATE_CTS;
  }

  if ((state & TNET_COM_MODEMSTATE_CD) != 
      (state_mask & TNET_COM_MODEMSTATE_CD))
  {
    state |= (unsigned char) TNET_COM_MODEMSTATE_CD_DELTA;
  }

  /* RI_TRAIL should be set when RI goes from low to high */
  if ((state & TNET_COM_MODEMSTATE_RI) && 
      ((state & TNET_COM_MODEMSTATE_RI) != 
       (state_mask & TNET_COM_MODEMSTATE_RI)))
  {
    state |= (unsigned char) TNET_COM_MODEMSTATE_RI_TRAIL;
  }
  if ((state & TNET_COM_MODEMSTATE_DSR) != 
      (state_mask & TNET_COM_MODEMSTATE_DSR))
  {
    state |= (unsigned char) TNET_COM_MODEMSTATE_DSR_DELTA;
  }
  if ((state & TNET_COM_MODEMSTATE_CTS) != 
      (state_mask & TNET_COM_MODEMSTATE_CTS))
  {
    state |= (unsigned char) TNET_COM_MODEMSTATE_CTS_DELTA;
  }
  
  return (state);
}

/* Set the serial port data size */
void set_port_DataSize(int port_fd, unsigned char DataSize)
{
  struct termios port_info;
  tcflag_t PDataSize;

  switch (DataSize)
  {
   case 5:
    PDataSize = CS5;
    break;
   case 6:
    PDataSize = CS6;
    break;
   case 7:
    PDataSize = CS7;
    break;
   case 8:
    PDataSize = CS8;
    break;
   default:
    PDataSize = CS8;
    break;
  }

  tcgetattr(port_fd, &port_info);
  port_info.c_cflag = port_info.c_cflag &
    ((port_info.c_cflag & ~CSIZE) | PDataSize);
  tcsetattr(port_fd, TCSA_MODE, &port_info);
}

/* Set the serial port parity */
void set_port_Parity(int port_fd, unsigned char Parity)
{
  struct termios port_info;

  tcgetattr(port_fd, &port_info);

  switch (Parity)
  {
   case TNET_COM_PARITY_NONE:
    port_info.c_cflag = port_info.c_cflag & ~PARENB;
    break;
   case TNET_COM_PARITY_ODD :
    port_info.c_cflag = port_info.c_cflag | PARENB | PARODD;
    break;
   case TNET_COM_PARITY_EVEN:
    port_info.c_cflag = (port_info.c_cflag | PARENB) & ~PARODD;
    break;
    /* There's no support for MARK and SPACE parity so sets no parity */
   case TNET_COM_PARITY_MARK:
   case TNET_COM_PARITY_SPACE:
   default:
    DLOGMSG(LOG_WARNING,"Requested unsupported parity %i. Set to no parity\n",
            (int)Parity);
    port_info.c_cflag = port_info.c_cflag & ~PARENB;
    break;
  }

  tcsetattr(port_fd, TCSA_MODE, &port_info);
}

/* Set the serial port stop bits size */
void set_port_StopSize(int port_fd, unsigned char StopSize)
{
  struct termios port_info;

  tcgetattr(port_fd, &port_info);

  switch (StopSize)
  {
   case TNET_COM_STOPSIZE1:
    port_info.c_cflag = port_info.c_cflag & ~CSTOPB;
    break;
   case TNET_COM_STOPSIZE2:
    port_info.c_cflag = port_info.c_cflag | CSTOPB;
    break;
   case TNET_COM_STOPSIZE1_5:
    port_info.c_cflag = port_info.c_cflag & ~CSTOPB;
    DLOGMSG(LOG_WARNING,"Requested unsupported 1.5 bits stop size. "
           "Set to 1 bit stop size\n");
    break;
   default:
    port_info.c_cflag = port_info.c_cflag & ~CSTOPB;
    break;
  }

  tcsetattr(port_fd, TCSA_MODE, &port_info);
}

/* Set the port flow control and DTR and RTS status */
void set_port_FlowControl(int port_fd, unsigned char flow_control)
{
  struct termios port_info;
  int line;

  /* Gets the base status from the port */
  tcgetattr(port_fd, &port_info);
  ioctl(port_fd, TIOCMGET, &line);

  /* Check which settings to change */
  switch (flow_control)
  {
    /* No Flow Control (outbound/both) */
   case TNET_COM_SET_CONTROL_FLOW_NONE:
    port_info.c_iflag = port_info.c_iflag & ~(IXON | IXOFF);
    port_info.c_cflag = port_info.c_cflag & ~CRTSCTS;
    break;
    /* XON/XOFF Flow Control (outbound/both) */
   case TNET_COM_SET_CONTROL_FLOW_XONXOFF:
    port_info.c_iflag = port_info.c_iflag | IXON | IXOFF;
    break;
   case TNET_COM_SET_CONTROL_FLOW_XONXOFF_OFF:
    port_info.c_iflag = port_info.c_iflag & ~(IXON | IXOFF);
    break;                                      
    /* HARDWARE Flow Control (outbound/both) */
   case TNET_COM_SET_CONTROL_FLOW_RTSCTS:
    port_info.c_cflag = port_info.c_cflag | CRTSCTS;
    break;
   case TNET_COM_SET_CONTROL_FLOW_RTSCTS_OFF:
    port_info.c_cflag = port_info.c_cflag & ~CRTSCTS;
    break;                                                  
    /* BREAK State ON */
   case TNET_COM_SET_CONTROL_BREAK_ON:
    tcsendbreak(port_fd, 0);
    break;
    /* BREAK State OFF - is there really a thing as "break off"?*/
   case TNET_COM_SET_CONTROL_BREAK_OFF:
    break;
    /* DTR Signal State ON */
   case TNET_COM_SET_CONTROL_DTR_ON:
    line |= TIOCM_DTR;
    break;
    /* DTR Signal State OFF */
   case TNET_COM_SET_CONTROL_DTR_OFF:
    line &= ~TIOCM_DTR;
    break;
    /* RTS Signal State ON */
   case TNET_COM_SET_CONTROL_RTS_ON:
    line |= TIOCM_RTS;
    break;
    /* RTS Signal State OFF */
   case TNET_COM_SET_CONTROL_RTS_OFF:
    line &= ~TIOCM_RTS;
    break;
    /* No Flow Control (inbound) */
   case TNET_COM_SET_CONTROL_FLOWIN_NONE:
    port_info.c_iflag &= ~IXOFF;
    break;
    /* XON/XOFF Flow Control (inbound) */
   case TNET_COM_SET_CONTROL_FLOWIN_XOFF:
    port_info.c_iflag |= IXOFF;
    break;
    /* HARDWARE Flow Control (inbound) */
   case TNET_COM_SET_CONTROL_FLOWIN_RTSCTS:
    port_info.c_cflag |= CRTSCTS;
    break;

/* NOTE: How handle FLOW_CONTROL_DSRDTR ?     
    case TNET_COM_SET_CONTROL_FLOW_DSRDTR
     port_info.c_cflag |= TIOCM_DSR | TIOCM_DTR;
     break;
    case TNET_COM_SET_CONTROL_FLOW_DSRDTR_OFF
     port_info.c_cflag &= ~(TIOCM_DSR | TIOCM_DTR);
     break; */
     
    /* RI Signal State ON */
   case TNET_COM_SET_CONTROL_RI_ON:
    line |= TIOCM_RNG;
    break;
    /* RI Signal State OFF */
   case TNET_COM_SET_CONTROL_RI_OFF:
    line &= ~TIOCM_RNG;
    break;
    /* CD Signal State ON */
   case TNET_COM_SET_CONTROL_CD_ON:
    line |= TIOCM_CD;
    break;
    /* CD Signal State OFF */
   case TNET_COM_SET_CONTROL_CD_OFF:
    line &= ~TIOCM_CD;
    break;
   default:
    DLOGMSG(LOG_WARNING,("Requested unsupported flow control , "
           "setting to no flow control\n"));
    port_info.c_iflag &= ~IXON;
    port_info.c_iflag &= ~IXOFF;
    port_info.c_cflag &= ~CRTSCTS;
    break;
  }
    
  tcsetattr(port_fd, TCSA_MODE, &port_info);
  ioctl(port_fd, TIOCMSET, &line);
}

/* Set the serial port speed */ 
void set_port_Speed(int port_fd, unsigned long baud_rate)
{
  struct termios port_info;
  speed_t speed;
  int i = 0;
  long min_diff = 0x7FFFFFF;
  long diff;
  
  int min_ix = 0;
  
  
  while (tcspeeds[i].code != 0 && tcspeeds[i].baud != baud_rate)
  {
    diff = labs(tcspeeds[i].baud - baud_rate);
    if (diff < min_diff)
    {
      min_diff = diff;
      min_ix = i;
    }
    i++;
  }
  if (tcspeeds[i].baud == baud_rate)
  {
    speed = tcspeeds[i].code;
  }
  else
  {
    DLOGMSG(LOG_WARNING,"Unknown baud rate requested. Setting to nearest");
    
    speed = tcspeeds[min_ix].code;
  }
  D(printf("set_port_Speed: %lu %lu\n", baud_rate, (unsigned long)speed));
  
  tcgetattr(port_fd, &port_info);
  cfsetospeed(&port_info, speed);
  cfsetispeed(&port_info, speed);

  if (tcsetattr(port_fd, TCSA_MODE, &port_info) != 0)
  {
    D(printf("\nCall to tcsetattr(...) failed\n"));
    D(perror("tcsetattr()"));
  }
  else
  {
    D(printf("\nCall to tcsetattr(...) OK!!!\n"));
  }          
}



/****************** END OF FILE serutil.c ***********************************/



