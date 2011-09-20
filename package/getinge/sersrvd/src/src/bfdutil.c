/*!***************************************************************************
*!
*! FILE NAME  : bfdutil.c
*!
*! DESCRIPTION: Buffered file descriptor handling.
*!
*! FUNCTIONS  : buffered_fd_init
*! (EXTERNAL)   buffered_write
*!              buffered_close
*!
*! FUNCTIONS  : buffered_fd_free
*! (LOCAL)      buffered_fd_close
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Dec 13 1999  Johan Adolfsson    Initial version
*! $Log: bfdutil.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.16  2002/06/02 00:39:36  johana
*! update_statistics when RS-485 write succeds.
*!
*! Revision 1.15  2002/02/25 13:43:19  johana
*! Clearer LOGDATA.
*!
*! Revision 1.14  2001/07/11 08:45:19  matsfg
*! Corrected spelling error
*!
*! Revision 1.13  2001/07/06 15:08:16  johana
*! Support for using RS-485 driver in kernel if it exists (and works).
*!
*! Revision 1.12  2001/07/05 13:49:26  johana
*! Added rs485 mode by toggling RTS before and after write.
*! No kernel driver used, and not needed unless devices responds
*! to fast.
*!
*! Revision 1.11  2001/07/04 10:02:29  johana
*! Added write_errno and handle it.
*!
*! Revision 1.10  2001/02/01 20:34:56  johana
*! Updated debugging and telnet option handling
*!
*! Revision 1.9  2001/01/19 12:32:26  johana
*! Added PARANOIACHECK stuff to detect errors (disabled).
*!
*! Revision 1.8  2000/11/20 16:36:05  johana
*! Timeout now implemented
*!
*! Revision 1.7  2000/11/14 14:33:55  johana
*! Added support for user authentication and IP number verification.
*!
*! Revision 1.6  2000/10/05 14:05:25  johana
*! Review changes
*!
*! Revision 1.5  2000/09/11 16:35:46  johana
*! Merged with changes by lenamn. Still some things to do.
*!
*! Revision 1.4  2000/09/11 11:38:40  johana
*! Removed warnings
*!
*! Revision 1.3  2000/02/23 11:50:39  johana
*! Fixed debug printouts and buffering
*!
*! Revision 1.2  1999/12/20 19:19:08  johana
*! Starting to handle telnet options and RFC2217
*!
*! Revision 1.1  1999/12/15 17:28:15  johana
*! Uses uC-libc and some clean up.
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: bfdutil.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/


#include "bfdutil.h"
#include <stdio.h>
#include <stdlib.h>
#include "netutil.h"
#include <termios.h>
#ifdef __CRIS__
#include <asm/ioctls.h>      /* RS485 support */
#ifndef TIOCSERSETRS485
#warning "### TODO: Add RS-485 support to kernel! "
#warning "### RS-485 works anyway unless timing is critical"
#endif
#endif

/****************** CONSTANT AND MACRO SECTION ******************************/

#define D(x)
#define DERROR(x) x
#define DBUFINFO(x)
#define DWRITE(x)

#if 0
#define DLOGWRITE_INCLUDED 1
#define DLOGWRITE(type, fd, buf, size) dlogwrite(type, fd, buf, size)
#define DLOGWRITE1(fd, buf, size) dlogwrite(1,fd,buf,size)
#define DLOGWRITE2(fd, buf, size) dlogwrite(2,fd,buf,size)
#define DLOGWRITE3(fd, buf, size) 
#else
#define DLOGWRITE_INCLUDED 0
#define DLOGWRITE(type, fd, buf, size) 
#define DLOGWRITE1(fd, buf, size) 
#define DLOGWRITE2(fd, buf, size) 
#define DLOGWRITE3(fd, buf, size) 
#endif

#if 0
#define PARANOIACHECK(x) x
#define PARANOIACHECKBUF(buf, len) if (memchr(buf, 0, len)){  \
  LOGMSG(LOG_INFO, "PARANOIA bfdutil: %lu %s\n", __LINE__, buf);}
#else
#define PARANOIACHECK(x)
#define PARANOIACHECKBUF(buf, len)
#endif

/****************** TYPE DEFINITION SECTION *********************************/

/* FIXME - That asm/termios.h stuff in the kernel needs to be fixed. Until then
 * create 2 dummy structures here in order to make this program compile with
 * uC-libc Beta2.
 */

struct rs485_wrt {
	unsigned short outc_size;
	const unsigned char *outc;
};


/****************** LOCAL FUNCTION DECLARATION SECTION **********************/
/* Free resources used by bfd */
static void buffered_fd_free(buffered_fd *bfd);

/* Close file descriptor for this bfd */
static void buffered_fd_close(buffered_fd *bfd);

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

/****************** FUNCTION DEFINITION SECTION *****************************/


void dlogwrite(int type, int fd, const char *buf, ssize_t size)
{
  char name[30];
  D(printf("*** BFDUTIL.C: dlogwrite\n"));
  sprintf(name,"/tmp/sersrvd.write%i.%i", type, fd);
  fd = open(name, O_CREAT | O_APPEND | O_WRONLY);
  if (fd > 0)
  {
    if (size < 0)
    {
      write(fd, "#size<0#",8);
    }
    else
    {
      write(fd, buf, size);
    }
    
    close(fd);
  }
} /* dlogwrite */

/* Wrapper to get correct type */
static int tnet_buffered_write(void *bfd, 
                               const unsigned char *buf, 
                               size_t size)
{
  D(printf("*** BFDUTIL.C: tnet_buffered_write\n"));
  return buffered_write((buffered_fd*)bfd, buf, size);
} /* tnet_buffered_write */


/*#***************************************************************************
*#
*# FUNCTION NAME: buffered_fd_init
*#
*# PARAMETERS   : buffered_fd *bfd   - The bfd to init 
*#                size_t size        - Size of buffer
*#
*# RETURNS      : bool, TRUE if Ok
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Init some bfd fields, return TRUE if ok 
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Oct  5 2000  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
bool buffered_fd_init(buffered_fd *bfd, size_t size)
{
  D(printf("*** BFDUTIL.C: buffered_fd_init\n"));
  memset(bfd, 0, sizeof(*bfd));
  bfd->rx_count = 0;
  bfd->tx_count = 0;
  bfd->txfifo = webctrl_fifo_init(size);
  tnet_handler_init(bfd->tnet, bfd, tnet_buffered_write);
  return (bfd->txfifo != NULL);
} /* buffered_fd_init */

/* Write buf to bfd->fd, return number of bytes sent */
static int do_write(buffered_fd *bfd, const unsigned char *buf, size_t size)
{
  int num_written;
  if (bfd->isatty && bfd->config->rs485mode)
  {
    /* If we have kernel RS-485 support and it's enabled, use that - 
     * otherwise toggle RTS from this application.
     */
#ifdef TIOCSERWRRS485
    struct rs485_wrt  io485;
    
    io485.outc_size = size;
    io485.outc = buf;

    num_written = ioctl(bfd->fd, TIOCSERWRRS485, &io485);
    if (num_written < 0) /* Failed */
#endif
    {
      int lines = TIOCM_RTS;
      /* Clear RTS to enable TX mode */
      ioctl(bfd->fd, TIOCMBIC, &lines);  
      /* Remember to set it again when all sent */
      bfd->set_rts_after_write = 1;
      goto send_data;
    }
    goto update_statistics;
  }
  else
  {
send_data:
    num_written = write(bfd->fd, buf, size);
update_statistics:
    if (num_written > 0)
    {
      gettimeofday(&bfd->tx_time, NULL);
      bfd->tx_count += num_written;
    }
    else if (num_written < 0)
    {
      num_written = 0;
      bfd->write_errno = errno;
    }
  }
  return num_written;
}


/*#***************************************************************************
*#
*# FUNCTION NAME: buffered_write
*#
*# PARAMETERS   : buffered_fd *bfd          - The bfd to write to
*#                const unsigned char *buf  - The data to write 
*#                size_t size               - size of data
*#
*# RETURNS      : int, number of bytes written or buffered
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Write data to fd or buffer it if possible.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Oct  5 2000  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
int buffered_write(buffered_fd *bfd, const unsigned char *buf, size_t size)
{
  ssize_t num_written;
  /* Write any pending data */
  D(printf("*** BFDUTIL.C: buffered_write\n"));
  DWRITE(printf("BF_write %i: (%lu): '%s'\n", bfd->fd, size, buf));
  DLOGWRITE1(bfd->fd, buf, size);
  if (bfd->isatty)
  {
    LOGDATA(buf, size,"%s tty :%2i TX %i: ",bfd->config->port_name,bfd->fd,size);
  }
  else
  {
    LOGDATA(buf, size,"%s net :%2i TX %i: ",bfd->config->port_name,bfd->fd,size);
  }
  
  if (bfd->write_errno)
  {
    return 0;
  }

  if (webctrl_fifo_num_unread(bfd->txfifo) > 0)
  {
    size_t tmplen = 0;
    byte *tmpbuf = webctrl_fifo_hold_read(bfd->txfifo, &tmplen);
    PARANOIACHECKBUF(tmpbuf, tmplen);
    
    num_written = do_write(bfd, tmpbuf, tmplen);

    DLOGWRITE2(bfd->fd, tmpbuf, num_written);
    webctrl_fifo_hold_return(bfd->txfifo, tmpbuf, num_written);
    if (num_written == tmplen)
    {
      /* wrote everything */
      /* Try to read/write again, in case it is wrapped */
      if (webctrl_fifo_num_unread(bfd->txfifo) > 0)
      {
        tmplen = 0;
        tmpbuf = webctrl_fifo_hold_read(bfd->txfifo, &tmplen);
        PARANOIACHECKBUF(tmpbuf, tmplen);        
        num_written = do_write(bfd, tmpbuf, tmplen);
        DLOGWRITE2(bfd->fd, tmpbuf, num_written);
        webctrl_fifo_hold_return(bfd->txfifo, tmpbuf, num_written);        
      }
    }
    if (bfd->write_errno)
    {
      return num_written;
    }
  
    if (num_written == tmplen)
    {
      /* Wrote everything in fifo, now write data */
      PARANOIACHECKBUF(buf, size);
      num_written = do_write(bfd, buf, size);
      DLOGWRITE2(bfd->fd, buf, num_written);
    }
    else
    {
      num_written = 0;
    }
  }
  else
  {
    /* No data in fifo, try to write */
    PARANOIACHECKBUF(buf, size);
    num_written = do_write(bfd, buf, size);
    DLOGWRITE2(bfd->fd, buf, num_written);
  }

  /* Add unwritten data to fifo */
  if (num_written != size)
  {
    size_t len_left = size - num_written;
    size_t buffered;
#if DLOGWRITE_INCLUDED    
    char s[50];
    sprintf(s, "# ONLY %lu/%lu fifo: %lu/%lu #", 
            (unsigned long)num_written, (unsigned long)size, 
            (unsigned long)webctrl_fifo_num_unread(bfd->txfifo),
            (unsigned long)bfd->txfifo->size);
#endif    
    DLOGWRITE3(bfd->fd, s, strlen(s));
    /* Add to fifo */
    DBUFINFO(fprintf(stderr, "Failed to write all to %i tx: %lu.\n", 
                   bfd->fd,
                   bfd->tx_count));
    buffered = webctrl_fifo_write(bfd->txfifo, &buf[num_written],
                                  len_left);
    num_written += buffered;
    
    if (buffered != len_left)
    {
      LOGMSG(LOG_ERR, "!! Failed to buffer all!\n");

#if DLOGWRITE_INCLUDED
      sprintf(s, "# CANT BUF ALL %lu/%lu fifo: %lu/%lu #", 
              (unsigned long)num_written, (unsigned long)size, 
              (unsigned long)webctrl_fifo_num_unread(bfd->txfifo), 
              (unsigned long)bfd->txfifo->size);
      
      DLOGWRITE3(bfd->fd, s, strlen(s));
#endif
      DERROR(fprintf(stderr, "***** Failed to write all to fifo!!!!\n"));
    }
  }
  if (bfd->write_errno)
  {
    LOGMSG(LOG_ERR, "write(%i) errno: %i!\n", bfd->fd, bfd->write_errno);
  }
  
  return num_written;
}



/*#***************************************************************************
*#
*# FUNCTION NAME: buffered_close
*#
*# PARAMETERS   : struct buffered_fd **head - List head
*#                struct buffered_fd *bfd   - bfd to close
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Close and free bfd, remove it from list
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Oct  5 2000  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
void buffered_close(struct buffered_fd **head, struct buffered_fd *bfd)
{
  struct buffered_fd *tmp = *head;
  struct buffered_fd *prev = NULL;
  D(printf("*** BFDUTIL.C: bfd_close\n"));
  /* Find it */
  while (tmp != NULL && tmp != bfd)
  {
    prev = tmp;
    tmp = tmp->next;
  }

  if (tmp != NULL)
  {
    /* Unlink it */
    if (prev == NULL)
    {
      /* First element */
      *head = tmp->next;
      
    }
    else
    {
      prev->next = tmp->next;
    }
  }
  else
  {
    fprintf(stderr, "bfd_close: Not in list!\n");
  }
  buffered_fd_free(bfd);
  if (bfd->fd >= 0)
  {
    shutdown(bfd->fd, 2);
    buffered_fd_close(bfd);
  }
  free(bfd);
} /* buffered_close */


static void buffered_fd_free(buffered_fd *bfd)
{
  D(printf("*** BFDUTIL.C: buffered_fd_free\n"));
  D(fprintf(stderr, "TMP: fifo_done: 0x%08X->0x%08X\n", bfd, bfd->txfifo));
  if (bfd->txfifo != NULL)
  {
    webctrl_fifo_done(bfd->txfifo);
  }
  bfd->txfifo = NULL;
  D(fprintf(stderr, "TMP: fifo_done\n"));
  tnet_handler_destroy(bfd->tnet);
}


static void buffered_fd_close(buffered_fd *bfd)
{
  int err;
  
  err = close(bfd->fd);
  if (err)
  {
    LOGMSG(LOG_ERR, "close(%i) failed errno: %i \n", bfd->fd, errno);
  }
  
  bfd->fd = -1;
  D(printf("*** BFDUTIL.C: buffered_fd_closed\n")); 
  /* If I'm sending to someone and it is sending to me, don't let it anymore */
  if (bfd->tobfd != NULL)
  {
    if (bfd->tobfd->tobfd == bfd)
    {
      bfd->tobfd->tobfd = NULL;
    }
  }
} /* buffered_fd_close */



/****************** END OF FILE bfdutil.c ***********************************/
