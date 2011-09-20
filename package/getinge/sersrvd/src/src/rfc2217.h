/*!***************************************************************************
*!
*! FILE NAME  : rfc2217.h
*!
*! DESCRIPTION: Telnet COM Port Option according to RFC 2217
*!
*! FUNCTIONS  : handle_rfc2217_command
*! (EXPORTED)   rfc2217_send_byte_command
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Dec 20 1999  Johan Adolfsson    Initial version
*! $Log: rfc2217.h,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.7  2001/01/15 14:05:07  johana
*! Added rfc2217_init_connection()
*!
*! Revision 1.6  2001/01/09 19:02:24  johana
*! Added rfc2217_send_flow_command and fixed RESUME/SUSPEND
*!
*! Revision 1.5  2000/10/05 14:05:25  johana
*! Review changes
*!
*! Revision 1.4  2000/09/18 09:08:35  johana
*! Clean up
*!
*! Revision 1.3  2000/09/11 16:35:46  johana
*! Merged with changes by lenamn. Still some things to do.
*!
*! Revision 1.2  2000/02/23 11:51:17  johana
*! Fixed header
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: rfc2217.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef RFC2217_H
#define RFC2217_H

/****************** INCLUDE FILES SECTION ***********************************/
#include "tnetopt.h"

/****************** CONSTANT AND MACRO SECTION ******************************/

/* COM-PORT-OPTION according to RFC2217, subnegotiation codes */
/*
                   Client to Access Server   Access Server to Client
       SIGNATURE               0 ?  text              100 ?   text
       SET-BAUDRATE            1                      101
       SET-DATASIZE            2                      102
       SET-PARITY              3                      103
       SET-STOPSIZE            4                      104
       SET-CONTROL             5                      105
       NOTIFY-LINESTATE        6                      106
       NOTIFY-MODEMSTATE       7                      107
       FLOWCONTROL-SUSPEND     8                      108
       FLOWCONTROL-RESUME      9                      109
       SET-LINESTATE-MASK     10                      110
       SET-MODEMSTATE-MASK    11                      111
       PURGE-DATA             12                      112

*/
/* From Client to Access Server: */
#define COMC_SIGNATURE               0 
#define COMC_SET_BAUDRATE            1 
#define COMC_SET_DATASIZE            2 
#define COMC_SET_PARITY              3 
#define COMC_SET_STOPSIZE            4 
#define COMC_SET_CONTROL             5 
#define COMC_NOTIFY_LINESTATE        6 
#define COMC_NOTIFY_MODEMSTATE       7 
#define COMC_FLOWCONTROL_SUSPEND     8 
#define COMC_FLOWCONTROL_RESUME      9 
#define COMC_SET_LINESTATE_MASK     10 
#define COMC_SET_MODEMSTATE_MASK    11 
#define COMC_PURGE_DATA             12 

/* From Access Server to Client: */
#define COMS_SIGNATURE                         100 
#define COMS_SET_BAUDRATE                      101
#define COMS_SET_DATASIZE                      102
#define COMS_SET_PARITY                        103
#define COMS_SET_STOPSIZE                      104
#define COMS_SET_CONTROL                       105
#define COMS_NOTIFY_LINESTATE                  106
#define COMS_NOTIFY_MODEMSTATE                 107
#define COMS_FLOWCONTROL_SUSPEND               108
#define COMS_FLOWCONTROL_RESUME                109
#define COMS_SET_LINESTATE_MASK                110
#define COMS_SET_MODEMSTATE_MASK               111
#define COMS_PURGE_DATA                        112



/* RFC2217 command values:
 General a value of 0 indicates we want to receive the current value

IAC SB COM-PORT-OPTION SIGNATURE <text> IAC SE
  <text> should be escaped (IAC to IAC IAC)

IAC SB COM-PORT-OPTION SET-BAUD <value(4)> IAC SE
  <value> is the baudrate in standard network format, 0 = query.

IAC SB COM-PORT-OPTION SET-DATASIZE <value> IAC SE
  <value> is one byte with the number of bits (5,6,7,8) 0 = query.

IAC SB COM-PORT-OPTION SET-PARITY <value> IAC SE
       This command is sent by the client to the access server to set
       the parity.  The command can also be sent to query the current
       parity. The value is one octet (byte). The value is an index into
       the following value table:

            Value      Parity [1]
              0           Request Current Data Size
              1           NONE
              2           ODD
              3           EVEN
              4           MARK
              5           SPACE
              6-127       Available for Future Use
*/


/*
IAC SB COM-PORT-OPTION SET-STOPSIZE <value> IAC SE
       This command is sent by the client to the access server to set
       the number of stop bits. The command can also be sent to query
       the current stop bit size. The value is one octet (byte). The
       value is an index into the following value table:

           Value      Stop Bit Size
             0           Request Current Data Size
             1           1
             2           2
             3           1.5
             4-127       Available for Future Use
 */


/*
IAC SB COM-PORT-OPTION SET-CONTROL <value> IAC SE
       This command is sent by the client to the access server to set
       special com port options. The command can also be sent to query
       the current option value. The value is one octet (byte). The
       value is an index into the following value table:

           Value      Control Commands
             0           Request Com Port Flow Control Setting
                           (outbound/both)
             1           Use No Flow Control (outbound/both)
             2           Use XON/XOFF Flow Control (outbound/both)
             3           Use HARDWARE Flow Control (outbound/both)
             4           Request BREAK State
             5           Set BREAK State ON
             6           Set BREAK State OFF
             7           Request DTR Signal State
             8           Set DTR Signal State ON
             9           Set DTR Signal State OFF
            10           Request RTS Signal State
            11           Set RTS Signal State ON
            12           Set RTS Signal State OFF
            13           Request Com Port Flow Control Setting (inbound)
            14           Use No Flow Control (inbound)
            15           Use XON/XOFF Flow Control (inbound)
            16           Use HARDWARE Flow Control (inbound)
            17           Use DCD Flow Control (outbound/both)
            18           Use DTR Flow Control (inbound)
            19           Use DSR Flow Control (outbound/both)
            20-127       Available for Future Use
*/

/*
IAC SB COM-PORT-OPTION SET-LINESTATE-MASK <value> IAC SE
       This command is sent by the client to the access server to set a
       bit mask for the sending of the NOTIFY-LINESTATE option (see
       section 4).  When the LINESTATE changes on the access server, the
       access server will "AND" the new LINESTATE with the LINESTATE-
       MASK.  If the result is not zero, the access server will send the
       result of the "AND" as the value in a NOTIFY-LINESTATE com port
       option. If more than one bit satisfies the LINESTATE-MASK, only
       one NOTIFY-LINESTATE, with all the satisfying bits, will be sent
       to the client.  The SET-LINESTATE-MASK may be any combination of
       bits as listed below.  These are the same bit values used in the
       NOTIFY-LINESTATE option.  The SET-LINESTATE-MASK values are based
       on the most popular UART (com port control chip) in use [1].

           Bit Position     Value     Meaning
              7              128         Time-out Error
              6               64         Transfer Shift Register Empty
              5               32         Transfer Holding Register Empty
              4               16         Break-detect Error
              3                8         Framing Error
              2                4         Parity Error
              1                2         Overrun Error
              0                1         Data Ready
*/


/*
IAC SB COM-PORT-OPTION SET-MODEMSTATE-MASK <value> IAC SE
       This command is sent by the client to the access server to set a
       bit mask for the sending of the NOTIFY-MODEMSTATE option (see
       section 4).  When the MODEMSTATE changes on the access server,
       the access server will "AND" the new MODEMSTATE with the
       MODEMSTATE-MASK.  If the result is not zero, the access server
       will send the result of the "AND" as the value in a NOTIFY-
       MODEMSTATE com port option. If more than one bit satisfies the
       MODEMSTATE-MASK, only one NOTIFY-MODEMSTATE, with all the
       satisfying bits, will be sent to the client.  The SET-
       MODEMSTATE-MASK may be any combination of bits as listed below.
       These are the same bit values used in the NOTIFY-MODEMSTATE
       option.  The SET-MODEMSTATE-MASK values are based on the most
       popular UART (com port control chip) in use [1].

           Bit Position     Value     Meaning
              7              128        Receive Line Signal Detect
                                        (also known as Carrier Detect)
              6               64        Ring Indicator
              5               32        Data-Set-Ready Signal State
              4               16        Clear-To-Send Signal State
              3                8        Delta Receive Line Signal Detect
              2                4        Trailing-edge Ring Detector
              1                2        Delta Data-Set-Ready
              0                1        Delta Clear-To-Send

       Discussion: The SET-MODEMSTATE-MASK value of 0 will prevent the
                   access server from sending NOTIFY-MODEMSTATE options
                   to the client.

       Discussion: The SET-MODEMSTATE-MASK value of 255 will allow the
                   access server to send a NOTIFY-MODEMSTATE option to
                   the client each time the MODEMSTATE changes on the
                   access server.

       Discussion: The initial MODEMSTATE-MASK at the access server
                   is 255.

       Discussion: The client does not have to send a new
                   SET-MODEMSTATE-MASK after receiving a NOTIFY-
                   MODEMSTATE.  The MODEMSTATE-MASK on the access server
                   is retained until set by the client or reset at the
                   start of a new Telnet session.
*/

/*
IAC SB COM-PORT-OPTION PURGE-DATA <value> IAC SE
       This command is sent by the client to the access server to
       instruct the access server to immediately clear all data from the
       buffer or buffers referenced by the value.  The value is one
       octet (byte).  The value is an index into the following value
       table:

           Value      Purge Data Buffer
             0           Available for Future Use
             1           Purge access server receive data buffer
             2           Purge access server transmit data buffer
             3           Purge both the access server receive data
                         buffer and the access server transmit data
                         buffer
             4-127       Available for Future Use

*/

/*
4. Notification of Com port and Modem Line Changes

   The access server can send these commands to the client any time
   and multiple times throughout the Telnet session. The access
   server should send the appropriate command to the client as soon
   as the com port or modem line changes occurs.  The client does
   not issue a response to these commands.

IAC SB COM-PORT-OPTION NOTIFY-LINESTATE <value> IAC SE
       The value is one octet (byte). The value is a bit level
       composition made up from the value table below. Multiple bit
       values may be set in a single transmission. The values are based
       on the most popular UART (com port control chip) in use [1].

           Bit Position     Value     Meaning
              7              128         Time-out Error
              6               64         Transfer Shift Register Empty
              5               32         Transfer Holding Register Empty
              4               16         Break-detect Error
              3                8         Framing Error
              2                4         Parity Error
              1                2         Overrun Error
              0                1         Data Ready


       Discussion: The LINESTATE is the line state of the UART on
                   the access server.
*/



/*
IAC SB COM-PORT-OPTION NOTIFY-MODEMSTATE <value> IAC SE
       The value is one octet (byte). The value is a bit level
       composition made up from the value table below. Multiple bit
       values may be set in a single transmission. The values are based
       on the most popular UART (com port control chip) in use [1].

           Bit Position     Value     Meaning
              7              128        Receive Line Signal Detect
                                        (also known as Carrier Detect)
              6               64        Ring Indicator
              5               32        Data-Set-Ready Signal State
              4               16        Clear-To-Send Signal State
              3                8        Delta Receive Line Signal Detect
              2                4        Trailing-edge Ring Detector
              1                2        Delta Data-Set-Ready
              0                1        Delta Clear-To-Send
*/


/*
5. Flow Control

   The client and/or access server can send these commands any time and
   multiple times throughout the Telnet session.

IAC SB COM-PORT-OPTION FLOWCONTROL-SUSPEND IAC SE
       The sender of this command is requesting that the receiver
       suspend transmission of both data and commands until the
       FLOWCONTROL-RESUME is transmitted by the sender.

IAC SB COM-PORT-OPTION FLOWCONTROL-RESUME IAC SE
       The sender of this command is requesting that the receiver resume
       transmission of both data and commands.

*/


/****************** TYPE DEFINITION SECTION *********************************/

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/

void rfc2217_init_connection(tnet_handler_type *tnet);


/* Handling of COM Port Control specific commands */
int handle_rfc2217_command(tnet_handler_type *tnet, 
                            unsigned char *cmd, size_t size);

/* Send the telnet COM command using param as parameter */
void rfc2217_send_byte_command(tnet_handler_struct_type *tnet, 
                               unsigned char command, 
                               unsigned char param);

void rfc2217_send_flow_command(tnet_handler_type *tnet, unsigned char cmd);


#endif /* RFC2217_H */
/****************** END OF FILE rfc2217.h ***********************************/
