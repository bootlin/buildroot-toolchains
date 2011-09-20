/*!***************************************************************************
*! 
*! FILE NAME  : sersrvd.h
*!
*! DESCRIPTION: Serial server application, types and functions.
*!
*! FUNCTIONS  : <The names of the exported functions.>
*! (EXPORTED)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Aug  3 1999  Johan Adolfsson    Initial version
*! $Log: sersrvd.h,v $
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
/* $Id: sersrvd.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef SERSRVD_H
#define SERSRVD_H

/****************** INCLUDE FILES SECTION ***********************************/

#include "webctrl_fifo.h"
#include "sersrvd_config.h"
#include "bfdutil.h"

/****************** CONSTANT AND MACRO SECTION ******************************/

/****************** TYPE DEFINITION SECTION *********************************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

extern buffered_fd *netbfd_cbs;   /* List of network connections */
extern buffered_fd *portbfd_cbs;  /* List of port connections (serial) */

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/


struct buffered_fd *
net_bfd_init_new(struct buffered_fd *portbfd, 
                 int new_fd);


/* Close a network bfd and remove it from netbfd_cbs list */
void net_bfd_close(struct buffered_fd *bfd);


/* Close a port bfd and remove it from portbfd_cbs list*/
void port_bfd_close(struct buffered_fd *bfd);


/* Initialise the port defined in the config. */
void init_port( sersrvd_config_type *config);



#endif /* SERSRVD_H */
/****************** END OF FILE sersrvd.h ***********************************/
