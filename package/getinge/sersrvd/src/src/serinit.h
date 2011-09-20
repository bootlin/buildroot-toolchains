/*!***************************************************************************
*!
*! FILE NAME  : serinit.h
*!
*! DESCRIPTION: Serial port initialisation.
*!
*! FUNCTIONS  : serial_init_port
*! (EXPORTED)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Aug  4 1999  Johan Adolfsson    Initial version
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: serinit.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef SERINIT_H
#define SERINIT_H

/****************** INCLUDE FILES SECTION ***********************************/
#include <termio.h>
#include <termios.h>

/****************** CONSTANT AND MACRO SECTION ******************************/
#define MAX_COM_PORTS 4
#define MAX_DEVICE_LEN 15 /* Room for "/dev/ttyS0", "/dev/modem" */
#define MAX_PORTNAME_LEN 15 /* Room for "/dev/ttyS0", "/dev/modem" */


/****************** TYPE DEFINITION SECTION *********************************/

/* The port_mode_type_strings[] should match theese */
typedef enum
{
  PORT_OFF, 
  COMMAND_MODE, 
  AUTO_CONNECT,
  PORT_DEBUG
} port_mode_type;

typedef struct com_port_info_type
{
  port_mode_type port_mode;
  char device[MAX_DEVICE_LEN + 1];       /* e.g. "dev/ttyS0" */
  char port_name[MAX_PORTNAME_LEN + 1];  /* e.g. "COM1" */
  struct termios initial_settings;
  struct termios settings;
  int fd;
} com_port_info_type;
/****************** GLOBAL VARIABLES DECLARATION SECTION *******************/
extern const char * port_mode_type_strings[];

extern struct com_port_info_type com_port_info[MAX_COM_PORTS];

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/
extern int comportfd[4];

int serial_init_port(int portnbr, 
                     port_mode_type mode,
                     const char *device, 
                     const char *name,
                     unsigned long speed,
                     unsigned char datasize,
                     unsigned char parity,
                     unsigned char stopsize,
                     const unsigned char *control
                     );


int
SetupSerial(int portnr, unsigned long baudRate);


#endif /* SERINIT_H */
/****************** END OF FILE serinit.h ***********************************/
