/*!***************************************************************************
*!
*! FILE NAME  : webctrl_fifo.c
*!
*! DESCRIPTION: FIFO buffer routines 
*!
*! FUNCTIONS  : 
*! (EXTERNAL)
*!  webctrl_fifo_init
*!  webctrl_fifo_done
*!  webctrl_fifo_num_free
*!  webctrl_fifo_num_unread
*!  webctrl_fifo_write
*!  webctrl_fifo_read
*!  webctrl_fifo_hold_read
*!  webctrl_fifo_hold_return
*!  webctrl_fifo_print
*!  webctrl_fifo_set_trig
*!
*! FUNCTIONS  : 
*! (LOCAL)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Mar 13 1998  Johan Adolfsson    Initial version
*! Mar 16 1998  Johan Adolfsson    Added function headers on larger functions.
*! Mar 18 1998  Johan Adolfsson    Added theFifo->isTrigged
*! Apr 16 1998  Johan Adolfsson    Changed webctrl_log.h -> debug_log.h
*! $Log: webctrl_fifo.c,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.2  2000/10/05 14:05:26  johana
*! Review changes
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1998, 1999, 2000 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: webctrl_fifo.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $  */

/********************** INCLUDE FILES SECTION *******************************/

#include "webctrl_fifo.h"
#include <stdlib.h>

/*#define WEBCTRL_FIFO_TEST*/
#ifdef WEBCTRL_FIFO_TEST
#include "stdx.h"
#endif

/********************** CONSTANT AND MACRO SECTION **************************/
#define TRUE 1
#define FALSE 0
#define MIN(a,b) ((a)<(b)?(a):(b))

/* This code is originally used both from user and interrupt space,
 * but make the interrupt macros do nothing now */
#define DISABLE_SAVE()
#define RESTORE()

#if 0 || defined(WEBCTRL_FIFO_TEST)/* 1 to enable debug */
#define FIFO_ERR(s) printf("%s",s)
#define FIFOD_STR(s) printf("%s", s)
#ifdef WEBCTRL_FIFO_TEST
#define FIFO_LOG_NBR(s, nbr) printf("%u %s",nbr,s)
#else
#define FIFO_LOG_NBR(s, nbr) DEBUG_LOG_NBR(s, nbr)
#endif
#define FIFOD(x) x
#define FIFO_FUNC(x) x
#else
#define FIFO_ERR(s) 
#define FIFOD_STR(s) 
#define FIFO_LOG_NBR(s, nbr) 
#define FIFOD(x) 
#define FIFO_FUNC(x) 
#endif /* 0/1 */
/********************** TYPE DEFINITION SECTION *****************************/

/********************** LOCAL FUNCTION DECLARATION SECTION ******************/

/********************** GLOBAL VARIABLE DECLARATION SECTION *****************/

/********************** LOCAL VARIABLE DECLARATION SECTION ******************/

/********************** FUNCTION DEFINITION SECTION *************************/


/*#***************************************************************************
*# FUNCTION NAME: webctrl_fifo_init
*#
*# PARAMETERS   : 
*#                size_t    theSize  - Size of fifo to create
*#
*# RETURNS      : webctrl_fifo_type* , the created fifo.
*#
*# SIDE EFFECTS : Allocates memory, to be freed by webctrl_fifo_done()
*#
*# DESCRIPTION  : Creates and initialises a fifo with theSize bytes of data
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*! Mar 16 1998  Johan Adolfsson    Initial version
*#                                 <Describe what changes were made and why.>
*# 
*#***************************************************************************/
webctrl_fifo_type* webctrl_fifo_init(size_t theSize)
{
  webctrl_fifo_type* aFifo = (webctrl_fifo_type*)
    malloc(sizeof(webctrl_fifo_type));
  if (aFifo)
  {
    aFifo->buffer = (byte*)malloc(theSize+1);
    if (aFifo->buffer)
    {
      aFifo->size = theSize;
      aFifo->num_free = theSize - 1; /* 1 less then size */
      aFifo->new_pos = aFifo->buffer;
      aFifo->hold_pos = aFifo->buffer;
      aFifo->unread_pos = aFifo->buffer;
      aFifo->buffer_end = &aFifo->buffer[aFifo->size];
      aFifo->isTrigged = FALSE;      
      aFifo->trig_function = NULL;
      aFifo->cb = NULL;
#ifdef WEBCTRL_FIFO_TEST /* Fill the buffer with characters acc. to position */
      {
        size_t i =0;
        for (i=0;i<aFifo->size;i++)
        {
          aFifo->buffer[i]= (i%10)+'0';
          if (i%10 == 0)
          {
            aFifo->buffer[i]= ((i/10)%10)+'0';            
          }
        }
      }
#endif
      aFifo->buffer[aFifo->size]='\0';
    }
    else
    {
      /* Failed */
      free(aFifo);
      aFifo = NULL;
    }
  }
  return aFifo;
} /* webctrl_fifo_init */


void webctrl_fifo_set_trig(webctrl_fifo_type* theFifo,
                            fifo_trig_function_type* trig_function)
/* Set the trig function */
{
  theFifo->trig_function = trig_function;
} /* webctrl_fifo_set_trig */


void webctrl_fifo_done(webctrl_fifo_type* theFifo)
/* free theFifo */
{
  if (theFifo)
  {
    free(theFifo->buffer);
    free(theFifo);
  }
} /* webctrl_fifo_done */


size_t webctrl_fifo_num_free(struct webctrl_fifo_type* theFifo)
/* return number of free bytes in theFifo */
{
  if (WEBCTRL_FIFO_NOT_WRAPPED(theFifo))
  {
    return theFifo->size - (theFifo->new_pos - theFifo->hold_pos)-1;
  }
  else
  {
    /*wrapped */
    return (theFifo->hold_pos - theFifo->new_pos) - 1;
  }
} /* webctrl_fifo_num_free */


size_t webctrl_fifo_num_unread(struct webctrl_fifo_type* theFifo)
/* return number of bytes not read */
{
  if (theFifo->new_pos >= theFifo->unread_pos)
  {
    return (theFifo->new_pos - theFifo->unread_pos);
  }
  else
  {
    /*wrapped */
    return theFifo->size - (theFifo->unread_pos - theFifo->new_pos);
  }
} /* webctrl_fifo_num_unread */


/*#***************************************************************************
*# FUNCTION NAME: webctrl_fifo_write_byte
*#
*# PARAMETERS   : 
*#                webctrl_fifo_type  *theFifo
*#                byte                theByte  The data to write
*#
*# RETURNS      : size_t, number of bytes written (0 or 1).
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  :  
*#  Adds theByte to theFifo, return number of bytes written 
*#  Could be called from interrupts 
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Mar 16 1998  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
size_t webctrl_fifo_write_byte(struct webctrl_fifo_type* theFifo,
                              byte theByte)
{
  /* FIFO_LOG_NBR("FIFO: write_byte \n", theByte); */
  if (theFifo->num_free>0)
  {
    byte *new_chars = theFifo->new_pos;
    *theFifo->new_pos = theByte;
    theFifo->new_pos++;
    theFifo->num_free--;
    if (theFifo->new_pos == theFifo->buffer_end)
    {
      /* Wrap */
      theFifo->new_pos = theFifo->buffer;
    }
    if (!theFifo->isTrigged && theFifo->trig_function)
    {
      if (theFifo->trig_function(theFifo, new_chars, 1))
      {
        /* Don't use theFifo->isTrigged = theFifo->trig_function(..), since
         * this function might be reentered.
         */
        theFifo->isTrigged = TRUE;
      }
      
    }
    return 1;
  }
  return 0;
} /* webctrl_fifo_write_byte */


/*#***************************************************************************
*# FUNCTION NAME: webctrl_fifo_write
*#
*# PARAMETERS   : 
*#                webctrl_fifo_type  *theFifo
*#                const byte         *theData The data to write
*#                size_t               theLen  Number of bytes to write
*#
*# RETURNS      : size_t, number of bytes written.
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  :  
*#  Write theData to theFifo, return number of bytes written 
*#  Could be called from interrupts 
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Mar 16 1998  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
size_t webctrl_fifo_write(struct webctrl_fifo_type* theFifo,
                         const byte* theData,
                         size_t theLen)
{
  size_t num_written = 0;
  byte* new_chars = theFifo->new_pos;
  FIFO_LOG_NBR("FIFO: write \n", theLen);
  
  while ((num_written < theLen) && (theFifo->num_free>0))
  {
    *theFifo->new_pos++ = theData[num_written++];
    theFifo->num_free--;
    if (theFifo->new_pos == theFifo->buffer_end)
    {
      /* Wrap */
      theFifo->new_pos = theFifo->buffer;
    }
  } /* while */
  if (!theFifo->isTrigged && theFifo->trig_function)
  {
    theFifo->isTrigged = theFifo->trig_function(theFifo,
                                                new_chars,
                                                num_written);
  }
  return num_written;
} /* webctrl_fifo_write */


/*#***************************************************************************
*# FUNCTION NAME: webctrl_fifo_read
*#
*# PARAMETERS   : 
*#                webctrl_fifo_type  *theFifo
*#                byte               *theData Where to put the data read.
*#                size_t               theLen  Max number of bytes to read.
*#
*# RETURNS      : size_t, number of bytes read (written to theData).
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  :  Copies max theLen bytes of theFifo data to theData,
*#                 return number of bytes written (read).
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Mar 16 1998  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
size_t webctrl_fifo_read(struct webctrl_fifo_type* theFifo,
                        byte* theData,
                        size_t theLen)
{
  size_t num_written = 0;
  theLen = MIN(webctrl_fifo_num_unread(theFifo), theLen);
  FIFO_FUNC(printf("FIFO: read %u\n", theLen));
  theFifo->isTrigged = FALSE;
  while ((num_written < theLen) && (theFifo->num_free>0))
  {
    theData[num_written++] = *theFifo->unread_pos++;
    theFifo->num_free++;
    if (theFifo->unread_pos == theFifo->buffer_end)
    {
      /* Wrap */
      theFifo->unread_pos = theFifo->buffer;
    }
  }
  /* update hold_pos too: */
  /* I don't think we can mix hold and copy */
  theFifo->hold_pos = theFifo->unread_pos;
  return num_written;
} /* webctrl_fifo_read */


/*#***************************************************************************
*# FUNCTION NAME: webctrl_fifo_hold_read
*#
*# PARAMETERS   : 
*#                webctrl_fifo_type  *theFifo
*#                size_t              *theLen  Input: Max number of bytes to
*#                                                   hold. If 0, maximum
*#                                            Output: Number of bytes hold
*#
*# RETURNS      : byte* to returned data
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Return pointer to unread data and updates *theLen with the
*#                length. Return the date with webctrl_fifo_hold_return(..).
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Mar 16 1998  Johan Adolfsson    Initial version
*# 
*#***************************************************************************/
byte* webctrl_fifo_hold_read(struct webctrl_fifo_type* theFifo,
                             size_t* theLen)
{
  byte* theData = theFifo->unread_pos;
  size_t aLen;
  DISABLE_SAVE(); /* new_pos is updated by write (called by int) */
  theFifo->isTrigged = FALSE;
  if (theFifo->new_pos >= theFifo->unread_pos)
  {
    /* read all */

    aLen = (theFifo->new_pos - theFifo->unread_pos);
    FIFO_LOG_NBR("FIFO: hold_read all \n", aLen);
    if (*theLen>0)
    {
      aLen = MIN(*theLen, aLen); /* max *theLen bytes */
    }
    theFifo->unread_pos += aLen;
  }
  else
  {
    /* wrapped, return max length to end of buffer */
    aLen = (theFifo->buffer_end - theFifo->unread_pos);
    FIFO_LOG_NBR("FIFO: hold_read wrapped \n", aLen);
    if ((*theLen>0) && (aLen > *theLen))
    {
      aLen = *theLen; /* max *theLen bytes */
      theFifo->unread_pos += aLen;
    }
    else
    {
      theFifo->unread_pos = theFifo->buffer; /* start at beginning next time*/
    }
  }
  RESTORE();  
  theFifo->hold_total += aLen;
#ifdef DEBUG
  if (aLen>0)
  {
    FIFO_FUNC(printf("FIFO: hold_read %u\n", aLen));
  }
#endif
  *theLen = aLen;
  return theData;
} /* webctrl_fifo_hold_read */


/*#***************************************************************************
*# FUNCTION NAME: webctrl_fifo_hold_return
*#
*# PARAMETERS   : 
*#              struct webctrl_fifo_type* theFifo,
*#                              const byte* theData,
*#                              size_t       theLen
*#
*# RETURNS      : void
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Return theLen bytes of hold data at theData to theFifo.
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Mar 16 1998  Johan Adolfsson    Initial version
*#                                 <Describe what changes were made and why.>
*# 
*#***************************************************************************/
void webctrl_fifo_hold_return(struct webctrl_fifo_type* theFifo,
                              const byte* theData,
                              size_t theLen)
{

  FIFO_FUNC(printf("FIFO: hold_return %u\n", theLen));

  if (theData == theFifo->hold_pos)
  {
    /* Normal case */
    DISABLE_SAVE(); /* updating num_free */      
    theFifo->hold_pos += theLen;
    theFifo->num_free += theLen;
    if (theFifo->hold_pos >= theFifo->buffer_end)
    {
      /* wrap */
      theFifo->hold_pos = theFifo->buffer;
    }
    if (!(theFifo->num_free<theFifo->size))
    {
      FIFO_ERR("hold_return with theLen to big!\n");
      theFifo->num_free = theFifo->size-1;
    }
    RESTORE();
  }
  else
  {
    /* someones is returning in wrong order */
    /* it's ok though, but we can't update hold_pos (unless nothing is hold) */
  }
  if (theFifo->hold_total< theLen)
  {
    FIFO_ERR("hold_total< theLen!\n");
    theFifo->hold_total = 0;
  }
  else
  {
    theFifo->hold_total -= theLen;
  }
  
  if (theFifo->hold_total == 0)
  {
    /* everything returned */
    DISABLE_SAVE();
    theFifo->hold_pos = theFifo->unread_pos;

    /* calc num_free */
    if (theFifo->new_pos >= theFifo->hold_pos)
    {
      theFifo->num_free = theFifo->size-(theFifo->new_pos-theFifo->hold_pos)-1;
    }
    else
    {
      /* wrap */
      theFifo->num_free = (theFifo->hold_pos - theFifo->new_pos) - 1;
    }
    RESTORE();    
  }
}


/*#***************************************************************************
*# FUNCTION NAME: webctrl_fifo_print
*#
*# PARAMETERS   : theFifo
*#
*# RETURNS      : void
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Print info about theFifo 
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*! Mar 16 1998  Johan Adolfsson    Initial version
*#                                 <Describe what changes were made and why.>
*# 
*#***************************************************************************/
void webctrl_fifo_print(struct webctrl_fifo_type* theFifo)
{
#ifdef DEBUG  
  printf("FIFO info\n");
  printf("  size: %u\n", theFifo->size);
  printf("  num_free: %u\n", theFifo->num_free);
  printf("  num_free_calc  : %u\n", webctrl_fifo_num_free(theFifo));
  printf("  num_unread_calc: %u\n", webctrl_fifo_num_unread(theFifo));
  printf("  buffer    : 0x%lX '%s'\n", theFifo->buffer, theFifo->buffer);
  printf("  new_pos   : 0x%lX (%u)\n", theFifo->new_pos,
         (theFifo->new_pos-theFifo->buffer));
  printf("  hold_pos  : 0x%lX (%u)\n", theFifo->hold_pos,
         (theFifo->hold_pos-theFifo->buffer));
  printf("  unread_pos: 0x%lX (%u)\n", theFifo->unread_pos,
         (theFifo->unread_pos-theFifo->buffer));
  printf("  hold_total: %u\n", theFifo->hold_total);
#endif  
}

/* ---------------------------------------------------------------------*/

#ifdef WEBCTRL_FIFO_TEST

bool test_fifo_trig_function(struct webctrl_fifo_type* theFifo,
                             byte *new_chars,
                             size_t new_chars_count)
{
  /* should be treated as an interrupt routine */
  bool trigged = FALSE;
  size_t trig_cnt = 0; /* number of unread chars until the trig */
  byte* curr = new_chars;
    
  printf("new_chars: 0x%lX count: %u\n", new_chars, new_chars_count);  
  /* The new chars may be wrapped! */  
  if (theFifo->buffer_end-new_chars < new_chars_count)
  {
    /* wrapped */
    trigged = TRUE;
    printf("Trig Wrapped!\n");
    trig_cnt = theFifo->buffer_end-new_chars;
  }
  else
  {
    size_t i;
    
    for (i=0; i<new_chars_count; i++)
    {
      if ((new_chars[i]=='\r') || (new_chars[i]=='\n'))
      {
        trigged = TRUE;
        trig_cnt = new_chars - theFifo->unread_pos + i + 1;
        
        if ((new_chars[i]=='\r') && (new_chars[i+1]=='\n'))
        {
          /* \r\n */
          trig_cnt++;
        }
        break;
      }
    }
  } /* for */
  if (trigged)
  {
    size_t len = trig_cnt;
    byte* data;

    
    printf("Trigged! trig_cnt: %u\n", trig_cnt);
    data = webctrl_fifo_hold_read(theFifo, &len);
    printf("****** TRIGDATA: (%u) '%.*s'\n", len, len ,data);
    webctrl_fifo_hold_return(theFifo, data, len);
#if 0
    /* tell application to read the fifo */
    (void)os_int_send(OTHER_INT_PROG, theFifo->cb->dest, LOGPR_CALLBACK_HIT,
                      TYPE_P_UWORD, trig_cnt, theFifo->cb);
#endif    
  }
  return trigged;
}


int main(void)
{
  webctrl_fifo_type* aFifo = webctrl_fifo_init(20);
  byte* aData1;
  byte* aData2;
  byte aDataCopy[1000];
  size_t aLen, aLen1,aLen2;
  webctrl_fifo_set_trig(aFifo, test_fifo_trig_function);
  
  webctrl_fifo_print(aFifo);
  printf("Write\n");
  webctrl_fifo_write(aFifo, (byte*)"ABCD\nEFGHIJ",10);
  webctrl_fifo_print(aFifo);
  webctrl_fifo_write(aFifo, (byte*)"x",1);
  webctrl_fifo_write_byte(aFifo, 'B');
  webctrl_fifo_print(aFifo);
  webctrl_fifo_write(aFifo, (byte*)"y",0);

  webctrl_fifo_print(aFifo);
#if 0  
  printf("Read\n");
  aLen = webctrl_fifo_read(aFifo, aDataCopy, 1000);
  aDataCopy[aLen]='\0';
  printf("(%d) '%s'\n", aLen, aDataCopy);
  webctrl_fifo_print(aFifo);

  printf("Write overflow\n");
  webctrl_fifo_write(aFifo, (byte*)"abcde\rfghij",10);
  webctrl_fifo_print(aFifo);
  webctrl_fifo_write(aFifo, (byte*)"klmnopqr\nstuvwx",15); /* overflow */
  webctrl_fifo_print(aFifo);  
  aData1 = webctrl_fifo_hold_read(aFifo, &aLen1);
  printf("(%d) '%.*s'\n", aLen1, aLen1, aData1);
  webctrl_fifo_print(aFifo);  
  aData2 = webctrl_fifo_hold_read(aFifo, &aLen2);
  printf("(%d) '%.*s'\n", aLen2, aLen2, aData2);
  webctrl_fifo_print(aFifo);

  printf("return in wrong order\n");
  webctrl_fifo_hold_return(aFifo, aData2, aLen2);
  webctrl_fifo_print(aFifo);
  webctrl_fifo_hold_return(aFifo, aData1, aLen1);
  
  webctrl_fifo_print(aFifo);

  printf("Write overflow\n");
  webctrl_fifo_write(aFifo, (byte*)"0123456789\r\n112345678921234567893",25);
  webctrl_fifo_print(aFifo);
  printf("read return\n");  
  aData1 = webctrl_fifo_hold_read(aFifo, &aLen1);
  printf("(%d) '%.*s'\n", aLen1, aLen1, aData1);
  webctrl_fifo_print(aFifo);  
  aData2 = webctrl_fifo_hold_read(aFifo, &aLen2);
  printf("(%d) '%.*s'\n", aLen2, aLen2, aData2);
  webctrl_fifo_print(aFifo);
  printf("return in right order\n");  
  webctrl_fifo_hold_return(aFifo, aData1, aLen1);
  webctrl_fifo_print(aFifo);
  webctrl_fifo_hold_return(aFifo, aData2, aLen2);
#endif  
  webctrl_fifo_print(aFifo);
  webctrl_fifo_done(aFifo);
  return 0;
}

#endif /*WEBCTRL_FIFO_TEST */


/********************** END OF FILE webctrl_fifo.c **************************/

