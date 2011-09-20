/*!***************************************************************************
*!
*! FILE NAME  : tnetopt.h
*!
*! DESCRIPTION: Telnet options, some taken from print server teldef.h
*!              + added options according to 
*!              RFC2217 -Telnet Com Port Control Option
*!
*! FUNCTIONS  : <The names of the exported functions.>
*! (EXPORTED)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Aug  5 1999  Johan Adolfsson    Initial version
*! $Log: tnetopt.h,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.13  2002/02/25 13:39:19  johana
*! Added comment.
*!
*! Revision 1.12  2001/02/01 20:34:56  johana
*! Updated debugging and telnet option handling
*!
*! Revision 1.11  2001/01/11 16:56:49  johana
*! Use neg_state instead of the *_will_sent.
*! tnet_send_option() keeps track if an option is sent or not.
*! Only allow RFC2217 commands if authorisation is complete.
*!
*! Revision 1.10  2001/01/09 18:53:54  johana
*! Added password so we can buffer it.
*! Added some *_will_sent to keep track of them.
*!
*! Revision 1.9  2000/11/14 14:33:55  johana
*! Added support for user authentication and IP number verification.
*!
*! Revision 1.8  2000/11/13 18:15:57  johana
*! Added rfc2217_will_sent, and tnet_send_option()
*!
*! Revision 1.7  2000/10/05 14:05:26  johana
*! Review changes
*!
*!
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: tnetopt.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef TNETOPT_H
#define TNETOPT_H

/****************** INCLUDE FILES SECTION ***********************************/
#include "debug.h"
/****************** CONSTANT AND MACRO SECTION ******************************/

/* TELNET commands */
#define IAC   ((unsigned char)255)   /* FF interpret as command: */
#define DONT  ((unsigned char)254)   /* FE you are not to use option */
#define DO    ((unsigned char)253)   /* FD please, you use option */
#define WONT  ((unsigned char)252)   /* FC I won't use option */
#define WILL  ((unsigned char)251)   /* FB I will use option */
#define SB    ((unsigned char)250)   /* FA interpret as subnegotiation */
#define GA    ((unsigned char)249)   /* F9 you may reverse the line */
#define EL    ((unsigned char)248)   /* F8 erase the current line */
#define EC    ((unsigned char)247)   /* F7 erase the current character */
#define AYT   ((unsigned char)246)   /* F6 are you there */
#define AO    ((unsigned char)245)   /* F5 abort output - but let prog finish*/
#define IP    ((unsigned char)244)   /* F4 interrupt process - permanently */
#define BREAK ((unsigned char)243)   /* F3 break */
#define DM    ((unsigned char)242)   /* F2 data mark - for connect. cleaning */
#define NOP   ((unsigned char)241)   /* F1 no operation */
#define SE    ((unsigned char)240)   /* F0 end of subnegotiation */
#if 1 /* TN3270E_INCLUDED || TN5250E_INCLUDED */
#define EOR   ((unsigned char)239)   /* end of record */
#endif

/* option codes */
#define TELOPT_BINARY         ((unsigned char)0)
#define TELOPT_ECHO           ((unsigned char)1)
#define TELOPT_SGA            ((unsigned char)3)
#define TELOPT_TM             ((unsigned char)6)
#if 1 /* TN3270E_INCLUDED || TN5250E_INCLUDED */
#define TELOPT_TERMINAL_TYPE  ((unsigned char)24)
#define TELOPT_EOR            ((unsigned char)25)
#define TELOPT_NEW_ENVIRON    ((unsigned char)39)
#define TELOPT_TN3270E        ((unsigned char)40)
#endif

/* COM-PORT-OPTION according to RFC2217 */
#define TELOPT_COM_PORT_OPTION  ((unsigned char)44) /* 2C */

/* AXCOMP protocol option (we made this up) */
#define TELOPT_AXCOM_PORT_OPTION  ((unsigned char)64) 

/* The mex TELOPT yo care about when handling the states, 
   odd to get alignment, the array is MAX_TELOPT+1 */
#define MAX_TELOPT 67 

/* Bitmasks to keep track of option status */
#define BW_WILL_SENT 1
#define BW_WONT_SENT 2
#define BW_DO_SENT   4
#define BW_DONT_SENT 8
#define BW_WILL_RECV 16
#define BW_WONT_RECV 32
#define BW_DO_RECV   64
#define BW_DONT_RECV 128


/* option flag bits */
#define NO_OPTS            0x00     /* no options at all */
#define ECHO_FLAG          0x01
#define SGA_FLAG           0x02
#if 1 /* TN3270E_INCLUDED || TN5250E_INCLUDED */
#define EOR_FLAG           0x04
#define TN3270E_FLAG       0x08
#define TERMINAL_TYPE_FLAG 0x10
#define NEW_ENVIRON_FLAG   0x20
#define BINARY_FLAG        0x80
#endif

/* special characters */
#define ASCII_NUL 0
#define CTRL_D    4   /* logout or exit */
#define ASCII_BEL 7
#define ASCII_BS  8
#define ASCII_HT  9
#define ASCII_LF  10
#define ASCII_VT  11
#define ASCII_FF  12
#define ASCII_CR  13
#define ASCII_DEL 127

/* Telnet definitions */
/* Size of TCP receive buffer for connections */
#define TNT_TCP_RXBUFSIZE 256

/* Reverse telnet definitions */
/* Size of TCP receive buffer for connections */
#define RTN_TCP_RXBUFSIZE   TCP_DEF_RX_BUF_SIZE



extern const unsigned char tnet_end[2]/* = { IAC, SE }*/;
extern const unsigned char IAC_str[2]/* = {IAC, '\0' }*/;




#define TRUE 1
#define FALSE 0

#define USERNAME_MAX_LEN 15
#define PASSWORD_MAX_LEN 15

/****************** TYPE DEFINITION SECTION *********************************/

typedef int bool;

typedef enum {
  IAC_normal = 0, 
  TOS_IAC,        /* IAC received, wait for command */
  TOS_PRINT_IAC   /* print escaped IAC character */
} IAC_status_type;

typedef enum {TNET_NOT_FINISHED, TNET_FINISHED } tnet_parse_status_type;

#define BUF_SIZE 1024
#define TNET_BUFFER_SIZE (BUF_SIZE + 256)

typedef int tnet_write_function_type(void *cb, 
                                     const unsigned char *buf, 
                                     size_t len);


typedef struct tnet_handler_struct_type
{
  void *cb;
  tnet_write_function_type *write;
  IAC_status_type opt_state;
  unsigned char buffer[TNET_BUFFER_SIZE];
  ssize_t buf_len;
  bool rfc2217_enabled;
  bool axcomp_enabled;
  bool echo_opt;
  bool is_server;
  bool rfc2217_flow_to_net_ok; /* If ok to send */
  bool rfc2217_flow_from_net_ok; /* If ok to receive */
  int axcomp_status_on_mask; /* TIOCM mask for those signals we 
			      *	want notification of */
  int rfc2217_line_state_mask; /* mask for those errors we 
                               	* want notification of */
  int rfc2217_modem_state_mask; /* mask for those signals we 
				 * want notification of */
  char username[USERNAME_MAX_LEN+1];
  char password[PASSWORD_MAX_LEN+1];
  unsigned char neg_state[MAX_TELOPT+1];
} tnet_handler_struct_type;

typedef struct tnet_handler_struct_type tnet_handler_type;


/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/
void tnet_handler_init(tnet_handler_struct_type *tnet, 
                       void *cb, 
                       tnet_write_function_type *write_func);

void tnet_handler_destroy(tnet_handler_struct_type *tnet);

/* Return 1 if success, 0 if there's not room */
int tnet_add_to_buffer(tnet_handler_struct_type *tnet, 
                       unsigned char *buf, 
                       ssize_t len);

void tnet_clear_buffer(tnet_handler_struct_type *tnet);

void tnet_used_buffer(tnet_handler_struct_type *tnet, ssize_t len);


void tnet_send_IAC_escaped(tnet_handler_type *tnet, 
                           unsigned char *buf , 
                           int len);

void tnet_send_string_IAC_escaped(tnet_handler_type *tnet, 
                                  unsigned char *s);

/* Send the specific telnet command + option to tnet */
void tnet_send_option(tnet_handler_type *tnet, 
                      unsigned char command, char option);

ssize_t 
tnet_handle_IAC(tnet_handler_struct_type *tnet, 
                unsigned char *buf, 
                ssize_t len,
                int only_allow_negotiation);


#endif /* TNETOPT_H */
/****************** END OF FILE tnetopt.h ***********************************/


