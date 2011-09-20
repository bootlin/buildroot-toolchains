/*!***************************************************************************
*!
*! FILE NAME  : serinit.c
*!
*! DESCRIPTION: Serial port initialisation
*!
*! FUNCTIONS  : serial_init_port
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
*! Aug  4 1999  Johan Adolfsson    Initial version
*! Oct 13 1999  Johan Adolfsson    O_NOCTTY is not supported in elinux
*! $Log: serinit.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.6  2000/10/05 14:05:25  johana
*! Review changes
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: serinit.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/

#include "serinit.h"
#include "serutil.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h> /* close */
#include <fcntl.h>
#include <sys/time.h>

/****************** CONSTANT AND MACRO SECTION ******************************/

/* Default value for the ports */
#define DEFAULT_BAUD 38400
#define DEFAULT_PARITY    TNET_COM_PARITY_NONE
#define DEFAULT_DATASIZE  8
#define DEFAULT_STOPSIZE  TNET_COM_STOPSIZE_1
#define DEFAULT_CONTROL  

const unsigned char DEFAULT_SET_CONTROL_CODES[] = 
{
  TNET_COM_SET_CONTROL_FLOW_NONE,
  TNET_COM_SET_CONTROL_BREAK_OFF,
  TNET_COM_SET_CONTROL_DTR_ON,
  TNET_COM_SET_CONTROL_RTS_ON,
  0
};



/****************** TYPE DEFINITION SECTION *********************************/




/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

/* The port_mode_type should match these */
const char * port_mode_type_strings[] = {
  "OFF",
  "COMMAND_MODE",
  "AUTO_CONNECT",
  "DEBUG",
  NULL
};


int comportfd[MAX_COM_PORTS] = {-1, -1, -1, -1};

const char* comportdev[MAX_COM_PORTS] = {
  "/dev/ttyS0",
  "/dev/ttyS1",
  "/dev/ttyS2",
  "/dev/ttyS3"
};

struct com_port_info_type com_port_info[MAX_COM_PORTS];


/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

/****************** FUNCTION DEFINITION SECTION *****************************/


int serial_init_port(int portnbr, 
                     port_mode_type mode,
                     const char *device, 
                     const char *name,
                     unsigned long speed,
                     unsigned char datasize,
                     unsigned char parity,
                     unsigned char stopsize,
                     const unsigned char *control
                     )
{
  int fd;
  struct com_port_info_type *ci = &com_port_info[portnbr];
  ci->port_mode = mode;
  
  strncpy(ci->device, device, MAX_DEVICE_LEN);
  ci->device[MAX_DEVICE_LEN] = '\0';
  strncpy(ci->port_name, name, MAX_PORTNAME_LEN);
  ci->device[MAX_PORTNAME_LEN] = '\0';
  fd = open(device, O_RDWR | O_NONBLOCK);
   /* Oct 13, 1999: O_RDWR | O_NOCTTY | O_NONBLOCK don't work on elinux
    *            but O_RDWR | O_NONBLOCK does.
    */
  ci->fd = fd;
  if (fd != -1)
  {
    /* Success */
    tcgetattr(fd, &ci->initial_settings);
    tcgetattr(fd, &ci->settings);
    /* Set the serial port to raw mode */
    cfmakeraw(&ci->settings);
    ci->settings.c_iflag = IGNBRK | IGNPAR;
    ci->settings.c_cflag |=CLOCAL;
    cfsetospeed(&ci->settings, B115200);
    cfsetispeed(&ci->settings, B115200);
    tcsetattr(fd, TCSANOW, &ci->settings);

    /* Configure port */

    set_port_Speed(fd, speed);
    set_port_DataSize(fd, datasize);
    set_port_Parity(fd, parity);
    set_port_StopSize(fd, stopsize);

    /* to Set the different flow control options and initial pin values,
       loop through the control bytes
    */
    if (control == NULL)
    {
      control = DEFAULT_SET_CONTROL_CODES;
    }
    {
      int i = 0;
      while (control[i] != 0)
      {
        set_port_FlowControl(fd, control[i]);
        i++;
      }
    }
    tcgetattr(fd, &ci->settings);

    
  }
  return fd;
}


/****************** END OF FILE serinit.c ***********************************/
