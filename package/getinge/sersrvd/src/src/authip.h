/*!***************************************************************************
*!
*! FILE NAME  : authip.h
*!
*! DESCRIPTION: Authenticate IP address
*!
*! FUNCTIONS  : authenticate_ip_address
*! (EXPORTED)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Nov 14 2000  Johan Adolfsson    Initial version
*! $Log: authip.h,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.1  2000/11/14 14:33:55  johana
*! Added support for user authentication and IP number verification.
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 2000, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: authip.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef AUTHIP_H
#define AUTHIP_H

/****************** INCLUDE FILES SECTION ***********************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/****************** CONSTANT AND MACRO SECTION ******************************/

/****************** TYPE DEFINITION SECTION *********************************/

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/

int authenticate_ip_address(const char *ip_list,
                            struct in_addr ip);
/* Check if ip is present in ip_list */


#endif
/****************** END OF FILE authip.h ************************************/
