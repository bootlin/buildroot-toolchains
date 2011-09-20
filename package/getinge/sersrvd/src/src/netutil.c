/*!***************************************************************************
*!
*! FILE NAME  : netutil.c
*!
*! DESCRIPTION: Network utility functions.
*!
*! FUNCTIONS  : client_connect
*! (EXTERNAL)   url_to_host_port
*!              server_listen_tcp_udp
*!              non_block_read
*!              poll_listener
*!              
*! FUNCTIONS  : 
*! (LOCAL)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Aug  4 1999  Johan Adolfsson    Initial version
*! $Log: netutil.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.8  2002/10/09 06:07:20  starvik
*! Added keepalive option
*!
*! Revision 1.7  2002/08/19 11:32:18  johana
*! Fix gethostbyname error message.
*!
*! Revision 1.6  2001/07/04 10:03:43  johana
*! More logging of errors.
*!
*! Revision 1.5  2000/11/13 18:19:50  johana
*! Added from as parameter in poll_listener(), print error in client_connect().
*!
*! Revision 1.4  2000/10/05 14:05:25  johana
*! Review changes
*!
*! Revision 1.3  2000/09/15 17:11:38  johana
*! Removed transfer stuff, is in libtransfer when needed
*!
*! Revision 1.2  1999/12/15 17:28:16  johana
*! Uses uC-libc and some clean up.
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: netutil.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/

#include "netutil.h"
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include "debug.h"


/****************** CONSTANT AND MACRO SECTION ******************************/
#define DTMP(x)
#define D(x)

/****************** TYPE DEFINITION SECTION *********************************/

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

extern int keepalive;

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

/****************** FUNCTION DEFINITION SECTION *****************************/


/*#***************************************************************************
*#
*# FUNCTION NAME: client_connect
*#
*# PARAMETERS   : const char *host
*#                u_short portnr, 
*#                int type - SOCK_STREAM or SOCK_DGRAM 
*#
*# RETURNS      : int - filedescriptor
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Opens TCP or UDP connection to host:portnr, 
*#                return filedescriptor > 0 if ok.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Aug  4 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
int client_connect(const char *host, u_short portnr, int type)
{
  int s = 0;
  struct sockaddr_in server;  
  struct hostent *hp;  
  hp = gethostbyname(host);
  if (hp != NULL)
  {
    bzero((char *) &server, sizeof server);
    bcopy(hp->h_addr, (char *) &server.sin_addr, hp->h_length);
    server.sin_family = hp->h_addrtype;    /* Protocol family */
    server.sin_port = htons(portnr);/* Port number */
    s = socket(AF_INET, type, 0);
    if (s < 0)
    {
      fprintf(stderr, "error in socket\n");
      LOGMSG(LOG_ERR, "socket() error: errno=%i!\n", errno);
    }    
    if (connect(s, (struct sockaddr *) & server, sizeof server) < 0)
    {
      fprintf(stderr, "error in connect()\n");
      LOGMSG(LOG_ERR, "connect() error: errno=%i!\n", errno);
    }
  }
  else
  {
    LOGMSG(LOG_ERR, "gethostbyname() failed for %s, : errno=%i!\n", 
           host, errno);
    fprintf(stderr, "gethostbyname failed for %s : errno=%i!\n", 
            host, errno);
  }
  
  return s;
} /* client_connect */


/*#***************************************************************************
*#
*# FUNCTION NAME: url_to_host_port
*#
*# PARAMETERS   : const char *url,
*#                u_short    default_portnr,
*#                char       *host,
'#                u_short    *portnr_p
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Parses an url (with a default portnbr specified) and
*#                write the host part to a string and return the
*#                correct portnbr. (If no port is specified in the url it
*#                will return the supplied default one.)
*#                URL kan be in "host:portnr[path]" or "host portnr"
*#
*#----------------------------------------------------------------------------
*# HISTORY
*#
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Jan 12 1999  Johan Adolfsson    Initial version
*# Jul  8 1999  Johan Adolfsson    '\0' terminate IP string etc.
*# Aug 20 1999  Johan Adolfsson    Changed uword to u_short
*#
*#***************************************************************************/
void url_to_host_port(const char *url,
                      u_short    default_portnr,
                      char       *host,
                      u_short    *portnr_p)
{
  u_short portnr = default_portnr;
  char *portnr_sp;
  char *slash_p;
  assert(url);
  assert(host);
  assert(portnr_p);

  slash_p = strchr(url, '/');
  if (slash_p != NULL)
  {
    size_t len = slash_p - url;

    /* Copy until the '/' */
    strncpy(host, url, len);
    host[len] = '\0';
  }
  else
  {
    strcpy(host, url);
  }

  /* either 'host:portnr[/]' or 'host portnr' */
  portnr_sp = strchr(host, ':');
  if (portnr_sp == NULL)
  {
    portnr_sp = strchr(host, ' ');
  }
  if (portnr_sp != NULL)
  {
    portnr_sp[0] = '\0';  /* terminate host */
    portnr_sp++;
    portnr = (u_short)atol(portnr_sp);
  }
  *portnr_p = portnr;
} /* url_to_host_port */


/*#***************************************************************************
*#
*# FUNCTION NAME: server_listen_tcp_udp
*#
*# PARAMETERS   : u_short listenerportnr, 
*#                int theType - SOCK_STREAM or SOCK_DGRAM 
*#
*# RETURNS      : int - filedescriptor
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Opens TCP or UDP listener
*#                return filedescriptor > 0 if ok.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Aug  4 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
int server_listen_tcp_udp(u_short listener_portnr, int theType)
{
  int s = 0;
  int len;
  
  struct sockaddr_in sa_in;
  struct sockaddr_in sa_server;

  bzero((char *) &sa_in, sizeof sa_in);
  s = socket(AF_INET, theType, IPPROTO_TCP);  
  sa_in.sin_port=htons(listener_portnr);
  /* Bind to any port and IP address */  
  if (bind(s, (struct sockaddr *) & sa_in, sizeof sa_in) < 0)
  {
    fprintf(stderr, "error %d in bind (%s)\n", errno, strerror(errno));
    LOGMSG(LOG_ERR, "error in bind: errno=%i!\n", errno);
    s = -1;
    
  }
  else
  {
    /* Locate the port number and report to user */
    len = sizeof sa_server;
    if (getsockname(s, (struct sockaddr *) & sa_server, &len) < 0)
    {
      fprintf(stderr, "error %d in getsockname\n", errno);
    }
  
    /* Accept any client */

    if (listen(s, 1) != 0)
    {
      LOGMSG(LOG_ERR, "listen failed: errno=%i!\n", errno);
    }
    fcntl(s, F_SETFL, O_NONBLOCK); 
  }
  
  return s;
} /* server_listen_tcp_udp */

/*#***************************************************************************
*#
*# FUNCTION NAME: non_block_read(
*#
*# PARAMETERS   : int g - filedescriptor
*#                char *buf
*#                int  buf_size
*#
*# RETURNS      : int - Number of bytes read.
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Read data fron fd.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Aug  4 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
int non_block_read(int fd, char *buf, int buf_size)
{
  long int ret = 0;
  fd_set fds;
  struct timeval timeout;
  timeout.tv_sec=0;
  timeout.tv_usec=10;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  ret = 0;
  if (select(fd + 1, &fds, 0, 0, &timeout)==1)
  {
    ret=read(fd, buf,buf_size);
    if (!ret)
    {
      ret = -1; /* force error */
    }
  }
  return ret;
} /* non_block_read */


/*#***************************************************************************
*#
*# FUNCTION NAME: poll_listener
*#
*# PARAMETERS   : int f - filedescriptor
*#                struct sockaddr_in *from 
*# RETURNS      : int - Created filedescriptor or < 0
*#                Fills in from parameter
*# 
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : If someone has connected to listener f, 
*#                return socket for the connection.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Aug  4 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
int poll_listener(int f, struct sockaddr *from)
{
  int g = 0;
  int len;
  int true = 1;
  
  fd_set fds;
  struct timeval timeout;
  
  timeout.tv_sec=0;
  timeout.tv_usec=10;

  bzero((char *) from, sizeof *from);  
  FD_ZERO(&fds);
  FD_SET(f,&fds);
  if (select(f + 1, &fds, 0, 0, &timeout) == 1)
  {
    len = sizeof *from;          
    g = accept(f, from, &len);
    if (g < 0)
    {
      LOGMSG(LOG_ERR, "error in accept: errno=%i!\n", errno);
      fprintf(stderr, "error in accept\n");
    }
    if (keepalive)
    {
      if (setsockopt(g, SOL_SOCKET, SO_KEEPALIVE, (void *)&true, sizeof(true)))
        perror("setsockopt");
    }
    
    DTMP(printf("Server connect!\n"));
  }
  else
  {
    LOGMSG(LOG_ERR, "poll_listener select() error: errno=%i!\n", errno);
  }
  
  return g;
} /* poll_listener */

/****************** END OF FILE netutil.c ***********************************/
