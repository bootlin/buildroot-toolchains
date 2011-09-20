/*!***************************************************************************
*!
*! FILE NAME  : debug.c
*!
*! DESCRIPTION: Debug routines
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
*! Feb  1 2001  Johan Adolfsson    Initial version
*! $Log: debug.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.2  2002/02/25 13:44:23  johana
*! Always log in hex
*!
*! Revision 1.1  2001/02/01 20:34:56  johana
*! Updated debugging and telnet option handling
*!
*!
*! ---------------------------------------------------------------------------
*! (C) Copyright 2001, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: debug.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/

#include "debug.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/****************** CONSTANT AND MACRO SECTION ******************************/
int LogLevel = LOG_DEBUG + 1;

/****************** TYPE DEFINITION SECTION *********************************/

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

/****************** FUNCTION DEFINITION SECTION *****************************/
void MyLogMsg(int level, const char *format, ...)
{
  char msg[100];
  
  va_list ap;
  if (level <= LogLevel)
  {
    va_start(ap, format);
    (void) vsnprintf(msg, 99, format, ap);
    printf(msg);
    syslog(level, msg);
    va_end(ap);
  }
}

void logdata(int level, const unsigned char *data, size_t len, 
             const char *format, ...)
{
  int i = 0;
  int o = 0;
  int c;
  static char msg[2000+5];
  va_list ap;
  va_start(ap, format);
  (void) vsnprintf(msg, 99, format, ap);
  va_end(ap);
  o = strlen(msg);
  while ((i<len) && (o < 2000))
  {
    c = *data;
    if ((c < 32) || (c >127))
    {
      c = 'x';
    }
    
    sprintf(&msg[o]," %c%02X", c,*data);
    o += 4;
    i++;
    data++;
  }
  msg[o] = '\0';
  syslog(level, msg);
}


/****************** END OF FILE debug.c *************************************/
