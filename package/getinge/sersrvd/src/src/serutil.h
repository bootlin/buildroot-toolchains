/*!***************************************************************************
*!
*! FILE NAME  : serutil.h
*!
*! DESCRIPTION: Serial port functions
*!
*! FUNCTIONS  : get_port_XX..
*! (EXPORTED)   set_port_XX..
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Aug  5 1999  Johan Adolfsson    Initial version
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: serutil.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef SERUTIL_H
#define SERUTIL_H

/****************** INCLUDE FILES SECTION ***********************************/
#include <termios.h>
#include <termio.h>

/****************** CONSTANT AND MACRO SECTION ******************************/

#define TNET_COM_PARITY_REQUEST 0
#define TNET_COM_PARITY_NONE    1
#define TNET_COM_PARITY_ODD     2
#define TNET_COM_PARITY_EVEN    3
#define TNET_COM_PARITY_MARK    4
#define TNET_COM_PARITY_SPACE   5

#define TNET_COM_STOPSIZE_REQUEST 0
#define TNET_COM_STOPSIZE1    1
#define TNET_COM_STOPSIZE2    2
#define TNET_COM_STOPSIZE1_5  3


#define TNET_COM_SET_CONTROL_REQUEST_FLOW 0
#define TNET_COM_SET_CONTROL_FLOW_NONE    1
#define TNET_COM_SET_CONTROL_FLOW_XONXOFF 2
#define TNET_COM_SET_CONTROL_FLOW_RTSCTS  3

#define TNET_COM_SET_CONTROL_REQUEST_BREAK  4
#define TNET_COM_SET_CONTROL_BREAK_ON       5
#define TNET_COM_SET_CONTROL_BREAK_OFF      6

#define TNET_COM_SET_CONTROL_REQUEST_DTR  7
#define TNET_COM_SET_CONTROL_DTR_ON       8
#define TNET_COM_SET_CONTROL_DTR_OFF      9

#define TNET_COM_SET_CONTROL_REQUEST_RTS  10
#define TNET_COM_SET_CONTROL_RTS_ON       11
#define TNET_COM_SET_CONTROL_RTS_OFF      12

#define TNET_COM_SET_CONTROL_REQUEST_FLOWIN 13
#define TNET_COM_SET_CONTROL_FLOWIN_NONE    14
#define TNET_COM_SET_CONTROL_FLOWIN_XOFF    15
#define TNET_COM_SET_CONTROL_FLOWIN_RTSCTS  16

#define TNET_COM_SET_CONTROL_FLOW_DCD    17
#define TNET_COM_SET_CONTROL_FLOWIN_DTR  18
#define TNET_COM_SET_CONTROL_FLOW_DSR    19

/* Special constant for Axis telnet std */
#define TNET_COM_SET_CONTROL_FLOW_XONXOFF_OFF 20
#define TNET_COM_SET_CONTROL_FLOW_RTSCTS_OFF  21
#define TNET_COM_SET_CONTROL_RI_ON       22
#define TNET_COM_SET_CONTROL_RI_OFF      23
#define TNET_COM_SET_CONTROL_CD_ON       24
#define TNET_COM_SET_CONTROL_CD_OFF      25
#define TNET_COM_SET_CONTROL_FLOW_DSRDTR      26
#define TNET_COM_SET_CONTROL_FLOW_DSRDTR_OFF  27

#define TNET_COM_LINESTATE_TIMEOUT_ERR 128
#define TNET_COM_LINESTATE_TSRE       64
#define TNET_COM_LINESTATE_THRE       32
#define TNET_COM_LINESTATE_BREAK_ERR  16
#define TNET_COM_LINESTATE_FRAME_ERR   8
#define TNET_COM_LINESTATE_PARITY_ERR  4
#define TNET_COM_LINESTATE_OVERRUN_ERR 2
#define TNET_COM_LINESTATE_DATAREADY   1



#define TNET_COM_MODEMSTATE_VALUEMASK 0xF0
#define TNET_COM_MODEMSTATE_DELTAMASK 0x0F
#define TNET_COM_MODEMSTATE_CD   128
#define TNET_COM_MODEMSTATE_RI    64
#define TNET_COM_MODEMSTATE_DSR   32
#define TNET_COM_MODEMSTATE_CTS   16
#define TNET_COM_MODEMSTATE_CD_DELTA  8
#define TNET_COM_MODEMSTATE_RI_TRAIL  4
#define TNET_COM_MODEMSTATE_DSR_DELTA 2
#define TNET_COM_MODEMSTATE_CTS_DELTA 1


/****************** TYPE DEFINITION SECTION *********************************/

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/

/* The values returned from these functions are coded acc. to RFC2217 */

/* Retrieves the port speed from port_fd */
unsigned long int get_port_Speed(int port_fd);

/* Retrieves the data size from port_fd */
unsigned char get_port_DataSize(int port_fd);

/* Retrieves the parity settings from port_fd */
unsigned char get_port_Parity(int port_fd);

/* Retrieves the stop bits size from port_fd */
unsigned char get_port_StopSize(int port_fd);
  
/* Retrieves the flow control status, including DTR and RTS status,
from port_fd */
unsigned char get_port_FlowControl(int port_fd, unsigned char Which);

/* Return the status of the modem control lines (DCD, CTS, DSR, RNG) */
unsigned char get_port_state(int port_fd, unsigned char PMState);

/* Set the serial port data size */
void set_port_DataSize(int port_fd, unsigned char DataSize);

/* Set the serial port parity */
void set_port_Parity(int port_fd, unsigned char Parity);

/* Set the serial port stop bits size */
void set_port_StopSize(int port_fd, unsigned char StopSize);

/* Set the port flow control and DTR and RTS status */
void set_port_FlowControl(int port_fd, unsigned char flow_control);

/* Set the serial port speed */ 
void set_port_Speed(int port_fd, unsigned long baud_rate);

int string2tiocmmask(const char *name);


#endif /* SERUTIL_H */
/****************** END OF FILE serutil.h ***********************************/

