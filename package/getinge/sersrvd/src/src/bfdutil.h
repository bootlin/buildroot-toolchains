/*!***************************************************************************
*!
*! FILE NAME  : bfdutil.h
*!
*! DESCRIPTION: Buffered file descriptor handling.
*!
*! FUNCTIONS  :  buffered_fd_init
*! (EXPORTED)    buffered_write
*!               buffered_close
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Dec 13 1999  Johan Adolfsson    Initial version
*! $Log: bfdutil.h,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.10  2001/07/05 13:49:26  johana
*! Added rs485 mode by toggling RTS before and after write.
*! No kernel driver used, and not needed unless devices responds
*! to fast.
*!
*! Revision 1.9  2001/07/04 10:02:29  johana
*! Added write_errno and handle it.
*!
*! Revision 1.8  2001/02/01 20:34:56  johana
*! Updated debugging and telnet option handling
*!
*! Revision 1.7  2000/11/20 16:36:05  johana
*! Timeout now implemented
*!
*! Revision 1.6  2000/11/14 14:33:55  johana
*! Added support for user authentication and IP number verification.
*!
*! Revision 1.5  2000/10/05 14:05:25  johana
*! Review changes
*!
*! Revision 1.4  2000/09/15 15:57:48  johana
*! Moved masks to tnetopt.h
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: bfdutil.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef BFDUTIL_H
#define BFDUTIL_H

/****************** INCLUDE FILES SECTION ***********************************/
#include <string.h>
#include "webctrl_fifo.h"
#include "sersrvd_config.h"
#include "tnetopt.h"
#include <sys/time.h>

/****************** CONSTANT AND MACRO SECTION ******************************/


/****************** TYPE DEFINITION SECTION *********************************/
typedef enum
{
  BFD_CMD_READ, 
  BFD_CMD_WRITE, 
  BFD_CMD_EXCEPT, 
  BFD_CMD_CLOSE
} bfd_command_type;

typedef enum
{
  LS_LOGIN_OK, 
  LS_WAITING_FOR_USER, 
  LS_WAITING_FOR_PASSWD, 
  LS_LOGIN_FAILED
} login_state_type;

struct buffered_fd;


typedef int bfd_process_function_type(bfd_command_type cmd, 
                                      struct buffered_fd *bfd, 
                                      int fd);

typedef struct buffered_fd
{
  int fd;
  unsigned long rx_count; /* Num bytes received */
  unsigned long tx_count; /* Num bytes transmitted */
  struct timeval rx_time;
  struct timeval tx_time;
  struct webctrl_fifo_type *txfifo;
  char rxbuf[BUF_SIZE];
  struct buffered_fd *tobfd;
  int isatty; /* Local copy of isatty() result */
  unsigned char port_state; /* For serial ports only */
  sersrvd_config_type *config;
  tnet_handler_type tnet[1]; /* array to get pointer notation */
  bfd_process_function_type *bfd_process_function;
  login_state_type login_state;
  struct buffered_fd *next;
  int write_errno;
  int set_rts_after_write;
}buffered_fd;


/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/

/* Initialise bfd with buffer with size size */
bool buffered_fd_init(buffered_fd *bfd, size_t size);


/* Write to bfd 
 * Returns nuber of chars supplied that was written or buffered */
int buffered_write(buffered_fd *bfd, const unsigned char* buf, size_t size);

/* Close and free bfd, remove from list head */
void buffered_close(struct buffered_fd **head, struct buffered_fd *bfd);


#endif
/****************** END OF FILE bfdutil.h ***********************************/

