/*!***************************************************************************
*!
*! FILE NAME  : listener.c
*!
*! DESCRIPTION: TCP listener
*!
*! FUNCTIONS  : create_listener
*! (EXTERNAL)   close_listeners
*!              listener_process_function
*!
*! FUNCTIONS  : <The names of the local functions in this file.>
*! (LOCAL)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Sep 11 2000  Johan Adolfsson    Initial version
*! $Log: listener.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.5  2001/07/04 10:01:44  johana
*! More logging of errors.
*!
*! Revision 1.4  2000/11/21 17:36:47  johana
*! Added 2 LOGMSG
*!
*! Revision 1.3  2000/11/20 15:14:41  johana
*! Added num_connections and num_allowed_connections.
*! For now num_allowed_connections is set to 1 and not configurable.
*! Additional connections are closed directly.
*!
*! Revision 1.2  2000/10/05 14:05:25  johana
*! Review changes
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 2000, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: listener.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/

#include <stdio.h>

#include "unistd.h"
#include "listener.h"
#include <sys/socket.h>
#include "netutil.h"
#include "sersrvd.h"

/****************** CONSTANT AND MACRO SECTION ******************************/
#define DLISTN(x) 
#define D(x)

/****************** TYPE DEFINITION SECTION *********************************/

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/
listener_cb_type *listener_cb = NULL;

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

/****************** FUNCTION DEFINITION SECTION *****************************/

/*#***************************************************************************
*#
*# FUNCTION NAME: create_listener
*#
*# PARAMETERS   : sersrvd_config_type *config - the config to use
*#                listener_process_function_type *proc_func 
*#
*# RETURNS      : listener_cb_type * - pointer to allocated cb.
*#
*# SIDE EFFECTS : Puts the created cb in list listener_cb.
*#
*# DESCRIPTION  : Create a listener socket and put it together with
*#                the function pointer to use to process new connections
*#                in a list.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Oct  5 2000  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
listener_cb_type *create_listener(sersrvd_config_type *config, 
                                  listener_process_function_type *proc_func)
{
  u_short listener_port = config->listener_port;
  listener_cb_type *cb = NULL;
  if (listener_port != 0)
  {
    cb = malloc(sizeof (*listener_cb));
    if (cb != NULL)
    {
      cb->portnbr = listener_port;
      cb->listenerfd = server_listen_tcp_udp(cb->portnbr, SOCK_STREAM);
      
      if (cb->listenerfd > 0)
      {
        int i = 1;
        struct linger ling;
        ling.l_onoff = 1;
        ling.l_linger = 20; /* 1/100 th of seconds */
      
        i = setsockopt(cb->listenerfd, SOL_SOCKET, SO_REUSEADDR, 
                       &i, sizeof(i));
        if (i != 0)
        {
          LOGMSG(LOG_ERR, "setsockopt REUSEADDR failed! errno=%i \n", errno);
        }
        i = setsockopt(cb->listenerfd, SOL_SOCKET, SO_LINGER, 
                       &ling, sizeof(ling));
        if (i != 0)
        {
          LOGMSG(LOG_ERR, "setsockopt LINGER failed! errno=%i \n", errno);
        }
        /* Insert in list */
        cb->next = listener_cb;
        listener_cb = cb;
        
        D(printf("Listens at port %u, socket %i\n", 
                 cb->portnbr, cb->listenerfd));
      }
      cb->config = config;
      cb->listener_process_function = proc_func;
    }
  }
  return cb;
} /* create_listener */

/*#***************************************************************************
*#
*# FUNCTION NAME: close_listeners
*#
*# PARAMETERS   : None
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Close all listener sockets in list and free memory.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Oct  5 2000  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
void close_listeners(void)
{
  listener_cb_type *cb;
  while (listener_cb != NULL)
  {
    cb = listener_cb;
    DLISTN(printf("  close listener: %i\n", cb->listenerfd));
    close(cb->listenerfd);      
    listener_cb = listener_cb->next;
    free(cb);
  }
} /* close_listeners */


/*#***************************************************************************
*#
*# FUNCTION NAME: listener_process_function
*#
*# PARAMETERS   : bfd_command_type cmd,
*#                struct listener_cb_type *cb, 
*#                int new_fd
*#
*# RETURNS      : int - if >0, the socket of the new connection
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Create new bfd control block for the new_fd from the
*#                listener cb.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Oct  5 2000  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
int 
listener_process_function(bfd_command_type cmd,
                          struct listener_cb_type *cb, 
                          int new_fd)
{
  struct buffered_fd *portbfd = cb->config->portbfd;
  struct buffered_fd *bfd = NULL;
  if (cb->config->num_connections < cb->config->num_allowed_connections)
  {
    bfd = net_bfd_init_new(portbfd, new_fd);
    if (bfd == NULL)
    {
      LOGMSG(LOG_INFO, "Port %s close(%i) - failed to alloc bfd.", 
             cb->config->port_name, new_fd);
      fprintf(stderr, "Failed to create bfd for new connection!\n");
      close(new_fd);
      new_fd = 0;
    }
    else
    {
      D(printf("Created socket %i for %s\n", bfd->fd, cb->config->port_name));
    }
  }
  else
  {
    LOGMSG(LOG_INFO, "Port %s close(%i) - to many connections.", 
           cb->config->port_name, new_fd);
    close(new_fd);
    new_fd = 0;    
  }
  
  return new_fd;
} /* listener_process_function */


/****************** END OF FILE listener.c **********************************/
