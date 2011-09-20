/*!***************************************************************************
*!
*! FILE NAME  : webctrl_fifo.h
*!
*! DESCRIPTION: FIFO buffer routines 
*!
*! FUNCTIONS  : <The names of the exported functions.>
*! (EXPORTED)
*!  webctrl_fifo_init
*!  webctrl_fifo_done
*!  webctrl_fifo_num_free
*!  webctrl_fifo_num_unread
*!  webctrl_fifo_write
*!  webctrl_fifo_read
*!  webctrl_fifo_hold_read
*!  webctrl_fifo_hold_return
*!  webctrl_fifo_print
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Mar 13 1998  Johan Adolfsson    Initial version
*!                                 <Describe what changes were made and why.>
*! $Log: webctrl_fifo.h,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.2  2000/10/05 14:05:26  johana
*! Review changes
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1998, 2000 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: webctrl_fifo.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef _WEBCTRL_FIFO_H_
#define _WEBCTRL_FIFO_H_
/********************** INCLUDE FILES SECTION *******************************/

#include <unistd.h>

/********************** CONSTANT AND MACRO SECTION **************************/
#define WEBCTRL_FIFO_NUM_FREE(f) webctrl_fifo_num_free(f)
#define WEBCTRL_FIFO_IS_WRAPPED(f) (f->new_pos < f->hold_pos)
#define WEBCTRL_FIFO_NOT_WRAPPED(f) (f->new_pos >= f->hold_pos)
/********************** TYPE DEFINITION SECTION *****************************/

typedef unsigned char byte;

struct webctrl_fifo_type; /* forward declaration */
typedef int fifo_trig_function_type(struct webctrl_fifo_type* theFifo,
                                    byte *new_chars,
                                    size_t new_chars_count);


typedef struct webctrl_fifo_type
{
  /* those with (PROTECTED) is changed by the write functions that could
   * be called from INTERRUPT's
   */
  size_t size;         /* Total size of buffer */
  size_t num_free;     /* How much space there is left (PROTECTED)*/
  byte* buffer;       /* allocated storage*/
  byte* buffer_end;   /* pointer to end of buffer (&buffer[size] )*/
  byte* new_pos;      /* pointer to where to put next byte (PROTECTED)*/
  byte* hold_pos;     /* pointer to first byte that is used by application */
  byte* unread_pos;   /* pointer to first unread byte */
  size_t hold_total;   /* Total number of bytes currently hold by app */
  int  isTrigged;    /* Set when trig_function returns TRUE, cleared when
                       * reading. Don't call trif_function if already trigged
                       */  
  
  fifo_trig_function_type* trig_function;  /* a trigfunction called when
                                            * data is written
                                            */
  void* cb;           /* A cb that could be set after init and used in
                       *  trig_function */
}webctrl_fifo_type;
/*
If new_pos == unread_pos there are 0 bytes in buffer,
therefor, the maximum number of bytes in buffer is size-1

EMPTY BUFFER:
size:   30
buffer: 012345678911234567892123456789
               ^
               |- new_pos    =7
               |- unread_pos =7
               |- hold pos   =7
num_used= 0 = new_pos - hold_pos
num_free = size = 29 = size - (new_pos - hold_pos)-1 = 29

buffer: 012345678911234567892123456789
               ^xxxxxxxx^
               |         - new_pos    =16
               |- unread_pos =7
               |- hold pos   =7
num_used= 0 = new_pos - hold_pos = 16-7 = 9
num_free = size = 30 = size - (new_pos - hold_pos)-1  = 30 -(16-7)-1 = 20

WRAPPED:
buffer: 012345678911234567892123456789
        xx^    ^xxxxxxxxxxxxxxxxxxxxxx
          |----|---- new_pos    =2
               |- unread_pos =7
               |- hold pos   =7
new_pos>hold_pos               
num_used= 0 = size - (hold_pos - new_pos) = 30 - (7-2) = 25
num_free = (hold_pos - new_pos) -1  = (7-2)-1 = 4

FULL BUFFER
buffer: 012345678911234567892123456789
        ^xxxxxxxxxxxxxxxxxxxxxxxxxxxx^
                                     |---- new_pos    =29
        |- unread_pos =0
        |- hold pos   =0
new_pos>hold_pos               
num_used= 29 = (new_pos - hold_pos) = 29 - 0= 29
num_free = 0 = size - (new_pos - hold_pos)-1  = 30 -(29-0)-1 = 0

WRAPPED FULL BUFFER
buffer: 012345678911234567892123456789
        xxxxxx^^xxxxxxxxxxxxxxxxxxxxxx
              +|---- new_pos    =6
               |- unread_pos =7
               |- hold pos   =7
new_pos<hold_pos               
num_used= 0 = size - (hold_pos - new_pos) = 30 - (7-6) = 29
num_free = (hold_pos - new_pos)  = (7-6)-1 = 0


 */

/********************** EXPORTED FUNCTION DECLARATION SECTION ***************/
webctrl_fifo_type* webctrl_fifo_init(size_t theSize);
/* Create a FIFO with size theSize */

void webctrl_fifo_set_trig(webctrl_fifo_type* theFifo,
                            fifo_trig_function_type* trig_function);
/* Set the trig function */

void webctrl_fifo_done(webctrl_fifo_type* theFifo);
/* Deallocates theFifo */

size_t webctrl_fifo_num_free(struct webctrl_fifo_type* theFifo);
/* return number of free bytes in theFifo */

size_t webctrl_fifo_num_unread(struct webctrl_fifo_type* theFifo);
/* return number of bytes not read */

size_t webctrl_fifo_write_byte(struct webctrl_fifo_type* theFifo,
                              byte theByte);
/* Adds theByte to theFifo, return number of bytes written */

size_t webctrl_fifo_write(struct webctrl_fifo_type* theFifo,
                         const byte* theData,
                         size_t theLen);
/* Adds theData to theFifo, return number of bytes written */

size_t webctrl_fifo_read(struct webctrl_fifo_type* theFifo,
                        byte* theData,
                        size_t theLen);
/* Copies max theLen bytes of theFifo data to theData,
 *  return number of bytes written (read)
 */

byte* webctrl_fifo_hold_read(struct webctrl_fifo_type* theFifo,
                             size_t* theLen);
/* Hold max *theLen bytes, if *theLen == 0, return maximum nbr of chars
 * Return pointer to unread data and updates *theLen with the length returned
 */

void webctrl_fifo_hold_return(struct webctrl_fifo_type* theFifo,
                              const byte* theData,
                              size_t theLen);
/* Return theLen bytes of hold data at theData to theFifo */

void webctrl_fifo_print(struct webctrl_fifo_type* theFifo);
/* Print info about theFifo */

#endif /* _WEBCTRL_FIFO_H_ */
/********************** END OF FILE webctrl_fifo.h **************************/

