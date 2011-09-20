/*!***************************************************************************
*!
*! FILE NAME  : netutil.h
*!
*! DESCRIPTION: Network utility functions.
*!
*! FUNCTIONS  : client_connect
*! (EXPORTED)   url_to_host_port
*!              server_listen_tcp_udp
*!              non_block_read
*!              poll_listener
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Aug  4 1999  Johan Adolfsson    Initial version
*! $Log: netutil.h,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.5  2000/11/13 18:19:50  johana
*! Added from as parameter in poll_listener(), print error in client_connect().
*!
*! Revision 1.4  2000/10/05 14:05:25  johana
*! Review changes
*!
*! Revision 1.3  2000/09/15 17:11:37  johana
*! Removed transfer stuff, is in libtransfer when needed
*!
*! Revision 1.2  1999/12/15 17:28:16  johana
*! Uses uC-libc and some clean up.
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: netutil.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef NETUTIL_H
#define NETUTIL_H

/****************** INCLUDE FILES SECTION ***********************************/

#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>  /* IPPROTO_TCP */


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>

#include <netdb.h>
#include <errno.h>
#include <unistd.h> /* close */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <assert.h>

/****************** CONSTANT AND MACRO SECTION ******************************/

/****************** TYPE DEFINITION SECTION *********************************/

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/

/* Use u_short since portnumber is actually only 16 bits,
 * and u_short is defined in linux/types.h 
 */

int client_connect(const char *host, u_short portnr, int type);
/* Opens TCP or UDP connection to host:portnr, return filedescriptor.
 * type is SOCK_STREAM or SOCK_DGRAM 
 */

void url_to_host_port(const char *url,
                      u_short    default_portnr,
                      char       *host,
                      u_short    *portnr_p);


int server_listen_tcp_udp(u_short listener_portnr, int theType);
/* theType could be SOCK_STREAM or SOCK_DGRAM */

int non_block_read(int fd, char *buf, int buf_size);

int poll_listener(int f, struct sockaddr *from);
/*  If someone has connected to listener f, 
 *   return socket for the connection.
 */

#endif /* NETUTIL_H */
/****************** END OF FILE netutil.h ***********************************/
