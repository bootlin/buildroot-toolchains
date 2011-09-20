/*!***************************************************************************
*!
*! FILE NAME  : listener.h
*!
*! DESCRIPTION: TCP listener
*!
*! FUNCTIONS  : create_listener
*! (EXPORTED)   close_listeners
*!              listener_process_function
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Sep 11 2000  Johan Adolfsson    Initial version
*! $Log: listener.h,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.2  2000/10/05 14:05:25  johana
*! Review changes
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 2000, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: listener.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef LISTENER_H
#define LISTENER_H

/****************** INCLUDE FILES SECTION ***********************************/
#include "bfdutil.h"

/****************** CONSTANT AND MACRO SECTION ******************************/

/****************** TYPE DEFINITION SECTION *********************************/

struct listener_cb_type;

typedef int listener_process_function_type(bfd_command_type cmd,
                                           struct listener_cb_type *cb, 
                                           int new_fd);


typedef struct listener_cb_type
{
  int listenerfd;
  unsigned short portnbr;
  void *context;
  sersrvd_config_type *config;
  listener_process_function_type *listener_process_function;
  struct listener_cb_type *next;

} listener_cb_type;



/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/
extern listener_cb_type *listener_cb;

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/

listener_cb_type *create_listener(sersrvd_config_type *config,
                                  listener_process_function_type *proc_func);

void close_listeners(void);

int 
listener_process_function(bfd_command_type cmd,
                          struct listener_cb_type *cb, 
                          int new_fd);


#endif /* LISTENER_H */
/****************** END OF FILE listener.h **********************************/
