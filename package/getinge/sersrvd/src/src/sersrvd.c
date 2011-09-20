/*!***************************************************************************
*!
*! FILE NAME  : sersrvd.c
*!
*! DESCRIPTION: Serial server daemon.
*!              Usage: sersrvd [-f configfile]
*!              Default tries the following until found:
*!                ./sersrvd.conf, 
*!                ../etc/sersrvd.conf,
*!                /etc/sersrvd.conf in ., 
*!
*! FUNCTIONS  : 
*! (EXTERNAL)
*!
*! FUNCTIONS  : 
*! (LOCAL)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Aug  3 1999  Johan Adolfsson    Initial version
*! $Log: sersrvd.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.36  2002/10/09 06:07:20  starvik
*! Added keepalive option
*!
*! Revision 1.35  2002/08/19 11:31:03  johana
*! Don't print individual stuff.
*! Removed warnings.
*!
*! Revision 1.34  2002/06/02 00:41:10  johana
*! Log serial rx and tx count as well, when closing a network connection.
*! Clear the serial counters when closing the network connection.
*!
*! Revision 1.33  2001/09/21 12:59:14  johana
*! Initialise new_fd to -1
*!
*! Revision 1.32  2001/07/06 15:08:16  johana
*! Support for using RS-485 driver in kernel if it exists (and works).
*!
*! Revision 1.31  2001/07/05 13:49:26  johana
*! Added rs485 mode by toggling RTS before and after write.
*! No kernel driver used, and not needed unless devices responds
*! to fast.
*!
*! Revision 1.30  2001/07/04 10:14:45  johana
*! Added PARANOIA_SELECT_ACCEPT_CHECK to work around what appears to be
*! a bug in elinux (2.0.38) where select() doesn't set the FD_SET for
*! a listen()'ing socket even though accept() will return a new
*! connection!
*! (Happens iafter a connection is closed fast and data is still pending)
*!
*! Revision 1.29  2001/02/20 12:58:38  johana
*! Just log SIGINT (break), don't exit.
*!
*! Revision 1.28  2001/02/01 20:34:56  johana
*! Updated debugging and telnet option handling
*!
*! Revision 1.27  2001/01/23 15:28:05  johana
*! Ignore SIGPIPE.
*! Changed read() error handling, allow EINTR.
*!
*! Revision 1.26  2001/01/23 10:52:39  johana
*! More logging when closing and exiting
*!
*! Revision 1.25  2001/01/19 12:32:26  johana
*! Added PARANOIACHECK stuff to detect errors (disabled).
*!
*! Revision 1.24  2001/01/15 15:23:34  johana
*! Added port_check_status(), and call it from check_timeout() so we really
*! send the NOTIFY-MODEMSTATE messages when they change, not only when
*! a character is received.
*!
*! Revision 1.23  2001/01/11 21:44:35  johana
*! Enable rfc2217 if we ignored it before authorized.
*!
*! Revision 1.22  2001/01/11 17:01:39  johana
*! Do telnet option handling before processing user/password.
*! Added .._init_server() and .._init_cleint() functions to
*! send initial data if needed.
*! Server sends DO/WILL BINARY, DO/WILL SGA before login:
*! and DO/WILL COMPORT after authorisation is complete.
*!
*! Revision 1.21  2001/01/09 19:15:59  johana
*! Last minute typo
*!
*! Revision 1.20  2001/01/09 19:09:18  johana
*! * Allow fragmented username/password.
*! * Allow telnet options while waiting for username/password.
*! * Restore to configuration settings when connection closes.
*! * Server sends telnet option when someone connects, not the client.
*! * Decreased status line poll timeout from 1 second to 20 ms
*!   TODO: Use alarm from serial driver or something instead/as well.
*! * Added SUSPEND/RESUME handling for RFC2217 flowcontrol.
*!
*! Revision 1.19  2000/11/20 16:36:05  johana
*! Timeout now implemented
*!
*! Revision 1.18  2000/11/20 15:14:41  johana
*! Added num_connections and num_allowed_connections.
*! For now num_allowed_connections is set to 1 and not configurable.
*! Additional connections are closed directly.
*!
*! Revision 1.17  2000/11/14 14:33:55  johana
*! Added support for user authentication and IP number verification.
*!
*! Revision 1.16  2000/11/13 18:22:24  johana
*! Added use of syslog.
*! Send RFC2217 WILL option when we connect as a client.
*!
*! Revision 1.15  2000/10/05 14:05:25  johana
*! Review changes
*!
*! Revision 1.14  2000/09/18 09:08:35  johana
*! Clean up
*!
*! Revision 1.13  2000/09/15 17:15:07  johana
*! Added init_port
*!
*! Revision 1.12  2000/09/15 16:01:56  johana
*! Masks in tnet
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: sersrvd.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/


#include "sersrvd.h"

#include "webctrl_fifo.h"
#include "netutil.h"
#include <arpa/inet.h> /* inet_ntoa */
#include "serutil.h"
#include "serinit.h"
#include "tnetopt.h"
#include "sersrvd_config.h"
#include <signal.h>
#include "rfc2217.h"
#include "axcomp.h"
#include "listener.h"
#include "authip.h"
#include "authuser.h"
#include "linux/version.h"

/****************** CONSTANT AND MACRO SECTION ******************************/

#define REVISION "$Revision: 1.1 $"
#define MYMAX(a, b) (((a) > (b))?(a):(b))

#define DTMP(x)
#define D(x)
#define DTNET(x) 
#define DREAD(x)
#define DWRITE(x) 
#define DLISTN(x) 
#define DCLIENT(x)
#define DCLOSE(x)
#define DAUTH(x)
#define DSTATE(x) 
#define DERROR(x) x
#define DLOG(fd, s) dlogwrite(3, fd, s, strlen(s))

#define CTRL_D_ON_PORT_QUITS 0
#define CTRL_D_ON_NET_QUITS 0

extern void dlogwrite(int type, int fd, const char *buf, ssize_t size);

const char *config_file_name = NULL;

/* Lots of logging.. */
#define L2(x)

#if 0
#define PARANOIACHECK(x) x
#define PARANOIACHECKBUF(buf, len) if (memchr(buf, 0, len)){  \
  LOGMSG(LOG_INFO, "PARANOIA sersrvd: %lu %s\n", __LINE__, buf);}
#else
#define PARANOIACHECK(x)
#define PARANOIACHECKBUF(buf, len)
#endif


#ifndef PARANOIA_SELECT_ACCEPT_CHECK
#if (LINUX_VERSION_CODE < 0x020400)
/* Problem exist in elinux 2.0.38, but not in our 2.4 port */
#define PARANOIA_SELECT_ACCEPT_CHECK 1
#else
#define PARANOIA_SELECT_ACCEPT_CHECK 0
#endif
#endif /* PARANOIA_SELECT_ACCEPT_CHECK */

/****************** TYPE DEFINITION SECTION *********************************/



/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

static 
void port_check_status(struct buffered_fd *portbfd);


static int 
handle_ports(fd_set *readfds,
             fd_set *writefds,
             fd_set *exceptfds,
             int *quit);

static void handle_sockets_loop(void);
static void check_timeout(void);


static void exit_function(void);
static void free_resources(void);

/* Function called on many signals */
static void signal_function(int action);
static void signal_do_nothing(int action);

static void breakhandler_function(int action);

static void signal_rereadconfig(int action);

/* Process functions */
static int bfd_process_port(bfd_command_type cmd, 
                            struct buffered_fd *portbfd, 
                            int fd);

static int bfd_process_net(bfd_command_type cmd, 
                           struct buffered_fd *bfd, 
                           int fd);

  

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/



buffered_fd *netbfd_cbs = NULL;   /* List of network connections */
buffered_fd *portbfd_cbs = NULL;  /* List of port connections (serial) */

struct termios stdin_initial_setting;
int keepalive = 0;

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/
static int global_break = 0;
static int global_signal_handled = 0;

static int last_signal_action = 0;
static int do_restart;

static int use_stdin_as_netport = 
#ifdef ELINUX
  0;
#else
  0;
#endif


static char glob_buf[BUF_SIZE+1];
/****************** FUNCTION DEFINITION SECTION *****************************/


/*#***************************************************************************
*#
*# FUNCTION NAME: handle_ports
*#
*# PARAMETERS   : fd_set *readfds,
*#                fd_set *writefds,
*#                fd_set *exceptfds,
*#                int *quit
*#
*# RETURNS      : Number of sockets handled
*#                May update *quit (but doesn't)
*#
*# SIDE EFFECTS : Whatever processing of incoming data might do.
*#
*# DESCRIPTION  : Handle all port/device file descriptors that are set
*#                in any of the fd_sets provided.
*#                Sends pending data and processes incoming data.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 16 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
static int 
handle_ports(fd_set *readfds,
             fd_set *writefds,
             fd_set *exceptfds,
             int *quit)
{
  buffered_fd *portbfd = portbfd_cbs;
  int num_set = 0;
  
  while (portbfd != NULL)
  {
    if (portbfd->fd >= 0) /* Allow stdin */
    {
      int fd = portbfd->fd;

      if (FD_ISSET(fd, readfds))
      {
        DREAD(printf("Ok to read Port %i\n!", fd));
        fd = portbfd->bfd_process_function(BFD_CMD_READ, portbfd, fd);
        num_set++;
      }
      if (FD_ISSET(fd, writefds))
      {
        DWRITE(printf("Ok to write Port %i\n!", fd));
        DLOG(portbfd->fd, "# WRITE port pending #");
        buffered_write(portbfd, NULL, 0); /* Send pending */
        num_set++;
      }          
      if (FD_ISSET(fd, exceptfds))
      {
        DLOG(portbfd->fd, "# EXCEPT! #");
        printf("Except on Port %i\n!", fd);
        /* Close and reopen port! */
        close(portbfd->fd);
        portbfd->fd = open(portbfd->config->device, O_RDWR | O_NONBLOCK);
        
        num_set++;
      }          
      port_check_status(portbfd);
    } /* port */
    portbfd = portbfd->next;
  } /* while */
  
  return num_set;
} /* handle_ports */


/*#***************************************************************************
*#
*# FUNCTION NAME: handle_sockets_loop
*#
*# PARAMETERS   : None
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : Yes - creates new connections etc.
*#
*# DESCRIPTION  : Main loop that handle the network connections/sockets + 
*#                listener sockets + device sockets/filehandles.
*#                Sets up the select() call properly and take actions.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 16 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
static void 
handle_sockets_loop()
{
  int fdmax = 0;
  int selcode;
 
  fd_set readfds;
  fd_set writefds;
  fd_set exceptfds;
  struct timeval timeout;
  int quit = 0;
  
  while (!quit && !global_break)
  {
    /* Init fd sets */
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 20000;
    fdmax = 0;
    /* Add listeners */
    {
      listener_cb_type *cb = listener_cb;
      while (cb != NULL)
      {
        DTMP(printf("listener: %i@%d for %s\n",
                    cb->listenerfd, cb->portnbr, cb->config->port_name));
        DTMP(LOGMSG(LOG_INFO, "listener: %i@%d for %s\n",
                    cb->listenerfd, cb->portnbr, cb->config->port_name));
        FD_SET(cb->listenerfd, &readfds);
        fdmax = MYMAX(fdmax, cb->listenerfd);
        cb = cb->next;
      }
    }
    
    /* Add port */
    {
      struct buffered_fd *portbfd = portbfd_cbs;
      portbfd = portbfd_cbs;
      while (portbfd != NULL)
      {
        if (portbfd->fd >= 0) /* allow stdin */
        {
          int fd = portbfd->fd;
          DTMP(printf("port : %i (%s %s )\n",
                      fd, 
                      portbfd->config->port_name, 
                      portbfd->config->device));
          if (portbfd->tobfd && portbfd->tobfd->tnet->rfc2217_enabled)
          {
            /* Support SUSPEND/RESUME: Don't read port id were not supposed
             * to send it.
             */
            if (portbfd->tobfd->tnet->rfc2217_flow_to_net_ok)
            {
              FD_SET(fd, &readfds);
            }
          }
          else
          {
            FD_SET(fd, &readfds);
          }
          
          if (webctrl_fifo_num_unread(portbfd->txfifo) != 0)
          {
            /* We want to write to this fd */      
            FD_SET(fd, &writefds);
          }
          
          FD_SET(fd, &exceptfds);  /* We can get execptions when other
                                      applications opens the port - but how to handle them? */
          fdmax = MYMAX(fdmax, fd);
        }    
        portbfd = portbfd->next;
      }
    }
    
    {
      /* Add network sockets */    
      struct buffered_fd *netbfd = netbfd_cbs;
      while (netbfd != NULL)
      {
        int fd = netbfd->fd;
        DTMP(printf("net  : %i (%s)\n",fd, 
                    netbfd->config?netbfd->config->port_name:"?"));
        if (fd >= 0)
        {
          if (netbfd->tobfd != NULL)
          {
            if (webctrl_fifo_num_unread(netbfd->tobfd->txfifo) == 0)
            {
              /* Buffer empty - we want to read from this fd */
              FD_SET(fd, &readfds);
            }
            else
            {
              tnet_handler_struct_type *tnet = netbfd->tnet;
              if (tnet->rfc2217_enabled && tnet->rfc2217_flow_to_net_ok)
              {
                if (tnet->is_server)
                {
                  rfc2217_send_flow_command(tnet, COMC_FLOWCONTROL_SUSPEND);
                }
                else
                {
                  rfc2217_send_flow_command(tnet, COMS_FLOWCONTROL_SUSPEND);
                }                    
                tnet->rfc2217_flow_to_net_ok = FALSE;
                LOGMSG(LOG_INFO, "Flowcontrol: sent SUSPEND\n");
              }
              DLOG(netbfd->tobfd->fd, "# SKIP READ net #");
            }
          }
          
          if (webctrl_fifo_num_unread(netbfd->txfifo) != 0)
          {
            /* We want to write to this fd */
            FD_SET(fd, &writefds);
            DLOG(netbfd->tobfd->fd, "# FD WRITE port#");
          }
          
          /*FD_SET(fd, &exceptfds); */ /* Don't know what exceptions there 
           * might be*/
          fdmax = MYMAX(fdmax, fd);
        }
        netbfd = netbfd->next;
      }
    }
    
    D(printf("fdmax: %i\n", fdmax));
    global_signal_handled = 0;
    if ((selcode = select(fdmax + 1,
                          &readfds,
                          &writefds,
                          &exceptfds,
                          &timeout)) != 0)
    {
      /* Something happend */
      DTMP(printf("select returned %i\n", selcode));
      if (selcode == -1 && !global_signal_handled)
      {
        LOGMSG(LOG_INFO, "select() return -1, errno: %i", errno);
        DERROR(perror("sersrvd select(): "));
        quit = 1;
      }
      
      while (selcode > 0)
      {
        DTMP(printf("Outer loop: selcode: %i \n", selcode));
        
        /* Check listeners */
        {
          listener_cb_type *cb = listener_cb;
          
          while (cb != NULL)
          {
            if (cb->listenerfd > 0)
            {
              struct sockaddr_in from;
              int len = sizeof(from);
              int new_fd = -1;

              DTMP(printf("Check listener\n"));
              
              if (FD_ISSET(cb->listenerfd, &readfds))
              {
                new_fd = accept(cb->listenerfd, (struct sockaddr*)&from, &len);
                DLISTN(printf("Listener %i\n!", cb->listenerfd));
                /*new_fd = poll_listener(cb->listenerfd, 
                                       (struct sockaddr*)&from);
                */
                selcode--;
              }
#if PARANOIA_SELECT_ACCEPT_CHECK
              else
              {
                /* Check anyway, something is fishy... */
                new_fd = accept(cb->listenerfd, (struct sockaddr*)&from, &len);
                if (new_fd > 0)
                {
                  LOGMSG(LOG_DEBUG, "## accept() ok anyway!");
                }
                else if(errno != EAGAIN)
                {
                  LOGMSG(LOG_DEBUG, "## accept error: errno=%i",errno);
                }
              }
#endif /* PARANOIA_SELECT_ACCEPT_CHECK */
              
              if (new_fd > 0)
              {
                LOGMSG(LOG_INFO, "Port %s server %i connected from %s", 
                       cb->config->port_name, new_fd, 
                       inet_ntoa(from.sin_addr));
                /* Authentication: Check if IP is allowed */
                if (cb->config->allowed_ip_addresses != NULL)
                {
                  int ok;
                  DAUTH( printf("Verify IP: %s against %s\n", 
                                inet_ntoa(from.sin_addr),
                                cb->config->allowed_ip_addresses));
                  ok=authenticate_ip_address(cb->config->allowed_ip_addresses,
                                             from.sin_addr);
                  if (!ok)
                  {
                    LOGMSG(LOG_INFO, "Unauthorised IP: %s", 
                           inet_ntoa(from.sin_addr));
                    close(new_fd);
                    new_fd = -1;
                  }
                }
              }
              if (new_fd > 0)
              {
                new_fd = cb->listener_process_function(BFD_CMD_READ,
                                                       cb, new_fd);
              }

              if (FD_ISSET(cb->listenerfd, &exceptfds))
              {
                DLISTN(printf("Listener except %i\n!", cb->listenerfd));
                LOGMSG(LOG_DEBUG, "Listener except! %i", 
                       cb->listenerfd);
                selcode--;
              }          
            }
            cb = cb->next;
          } /* while */
        } /* listeners */
        
        selcode -= handle_ports(&readfds, &writefds, &exceptfds, &quit);
        DTMP(printf("after handle_ports: selcode: %i\n", selcode));
        
        {
          struct buffered_fd *netbfd = netbfd_cbs;
          while (selcode > 0 && netbfd != NULL)
          {
            int fd = netbfd->fd;          
            DTMP(printf("Inner loop: selcode: %i \n",
                        selcode));
            if (fd >= 0)
            {
              if (FD_ISSET(fd, &readfds))
              {
                DREAD(printf("Ok to read %i\n!", fd));
                fd = netbfd->bfd_process_function(BFD_CMD_READ, 
                                                  netbfd,
                                                  fd);
                
                selcode--;
              }
              else
              {
                tnet_handler_struct_type *tnet = netbfd->tnet;
                /* No data, check if we have sent SUSPEND and if we should 
                * send RESUME 
                */
                if (!tnet->rfc2217_flow_from_net_ok && /* We sent SUSPEND and*/
                    webctrl_fifo_num_free(netbfd->tobfd->txfifo))/* not full */
                {
                  if (tnet->is_server)
                  {
                    rfc2217_send_flow_command(tnet, COMC_FLOWCONTROL_RESUME);
                  }
                  else
                  {
                    rfc2217_send_flow_command(tnet, COMS_FLOWCONTROL_RESUME);
                  }
                  tnet->rfc2217_flow_from_net_ok = TRUE;
                  LOGMSG(LOG_INFO, "Flowcontrol: sent RESUME\n");
                }
              }
            }
            if (selcode > 0 && fd >= 0)
            {
              if (FD_ISSET(fd, &writefds))
              {
                DWRITE(printf("Ok to write: %i\n!", fd));
                buffered_write(netbfd, NULL, 0); /* Send pending */
                /* Write might trigger a close */
                fd = netbfd->fd;                              
                
                selcode--;
              }
            }
            if (selcode > 0 && fd >= 0)
            {
              if (FD_ISSET(fd, &exceptfds))
              {
                printf("Exception: %i\n!", fd);
                selcode--;
              }                  
            }
            netbfd = netbfd->next;
          } /* while */
        }
      } /* while selcode */
    } /* select */
    check_timeout();
  } /* while */
  LOGMSG(LOG_INFO, "end of socket loop");
} /* handle_sockets_loop */

static void check_timeout(void)
{
  struct timeval now;
  struct buffered_fd *netbfd = netbfd_cbs;
  buffered_fd *portbfd = portbfd_cbs;
  struct buffered_fd *tmp;
  while( portbfd != NULL)
  {
    port_check_status(portbfd);
    portbfd = portbfd->next;
  }

  gettimeofday(&now, NULL);
  while(netbfd != NULL)
  {
    D(LOGMSG(LOG_INFO, "check_timeout %s %i s now: %d rx_time: %d tx_time %d", 
             netbfd->config->port_name, netbfd->config->timeout, now.tv_sec,
             netbfd->rx_time.tv_sec, netbfd->tx_time.tv_sec));
    if (netbfd->config->timeout > 0)
    {
      unsigned long sec = netbfd->rx_time.tv_sec;
      if (netbfd->tx_time.tv_sec > sec)
      {
        sec = netbfd->tx_time.tv_sec;
      }
      if (sec + netbfd->config->timeout < now.tv_sec)
      {
        LOGMSG(LOG_INFO, 
               "Port %s net (%i) timeout %lu s now: %lu rx_time: %lu tx_time %lu", 
               netbfd->config?netbfd->config->port_name:"?", netbfd->fd,
               (unsigned long)netbfd->config->timeout, 
               (unsigned long)now.tv_sec,
               (unsigned long)netbfd->rx_time.tv_sec, 
               (unsigned long)netbfd->tx_time.tv_sec);
        tmp = netbfd;
        netbfd = netbfd->next;
        net_bfd_close(tmp);
      }
      else
      {
        netbfd = netbfd->next;
      }
    }
    else
    {
      netbfd = netbfd->next;
    }
  }
}


void bfd_tnet_init_client(struct buffered_fd *bfd)
{
  /* Client don't do anything, the server does */
  bfd->tnet->is_server = FALSE;
}


void bfd_tnet_send_tnet_opt(struct buffered_fd *bfd)
{
  /* Check if telnet options are enabled, and then send
   * telnet option to negotiate use of RFC2217 
   * Server typically sends "DO"
   * We will send both "DO" and "WILL" and expect a WILL/WONT, DO/DONT,
   * when we get a DO or WILL back we will enable RFC2217 but know
   * that we already sent a WILL and skip that.
   * WILL ->
   *      <- DO/DONT
   * (DO -> 
   *      <- WILL/WONT)
   */

  if (bfd->config->use_telnet_options)
  {
    tnet_handler_struct_type *tnet = bfd->tnet;
    tnet_send_option(tnet, WILL, TELOPT_BINARY);
    tnet_send_option(tnet, DO, TELOPT_BINARY);
    tnet_send_option(tnet, WILL, TELOPT_SGA);
    tnet_send_option(tnet, DO, TELOPT_SGA);
    tnet_send_option(tnet, WILL, TELOPT_COM_PORT_OPTION);
    tnet_send_option(tnet, DO, TELOPT_COM_PORT_OPTION);
    /* We might have received the option but ignored it, check that: */
    if (tnet->neg_state[TELOPT_COM_PORT_OPTION] & (BW_DO_RECV | BW_WILL_RECV))
    {
      LOGMSG(LOG_INFO,"Telnet COM Port Control Enabled (was ignored)");
      tnet->rfc2217_enabled = TRUE;
      rfc2217_init_connection(tnet);
    }
  }
} /* bfd_tnet_send_tnet_opt */


void bfd_tnet_init_server(struct buffered_fd *bfd)
{
  bfd->tnet->is_server = TRUE;
  /* Authentication: Send "login: and expect username\r\n
   * then send "Password: "and expect password, check 
   * with for user authentication */
  if (bfd->config->allowed_users != NULL)
  {
    bfd->login_state = LS_WAITING_FOR_USER;
    /* Send these now, since DialOut/IP wizard like it.
     * Don't send DO/WILL COM_PORT until authorised. 
     */
    tnet_send_option(bfd->tnet, WILL, TELOPT_BINARY);
    tnet_send_option(bfd->tnet, DO, TELOPT_BINARY);
    tnet_send_option(bfd->tnet, WILL, TELOPT_SGA);
    tnet_send_option(bfd->tnet, DO, TELOPT_SGA);
    buffered_write(bfd, "login: ",7);
  }
  else
  {
    bfd_tnet_send_tnet_opt(bfd);
  }
}



/*#***************************************************************************
*#
*# FUNCTION NAME: net_bfd_init_new
*#
*# PARAMETERS   : struct buffered_fd *portbfd, - The port associated with 
*#               int new_fd                    - The filedescriptor if 
*#                                               already created
*#
*# RETURNS      : struct buffered_fd * - The created net port or NULL
*#
*# SIDE EFFECTS : Inserts new net_bfd in netbfd_cbs
*#
*# DESCRIPTION  : Create a new network connection associated with portbfd.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 16 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
struct buffered_fd *
net_bfd_init_new(struct buffered_fd *portbfd, 
                 int new_fd)
{
  struct buffered_fd *bfd = malloc(sizeof(*bfd));
  if (bfd != NULL)
  {
    if (!buffered_fd_init(bfd, BUF_SIZE))
    {
      free(bfd);
      bfd = NULL;
      LOGMSG(LOG_ERR, "Failed to init bfd in net_bfd_init_new!\n");
    }
    else
    {
      bfd->config = portbfd->config;
      bfd->fd = new_fd;
      bfd->bfd_process_function = bfd_process_net;
      /* Link them together: */
      bfd->tobfd = portbfd;
      portbfd->tobfd = bfd;
      /* Init fifo and tnet */
      /* Insert first */
      bfd->next = netbfd_cbs;
      netbfd_cbs = bfd;
      bfd->config->num_connections++;
      gettimeofday(&bfd->rx_time, NULL);
      bfd->tx_time = bfd->rx_time;
      bfd_tnet_init_server(bfd);
    }
  }
  else
  {
    DERROR(fprintf(stderr,"Failed to alloc bfd in net_bfd_init_new!\n"));
    LOGMSG(LOG_ERR, "Failed to alloc bfd in net_bfd_init_new!\n");
  }
  
  return bfd;
} /* net_bfd_init_new */




/*#***************************************************************************
*#
*# FUNCTION NAME: net_bfd_close
*#
*# PARAMETERS   : struct buffered_fd *bfd
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  :  Close a network bfd and remove it from netbfd_cbs list.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Sep 18 2000  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
void 
net_bfd_close(struct buffered_fd *bfd)
{
  unsigned long port_tx, port_rx;
  if (bfd->tobfd)
  {
    port_tx = bfd->tobfd->tx_count;
    port_rx = bfd->tobfd->rx_count;
    /* Lets clear these counters for each session */
    bfd->tobfd->tx_count = 0;
    bfd->tobfd->rx_count = 0;
  }
  else
  {
    port_tx = port_rx = 0;
  }
  
  DCLOSE(printf("  net: close(%i) rx: %lu tx: %lu\n", 
                bfd->fd, bfd->rx_count, bfd->tx_count);
         if (bfd->tobfd != NULL)
         {
           printf("     port(%i) rx: %lu tx: %lu\n", 
                bfd->tobfd->fd, bfd->tobfd->rx_count, bfd->tobfd->tx_count);
         }
         );  
  LOGMSG(LOG_INFO, "Port %s rx: %lu tx: %lu net close(%i) rx: %lu tx: %lu", 
         bfd->config?bfd->config->port_name:"?", 
         port_rx, port_tx,
         bfd->fd, bfd->rx_count, bfd->tx_count);
  if (bfd->config)
  {
    bfd->config->num_connections--;
    if (bfd->config->num_connections == 0)
    {
      /* Last network connection to the port, revert to default values */
      set_initial_config(bfd->config);
    }
  }
  
  buffered_close(&netbfd_cbs, bfd);

} /* net_bfd_close */

/*#***************************************************************************
*#
*# FUNCTION NAME: port_bfd_close
*#
*# PARAMETERS   : struct buffered_fd *bfd
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  :  Close a port bfd and remove it from portbfd_cbs list.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Sep 18 2000  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
void 
port_bfd_close(struct buffered_fd *bfd)
{
  DCLOSE(printf("  port: close(%i) rx: %lu tx: %lu\n", 
                bfd->fd, bfd->rx_count, bfd->tx_count));  
  LOGMSG(LOG_INFO, "Port %s port close(%i) rx: %lu tx: %lu", 
         bfd->config?bfd->config->port_name: "?", bfd->fd, 
         bfd->rx_count, bfd->tx_count);
  buffered_close(&portbfd_cbs, bfd);
} /* port_bfd_close */


static 
void port_check_status(struct buffered_fd *portbfd)
{
  /* It's a real serial port */
  unsigned char prev_state = portbfd->port_state;
  unsigned char curr_state;
  int fd = portbfd->fd;
  tnet_handler_type *tnet = portbfd->tobfd->tnet;
  if (!portbfd->isatty)
  {
    return;
  }
  if (portbfd->set_rts_after_write)
  {
    int val = 0;
    ioctl(fd, TIOCSERGETLSR, &val);  
    if (val & TIOCSER_TEMT)
    {
      /* All sent */
      val = TIOCM_RTS;
      /* Set RTS to enable RX mode */
      ioctl(fd, TIOCMBIS, &val);  
      portbfd->set_rts_after_write = 0;
    }
  }
  curr_state = get_port_state(fd, prev_state);

  /*printf("States prev 0x%X curr 0x%X\n", 
    prev_state, curr_state); */
  if ((curr_state & TNET_COM_MODEMSTATE_VALUEMASK) !=
      (prev_state & TNET_COM_MODEMSTATE_VALUEMASK))
  {
    /* State has changed! Update value and send it to those 
       who want's it */
    DSTATE(printf("State changed from 0x%X to 0x%X\n", 
                  prev_state, curr_state));
    LOGMSG(LOG_DEBUG, "%s Modem state changed: 0x%02X\n", 
           portbfd->config->port_name, curr_state);
    
    portbfd->port_state = curr_state;
    /* Send it if the mask is correct */
    if (portbfd->tobfd )
    {
      if (tnet->rfc2217_enabled && 
          ((curr_state & tnet->rfc2217_modem_state_mask & 
            TNET_COM_MODEMSTATE_VALUEMASK) !=
           (prev_state & tnet->rfc2217_modem_state_mask & 
            TNET_COM_MODEMSTATE_VALUEMASK)))
      {
        DSTATE(printf("Send status (RFC2217): 0x%X\n", 
                      curr_state & tnet->rfc2217_modem_state_mask));
        LOGMSG(LOG_DEBUG, "Send MODEMSTATE (RFC2217): 0x%X\n", 
               curr_state & tnet->rfc2217_modem_state_mask);
        rfc2217_send_byte_command( portbfd->tobfd->tnet, 
                                   COMS_NOTIFY_MODEMSTATE,
                                   (curr_state & 
                                    tnet->rfc2217_modem_state_mask));
      }
      if (tnet->axcomp_enabled && 
          ((prev_state ^ curr_state) & tnet->axcomp_status_on_mask) )
      {
        DSTATE(printf("Send status (AXCOMP): 0x%X\n", 
                      curr_state & tnet->axcomp_status_on_mask));
        axcomp_send_notify_modemstate(portbfd->tobfd->tnet,
                                      curr_state,
                                      (prev_state ^ curr_state));
      }
      
    }
  } /* state changed */  
}



/*#***************************************************************************
*#
*# FUNCTION NAME: bfd_process_port
*#
*# PARAMETERS   : bfd_command_type cmd, 
*#                struct buffered_fd *portbfd, 
*#                int fd
*#
*#
*# RETURNS      : int - The file descriptor (-1 if it has closed)
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Process data/events on the device port.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 16 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
static int 
bfd_process_port(bfd_command_type cmd, 
                 struct buffered_fd *portbfd, 
                 int fd)
{
  ssize_t num_read;

  D(printf("process port %s data:\n", portbfd->config->port_name));
  
  switch (cmd)
  {
   case BFD_CMD_EXCEPT:
    break;
    
   case BFD_CMD_WRITE:
    break;
    
   case BFD_CMD_READ:
   default:
    {
      unsigned char *buf = glob_buf;
      if (portbfd->tobfd == NULL || portbfd->tobfd->fd == 0)
      {
        /* No connection or stdin */
        if (portbfd->config->connect_to != NULL)
        {
          char host[50];
          u_short portnr;
          int fd;
          

          url_to_host_port(portbfd->config->connect_to, 
                           portbfd->config->listener_port,
                           host, &portnr);
          DCLIENT(printf("Connecting to: %s:%u\n", host, portnr));
          errno = 0;
          fd = client_connect(host, portnr, SOCK_STREAM);
          if (fd > 0)
          {
            struct buffered_fd *bfd = net_bfd_init_new(portbfd, fd);
            DCLIENT(printf("Connected ok\n"));
            if (bfd == NULL)
            {
              /* ok */
              DERROR(fprintf(stderr, "Failed to init bfd - close conn!\n"));
              close(fd);
            }
            else
            {
              LOGMSG(LOG_INFO, "Port %s client %i connect to : %s:%u", 
                     bfd->config->port_name, bfd->fd, host, portnr);
              bfd_tnet_init_client(bfd);
            }
          }
          else
          {
            DCLIENT(printf("Connection failed %i %s\n", 
                           errno, strerror(errno)));
          }
        } /* connect_to */
      }
      /* Read data */
      num_read = read(fd, buf, BUF_SIZE);
      if (num_read > 0)
      {
        portbfd->rx_count += num_read;
        buf[num_read]='\0';
        PARANOIACHECKBUF(buf, num_read);
        LOGDATA(buf, num_read,"%s port:%2i RX %i: ",
                portbfd->config->port_name, portbfd->fd, num_read);
      }
      else
      {
        LOGMSG(LOG_INFO, "read() from port returned %i errno: %i!\n", 
               num_read, errno);
        num_read = 0;
      }
      
#if CTRL_D_ON_PORT_QUITS
      if (num_read == 1 && buf[0] == 0x04 /* Ctrl+D */)
      {
        /* quit */
        *quit = 1;
      }
#endif
      DREAD(printf("Read %li bytes: %s \n", (long int)num_read, buf));
      switch (portbfd->config->port_mode)
      {
       case PORT_OFF:
        DERROR(fprintf(stderr, "PORT_OFF for %s!\n", 
                       portbfd->config->port_name));
        break;
       case COMMAND_MODE:
        if (portbfd->config->echo_command_mode_port || 
            portbfd->tnet->echo_opt)
        {
          buffered_write(portbfd, buf, num_read);
        }
        
        break;
       case AUTO_CONNECT:
        /* Process data - write transparently (with or without tnet escaping)
         */
        if (portbfd->tobfd)
        {
          if (portbfd->config->use_telnet_options)
          {
            tnet_send_IAC_escaped(portbfd->tobfd->tnet, buf, num_read);
          }
          else
          {
            PARANOIACHECKBUF(buf, num_read);
            buffered_write(portbfd->tobfd, buf, num_read);
          }
        }
        break;
       case PORT_DEBUG:
        break;
        default:
         break;
      } /* switch */

      /* Check modem state on serial ports */
      if (portbfd->fd > 0 && portbfd->tobfd)
      {
        port_check_status(portbfd);

      }
      
    } /* port */
  } /* switch */
  
  return fd;
}


/*#***************************************************************************
*#
*# FUNCTION NAME: bfd_process_net
*#
*# PARAMETERS   : bfd_command_type cmd, 
*#                struct buffered_fd *bfd, 
*#                int fd
*#
*#
*# RETURNS      : int - The file descriptor (-1 if it has closed)
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Process data/events on the network port.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 16 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
static int 
bfd_process_net(bfd_command_type cmd, 
                struct buffered_fd *bfd, 
                int fd)
{
  ssize_t num_read;
/*  ssize_t num_processed;*/
  ssize_t len_left;
  ssize_t chars_read;
  ssize_t chunk_len;
  unsigned char *buf = glob_buf;
  tnet_handler_struct_type *tnet = bfd->tnet;
  D(printf("process net %s data fd = %i:\n", bfd->config->port_name, fd));

  /* DEBUG: Clear the buffer */
  D(memset(buf,0,BUF_SIZE));
  errno = 0;
  num_read = read(fd, buf, BUF_SIZE);
  if (num_read > 0)
  {
    PARANOIACHECKBUF(buf, num_read);
    gettimeofday(&bfd->rx_time, NULL);
    bfd->rx_count += num_read;

    len_left = num_read;
    buf[num_read] = '\0';
    LOGDATA(buf, num_read,"%s net :%2i RX %i: ",
            bfd->config->port_name, bfd->fd, num_read);
    DREAD(printf("Read %li bytes: 0x%02X '%s' \n", 
                 (long int)num_read, buf[0], buf));
    chars_read = 1;
    L2(LOGMSG(LOG_DEBUG, "** proc_net: %u bytes\n", len_left));
    tnet->opt_state = IAC_normal;
    while (len_left > 0 && 
           (chars_read > 0))
    {
      chars_read = 0;
      chunk_len = len_left;
      L2(LOGMSG(LOG_DEBUG, "* proc_net: %u bytes IAC: %i\n", 
                len_left, tnet->opt_state));
      L2(LOGDATA(tnet->buffer, tnet->buf_len,
                 "  tnet %i : ",
                 tnet->buf_len));
      L2(LOGDATA(buf, chunk_len,
                 "  buf  %i : ",
                 chunk_len));
      /* Handle telnet options if enabled */
      if (len_left > 0 && bfd->config->use_telnet_options)
      {
        if (tnet->buf_len && buf != tnet->buffer)
        {
          L2(LOGMSG(LOG_DEBUG, "   buffer new data\n"));
          if (!tnet_add_to_buffer(tnet, buf, chunk_len))
          {
            tnet_clear_buffer(tnet);
            LOGMSG(LOG_DEBUG, "TNET: clear buffer!\n");
          }
          buf = tnet->buffer;
          chunk_len = len_left = tnet->buf_len;
          L2(LOGDATA(tnet->buffer, tnet->buf_len,
                     "  tnet %i : ",
                     tnet->buf_len));
        }
        

        if (tnet->opt_state == IAC_normal)
        {
          unsigned char *pos;
          L2(LOGMSG(LOG_DEBUG, " * IAC_Normal"));
          pos = memchr(buf, IAC, len_left);
          if (pos == NULL)
          {
            /* No IAC */
            L2(LOGMSG(LOG_DEBUG, "   No IAC - process data"));
            /* IAC state normal - just process the data later */
          }
          else
          {
            /* We have found start IAC, process data before IAC */
            L2(LOGMSG(LOG_DEBUG, "   Found IAC"));        
            DTNET(printf("** Parsing telnet options **\n"));
            DTNET(printf("RX: IAC (%u bytes)\n", len_left));
            chunk_len = pos - buf; /* process data before IAC */

            if (chunk_len == 0) /* Nothing to process, take care of telnet */
            {
              /* Now see if there's an other IAC */
              DTNET(printf("Handle telnet\n"));
              L2(LOGMSG(LOG_DEBUG, "   Handle telnet"));
              chunk_len = len_left;
              chars_read = tnet_handle_IAC(tnet, buf, chunk_len, 
                                           bfd->login_state != LS_LOGIN_OK);
              if (chars_read > 0)
              {
                L2(LOGMSG(LOG_DEBUG, "   parsed telnet: %i bytes", 
                          chars_read));
                /* We have parsed it complete */
                len_left -= chars_read;
                buf += chars_read;
                if (tnet->buf_len)
                {
                  tnet_used_buffer(tnet, chars_read);
                  L2(LOGDATA(tnet->buffer, tnet->buf_len,
                             "buffer %i : ",
                             tnet->buf_len));
                  buf = tnet->buffer;
                  len_left = tnet->buf_len;
                }
                continue;
              }
              else
              {
                /* Not enough data to parse */
                if (buf != tnet->buffer)
                {
                  L2(LOGMSG(LOG_DEBUG, "   buffer telnet data\n"));
                  if (!tnet_add_to_buffer(tnet, buf, chunk_len))
                  {
                    tnet_clear_buffer(tnet);
                    LOGMSG(LOG_DEBUG, "TNET: clear buffer!\n");
                  }
                  L2(LOGDATA(tnet->buffer, tnet->buf_len,
                             "buffer %i : ",
                             tnet->buf_len));
                  len_left = tnet->buf_len;
                  buf = tnet->buffer;
                  tnet->opt_state = IAC_normal;
                  
                }
                else
                {
                  L2(LOGMSG(LOG_DEBUG, "   already buffered!\n"));
                }
                
                
                continue;
              }
            }
            else
            {
              /* Process data before IAC */
              L2(LOGMSG(LOG_DEBUG, "   Process %i before IAC data", 
                        chunk_len));
            }
          }
        }
        else if (tnet->opt_state == TOS_IAC)
        {
          /* We have buffered telnet data */
          /* The data should be buffered to complete the telnet string */
          L2(LOGMSG(LOG_DEBUG, " * TOS_IAC"));          
          if (!tnet_add_to_buffer(tnet, buf, chunk_len))
          {
            tnet_clear_buffer(tnet);
            fprintf(stderr, "TNET: clear buffer!\n");
            LOGMSG(LOG_ERR, "TNET: clear buffer!\n");
          }
          L2(LOGDATA(tnet->buffer, tnet->buf_len,
                     "buffer %i : ",
                     tnet->buf_len));
          LOGMSG(LOG_DEBUG, "   Handle buffered telnet data: %i", 
                 tnet->buf_len);
          chars_read = tnet_handle_IAC(tnet, 
                                       tnet->buffer, tnet->buf_len,
                                       bfd->login_state != LS_LOGIN_OK);
          if (chars_read > 0)
          {
            LOGMSG(LOG_DEBUG, "   chars_read: %i from buffer", chars_read);
            tnet_used_buffer(tnet, chars_read);
            L2(LOGDATA(tnet->buffer, tnet->buf_len,
                       "buffer %i : ",
                       tnet->buf_len));
            buf = tnet->buffer;
            len_left = tnet->buf_len;
            continue;
          }
          else
          {
            L2(LOGMSG(LOG_DEBUG, "   still not enough telnet data: %i", 
                      tnet->buf_len));
          }
          chunk_len = 0; /* No more to process in this round */
        }
        else if (tnet->opt_state == TOS_PRINT_IAC)
        {
          /* Process single IAC as data in this round */
          L2(LOGMSG(LOG_DEBUG, " * DOUBLE IAC *"));
          chars_read = 0;
          chunk_len = 1;
          tnet->opt_state = IAC_normal;
        }
        else
        {
          LOGMSG(LOG_ERR, "** IAC: %i\n", tnet->opt_state);
        }
      }
      else
      {
        /* Don't use telnet options, process data below */
      }

      if ((bfd->login_state != LS_LOGIN_OK) && (chunk_len > 0))
      {
        unsigned char *cr;
        unsigned char *lf;
        int len;
        int copy_len;
        LOGMSG(LOG_INFO, "Parse user/pass %i \n", chunk_len);
        switch (bfd->login_state)
        {
         case LS_WAITING_FOR_USER: /* expect user[\r|\n|\r\n] */
                                   /* but allow telnet options (IAC) */
          if ((buf[0] == IAC) && bfd->config->use_telnet_options)
          {
            LOGMSG(LOG_INFO, "# IAC 0x%02X 0x%02X 0x%02X while waiting for username!\n",
                   buf[1], buf[2], buf[3]);
          }
          cr = memchr(buf, '\r', chunk_len);
          lf = memchr(buf, '\n', chunk_len);
          if (cr || lf)
          {
            if (cr)
            {
              chars_read = cr - buf;
              *cr = '\0';
            }
            if (lf)
            {
              if (lf > cr)
              {
                chars_read = lf-buf;
              }
              *lf = '\0';
            }
            chars_read++;
            len = strlen(tnet->username);
            copy_len = USERNAME_MAX_LEN-len;
            if (chunk_len < copy_len)
            {
              copy_len = chunk_len;
            }
            memcpy(&tnet->username[len], buf, copy_len);
            tnet->username[USERNAME_MAX_LEN] = '\0';
            LOGMSG(LOG_INFO,"Username: '%s' chars_read: %li", 
                   tnet->username, (long int)chars_read);
            DAUTH(printf("username: '%s' chars_read: %li\n", 
                         tnet->username, (long int)chars_read));
            buffered_write(bfd, "Password: ", 10);
            bfd->login_state = LS_WAITING_FOR_PASSWD;
          }
          else
          {
            len = strlen(tnet->username);
            copy_len = USERNAME_MAX_LEN-len;
            if (chunk_len < copy_len)
            {
              copy_len = chunk_len;
            }
            memcpy(&tnet->username[len], buf, copy_len);
          }
          
          break;

         case LS_WAITING_FOR_PASSWD:
          {
            char *passwd = tnet->password;
            LOGMSG(LOG_INFO,"Waiting for password");
            cr = strchr(buf, '\r');
            lf = strchr(buf, '\n');
            if (cr || lf)
            {
              if (cr)
              {
                chars_read = cr-buf;
                *cr = '\0';
              }
              if (lf)
              {
                if (lf > cr)
                {
                  chars_read = lf-buf;
                }
                *lf = '\0';
              }
              len = strlen(passwd);
              copy_len = PASSWORD_MAX_LEN-len;
              if (chunk_len < copy_len)
              {
                copy_len = chunk_len;
              }
              memcpy(&passwd[len], buf, copy_len);
              passwd[PASSWORD_MAX_LEN]='\0';
              DAUTH(printf("passwd: '%s' chars_read: %li\n", 
                           passwd, (long int)chars_read));
              if (authenticate_user(tnet->username, 
                                    tnet->password, 
                                    bfd->config->allowed_users)) 
              {
                bfd->login_state = LS_LOGIN_OK;
                LOGMSG(LOG_INFO,"User '%s' ok on %s.", 
                       tnet->username, bfd->config->port_name);
                bfd_tnet_send_tnet_opt(bfd);
              }
              else
              {
                bfd->login_state = LS_LOGIN_FAILED;
                LOGMSG(LOG_INFO, "Unauthorised user: %s for %s", 
                       tnet->username, 
                       bfd->config->port_name);
                net_bfd_close(bfd);
                fd = -1;
                chars_read += len_left;
              }
              memset(passwd, 0, PASSWORD_MAX_LEN); /* Clear password */
            }
            else
            {
              len = strlen(passwd);
              copy_len = PASSWORD_MAX_LEN-len;
              if (chunk_len < copy_len)
              {
                copy_len = chunk_len;
              }
              memcpy(&passwd[len], buf, copy_len);
            }
          }
          break;

         case LS_LOGIN_FAILED:
          LOGMSG(LOG_INFO, "Unauthorised user: %s for %s", 
                 tnet->username, 
                 bfd->config->port_name);
          net_bfd_close(bfd);
          fd = -1;
          chars_read += len_left;
          break;
          
         default:
          break;          
        }
        len_left -= chars_read;
        buf += chars_read; 
        continue;
      } /* LOGIN */

      L2(LOGMSG(LOG_INFO, "* data: %li net:%i rx: %lu tx: %lu IAC:%i\n", 
                (long int)chunk_len, bfd->fd, (unsigned long)bfd->rx_count, 
                (unsigned long)bfd->tx_count,
                tnet->opt_state));
      L2(LOGMSG(LOG_INFO, "         port:%i rx: %lu tx: %lu \n", 
                bfd->tobfd->fd, 
                (unsigned long)bfd->tobfd->rx_count, 
                (unsigned long)bfd->tobfd->tx_count));
      /* Process chunk_len bytes at buf */
      switch (bfd->config->port_mode)
      {
       case PORT_OFF:
        fprintf(stderr, "PORT_OFF for %s!\n", bfd->config->port_name);
        break;
       case COMMAND_MODE:
        {
          ssize_t num_written = 0;
#if CTRL_D_ON_NET_QUITS  /* TODO: Should we have Ctrl+C and Ctrl+D detection? */
          if (num_read == 1 && fd == 0 &&
              (buf[0] == 0x04 /* Ctrl+D */ || 
               buf[0] == 0x03 /* Ctrl+C */ ))
          {
            /* quit */
            global_break = 1;
          }
#endif        
          if (bfd->config->echo_command_mode_net ||
              tnet->echo_opt)
          {
            num_written = buffered_write(bfd, buf, chunk_len);
          }
          chars_read += num_written;
        }
        
        break;
       case AUTO_CONNECT:
        /* Process data - write transparently */
        if (bfd->tobfd)
        {
          ssize_t num_written;
          PARANOIACHECKBUF(buf, chunk_len);
          num_written = buffered_write(bfd->tobfd, buf, chunk_len);
          chars_read += num_written;
        }
        break;
       case PORT_DEBUG:
        /* Consume it! */
        chars_read += chunk_len;
        break;
        default:
         /* Consume it! */
         chars_read += chunk_len;
         break;
      } /* switch */

      len_left -= chars_read;
      buf += chars_read;
      if (tnet->buf_len)
      {
        tnet_used_buffer(tnet, chars_read);
        L2(LOGDATA(tnet->buffer, tnet->buf_len,
                   "buffer %i : ",
                   tnet->buf_len));
        buf = tnet->buffer;
        len_left = tnet->buf_len;
      }
      
      L2(LOGMSG(LOG_DEBUG, " proc_net: chunk: %li left: %lu chars_read:%lu\n", 
              (long int)chunk_len, len_left, chars_read));
    } /* while */

    D(printf("len_left: %li  chars_read: %li\n", 
             (long int)len_left, (long int)chars_read));
    
  }
  else
  {
    int read_errno = errno;
    LOGMSG(LOG_INFO, "%s net %i read %i bytes, errno: %i", 
           bfd->config->port_name, fd, num_read, read_errno);
    /* Reading 0 bytes or less means end of file or error */
    if ((num_read == 0) || 
        !((read_errno == EAGAIN) || (read_errno == EINTR)) )
    {
      DCLOSE(printf("** read %li bytes, errno: %i - closing..\n", 
                    (long int)num_read, read_errno));
#if 0
      if (read_errno == EPIPE)
      {
        listener_cb_type *cb = listener_cb;
        while((cb != NULL) && (cb->config != bfd->config))
        {
          cb = cb->next;
        }
        if (cb != NULL)
        {

          close(cb->listenerfd);
          cb->listenerfd = server_listen_tcp_udp(cb->portnbr, SOCK_STREAM);
          if (cb->listenerfd > 0)
          {
            int i = 1;
            LOGMSG(LOG_ERR, "Got EPIPE, restarting listener\n");
            i = setsockopt(cb->listenerfd, SOL_SOCKET, SO_REUSEADDR, 
                           &i, sizeof(i));
            if (i != 0)
            {
              LOGMSG(LOG_ERR, "setsockopt failed!\n");
            }
          }
          else
          {
            LOGMSG(LOG_ERR, "Got EPIPE, failed to restart listener\n");
          }
        }
        else
        {
          LOGMSG(LOG_ERR, "Got EPIPE but couldn't find listener!\n");
        }
      } /* EPIPE */
#endif
      net_bfd_close(bfd);
      fd = -1;
    }
  }
  return fd;
} /* bfd_process_net */


/*#***************************************************************************
*#
*# FUNCTION NAME: init_port
*#
*# PARAMETERS   :   sersrvd_config_type *config
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : Yes, creates listeners and port sockets
*#
*# DESCRIPTION  : Initialise the port defined in the config.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Sep 15 2000  Johan Adolfsson    Initial version, moved stuff from init_ports
*# 
*#***************************************************************************/
void init_port( sersrvd_config_type *config)
{
  char *dev = config->device;  
  struct buffered_fd *portbfd;
  portbfd = malloc(sizeof(*portbfd)) ;
  if (portbfd == NULL)
  {
    fprintf(stderr, "Out of mem, can't initialise!\n");
    exit(1);
  }
  memset(portbfd, 0, sizeof (*portbfd));
  /* Insert first */
  portbfd->next = portbfd_cbs;
  portbfd_cbs = portbfd;
  portbfd->fd = 0; /* 0 = stdin*/
  portbfd->txfifo = NULL;
  portbfd->tobfd = NULL;
  
  D(printf("Using device: '%s' as port '%s'\n", dev, config->port_name));
  if (strcmp(dev, "-") != 0)
  {
    /* Not standard input */
    portbfd->fd = serial_init_port(0, 
                                   AUTO_CONNECT,
                                   dev, config->port_name, 
                                   config->baudrate, 
                                   config->datasize, 
                                   config->parity, 
                                   config->stopsize,
                                   NULL
                                   );
  }
  
  if (portbfd->fd < 0)
  {
#if 0
    sersrvd_config_type *this_config = config;
#endif
    fprintf(stderr, "Can't open %s at %s (error %i)\n", 
            config->port_name, dev, portbfd->fd);
    perror("Can't open device");
    /*portbfd->fd = 0;*/
    
    config = config->next;
#if 0
    /* Remove this config since we can't open port anyway */
    if (prev_config == NULL)
    {
      sersrvd_config_cbs = config;
    }
    else
    {
      prev_config->next = config;
    }
    portbfd->config = NULL;
    free(this_config);
#endif
    port_bfd_close(portbfd);
    fprintf(stderr, "Port closed\n"); 
  }
  else
  {
    D(printf("init_port %s %s fd: %i\n",  
             config->port_name, dev, portbfd->fd));
    portbfd->isatty = isatty(portbfd->fd);
    portbfd->txfifo = webctrl_fifo_init(BUF_SIZE);
    if (portbfd->fd > 0)
    {
      int fd = portbfd->fd;
      portbfd->port_state = get_port_state(fd, 0);
      portbfd->port_state = get_port_state(fd, portbfd->port_state);
      portbfd->port_state = get_port_state(fd, portbfd->port_state);
      D(printf("port state: 0x%X\n", portbfd->port_state));
    }
    portbfd->config = config;
    config->portbfd = portbfd;
    set_initial_config(config);
    portbfd->bfd_process_function = bfd_process_port;
    /* Add the listener for this port */
    create_listener(config, listener_process_function);
  }
    
}
 


/*#***************************************************************************
*#
*# FUNCTION NAME: init_ports
*#
*# PARAMETERS   : int argc, char **argv
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : Yes, creates listeners and port sockets
*#
*# DESCRIPTION  : Initialise the ports defined in the config.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 16 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
void init_ports(int argc, char **argv)
{
  sersrvd_config_type *config = sersrvd_config_cbs;
  sersrvd_config_type *prev_config = NULL;
  while (config != NULL)
  {
    if (config->port_mode == PORT_OFF)
    {
      /* Skip this config, move to next: */
      prev_config = config;
      config = config->next;
      continue;
    }
    init_port(config);
    
    prev_config = config;
    config = config->next;
  } /* while */
} /* init_ports */


/*#***************************************************************************
*#
*# FUNCTION NAME: init_ipc
*#
*# PARAMETERS   : None
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Initialise the IPC communication with web server.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 16 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
void init_ipc(void)
{
  /* Not implemented yet... */
} /* init_ipc */


/*#***************************************************************************
*#
*# FUNCTION NAME: main
*#
*# PARAMETERS   : std argc, argv
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Main function
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Dec 16 1999  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
int main(int argc, char **argv)
{
  int arg = 1;
  openlog("sersrvd", LOG_PID, LOG_USER);
  
  printf("sersrvd "
         REVISION "\n"
         );
  LOGMSG(LOG_INFO, "starting " REVISION);
  
/*  printf("argv[0]: %s \n", argv[0]);*/
  while (arg < argc)
  {
    D(printf("argv[%i]: %s \n", arg, argv[arg]));
    if (strcmp(argv[arg],"-f") == 0)
    {
      arg++;
      config_file_name = argv[arg];
    }
    else if (!strcmp(argv[arg],"-keepalive"))
    {
      keepalive = 1;
    }
    else if ( !strcmp(argv[arg],"-h") || !strcmp(argv[arg],"--help") )
    {
      printf("Usage: sersrvd [-f configfile] [-keepalive]\n");
      printf("if no -f option given, uses ../etc/sersrvd.conf or "
             "/etc/sersrvd.conf\n");
      return 0;
    }
    

    arg++;
  }
  

  do {
    do_restart = 0;
    global_break = 0;
    atexit(exit_function);
    signal(SIGHUP,  signal_function);
    signal(SIGQUIT, signal_function);
    signal(SIGABRT, signal_function);
    signal(SIGPIPE, signal_do_nothing); /* SIGIGN */
    signal(SIGTERM, signal_function);
    signal(SIGUSR1, signal_rereadconfig);
    signal(SIGUSR2, signal_rereadconfig);
    
    signal(SIGINT, breakhandler_function);
    /* Read config */
    if (!sersrvd_config_read(config_file_name))

    {
      printf("Error reading config\n");
    }
    /* Initialise according to config */
    DTMP(printf("---------------\n"));
    init_ports(argc, argv);
    init_ipc();
    DTMP(printf("---------------\n"));
    

    tcgetattr(fileno(stdin), &stdin_initial_setting);
    /* Don't set the stdin port to raw mode */
    /*    cfmakeraw(&ti);
     *    tcsetattr(bfd->fd, TCSANOW, &ti);
     */    
    
    /* Maybe make first netfd stdin */
    if (portbfd_cbs != NULL && use_stdin_as_netport)
    {
      struct buffered_fd *portbfd = portbfd_cbs;
      struct buffered_fd *bfd = net_bfd_init_new(portbfd, fileno(stdin));
      struct termios ti;
      if (bfd != NULL)
      {
        tcgetattr(bfd->fd, &ti);
      }
      else
      {
        fprintf(stderr, "Failed to init stdin port (net)\n");
      }
    }
    
    /* This is the main loop */
    handle_sockets_loop();
  }while(do_restart);
  LOGMSG(LOG_INFO, "end of main loop");
  sersrvd_config_free();
  /* Clean up taken care of by exit_function */
  /* close listeners */
  return 0;
} /* main */



/*#***************************************************************************
*#
*# FUNCTION NAME: signal_function
*#
*# PARAMETERS   : int action
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Signal handler for many signals
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
static void signal_function(int action)
{
  DTMP(printf("signal! %i \n", action));
  last_signal_action = action;
  LOGMSG(LOG_INFO, "SIGNAL: %i!", action);
  if (action == SIGHUP)
  {
    do_restart = 1;
  }
  global_break = 1;  
  signal(SIGHUP,  SIG_DFL);
  signal(SIGQUIT, SIG_DFL);
  signal(SIGABRT, SIG_DFL);
  signal(SIGTERM, SIG_DFL);
  signal(SIGUSR1, SIG_DFL);
  signal(SIGUSR2, SIG_DFL);
  exit_function();
} /* signal_function */

static void signal_do_nothing(int action)
{
  LOGMSG(LOG_INFO, "SIGPIPE");
  signal(SIGPIPE, signal_do_nothing);
  global_signal_handled = 1;
}



/*#***************************************************************************
*#
*# FUNCTION NAME: signal_rereadconfig
*#
*# PARAMETERS   : int action
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Signal handler for USR1 and USR2, rereads config
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
static void signal_rereadconfig(int action)
{
  LOGMSG(LOG_INFO, "rereadconfig");  
  DTMP(printf("signal_rereadconfig! %i \n", action));
  if (action == SIGUSR1)
  {
    sersrvd_config_read(config_file_name);
    signal(SIGUSR1, signal_rereadconfig);
    global_signal_handled = 1;
  }
  else if (action == SIGUSR2)
  {
    sersrvd_config_read(config_file_name);
    signal(SIGUSR2, signal_rereadconfig);
    global_signal_handled = 1;
  }
  LOGMSG(LOG_INFO, "rereadconfig done");  
}


/*#***************************************************************************
*#
*# FUNCTION NAME: breakhandler_function
*#
*# PARAMETERS   : int action
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Signal handler for break
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
static void breakhandler_function(int action)
{
  DTMP(printf("break signal! %i \n", action));
  LOGMSG(LOG_INFO, "BREAK!");
 
  signal(SIGINT, breakhandler_function);
} /* breakhandler_function */


/*#***************************************************************************
*#
*# FUNCTION NAME: exit_function
*#
*# PARAMETERS   : 
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : The atexit function, clean up resources.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
static void exit_function(void)
{
  close_listeners();
  free_resources();
  /* tcsetattr(0, TCSANOW, &stdin_initial_setting); */
  LOGMSG(LOG_INFO, "EXIT!");
} /* exit_function */


/*#***************************************************************************
*#
*# FUNCTION NAME: free_resources
*#
*# PARAMETERS   : 
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Close ports, free resources.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 
*#***************************************************************************/
static void free_resources(void)
{
  while (netbfd_cbs != NULL)
  {
    net_bfd_close(netbfd_cbs);
  }
  while (portbfd_cbs != NULL)
  {
    port_bfd_close(portbfd_cbs);
  }
} /* free_resources */


/****************** END OF FILE sersrvd.c ***********************************/
