/*!***************************************************************************
*!
*! FILE NAME  : debug.h
*!
*! DESCRIPTION: Debug routines
*!
*! FUNCTIONS  : <The names of the exported functions.>
*! (EXPORTED)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Feb  1 2001  Johan Adolfsson    Initial version
*! $Log: debug.h,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.1  2001/02/01 20:34:56  johana
*! Updated debugging and telnet option handling
*! 
*!
*! ---------------------------------------------------------------------------
*! (C) Copyright 2001, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: debug.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef DEBUG_H
#define DEBUG_H

/****************** INCLUDE FILES SECTION ***********************************/
#include <stdlib.h>
#include <syslog.h>

/****************** CONSTANT AND MACRO SECTION ******************************/

extern int LogLevel;

#if 1
#define LOGMSG(level, format...) syslog(level, format)
#else
void MyLogMsg(int level, const char *format, ...);
#define LOGMSG(level, format...) MyLogMsg(level, format)
#endif

void logdata(int level, const unsigned char *data, size_t len, 
             const char *format, ...);

#if 0
#define LOGDATA(data, len, format...) logdata(LOG_DEBUG, (data), (len), format)
#else
#define LOGDATA(data, len, format...)
#endif

/****************** TYPE DEFINITION SECTION *********************************/

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/

#endif /* DEBUG_H */
/****************** END OF FILE debug.h *************************************/
