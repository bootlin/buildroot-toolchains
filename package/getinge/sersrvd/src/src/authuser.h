/*!***************************************************************************
*!
*! FILE NAME  : authuser.h
*!
*! DESCRIPTION: Authenticate user
*!
*! FUNCTIONS  : authenticate_user
*! (EXPORTED)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Nov 14 2000  Johan Adolfsson    Initial version
*! $Log: authuser.h,v $
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
/* $Id: authuser.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef AUTHUSER_H
#define AUTHUSER_H

/****************** INCLUDE FILES SECTION ***********************************/

/****************** CONSTANT AND MACRO SECTION ******************************/

/****************** TYPE DEFINITION SECTION *********************************/

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/
int authenticate_user(const char *username, const char *passwd, 
                      const char *user_list);

#endif
/****************** END OF FILE authuser.h **********************************/
