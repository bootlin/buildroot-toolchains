/*!***************************************************************************
*!
*! FILE NAME  : axcomp.h
*!
*! DESCRIPTION: Telnet COM Port Option according to Axis "standard"
*!
*! FUNCTIONS  : handle_axcomp_command
*! (EXPORTED)   axcomp_send_notify_modemstate
*!              axcomp_send_int_command
*!              axcomp_send_str_command
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Dec 20 1999  Johan Adolfsson    Initial version
*! $Log: axcomp.h,v $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.3  2000/10/05 14:05:25  johana
*! Review changes
*!
*! Revision 1.2  2000/09/15 15:54:12  johana
*! Looks like a working version.
*!
*! Revision 1.1  2000/09/11 16:35:46  johana
*! Merged with changes by lenamn. Still some things to do.
*!
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 2000, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: axcomp.h,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

#ifndef AXCOMP_H
#define AXCOMP_H

/****************** INCLUDE FILES SECTION ***********************************/
#include "tnetopt.h"

/****************** CONSTANT AND MACRO SECTION ******************************/

/* Axcomp. The different COMP commands */
#define COMP_STATUSON                0
#define COMP_STATUSOFF               1
#define COMP_STATUS                  2
#define COMP_CONTROL                 3
#define COMP_SETCONFIG               4

/* These defines match the comp_id_strings[] in axcomp.c */
/* Axcomp. COMP_SETCONFIG codes */
#define COMP_ID_BAUDRATE             0
#define COMP_ID_BYTESIZE             1
#define COMP_ID_PARITY               2
#define COMP_ID_STOPBITS             3
#define COMP_ID_XONCHAR              4
#define COMP_ID_XOFFCHAR             5
#define COMP_ID_EOFCHAR              6
#define COMP_ID_FLOW_RTSCTS          7
#define COMP_ID_FLOW_DSRDTR          8
#define COMP_ID_FLOW_XONXOFF         9
#define COMP_ID_RTSTOGGLE           10
#define COMP_ID_BINARY              11

/* Axcomp. COMP_STATUS codes */
#define COMP_ID_RI                  12
#define COMP_ID_DSR                 13
#define COMP_ID_CD                  14
#define COMP_ID_RTS                 15
#define COMP_ID_CTS                 16
#define COMP_ID_DTR                 17
#define COMP_ID_TX                  18
#define COMP_ID_RX                  19

/* Axcomp. COMP_PARITY values */
#define COMP_P_NONE                  1
#define COMP_P_ODD                   2
#define COMP_P_EVEN                  3
#define COMP_P_MARK                  4
#define COMP_P_SPACE                 5

/* Axcomp. COMP_ID_STOPBITS values */
#define COMP_SB_ONESTOPBIT           1
#define COMP_SB_TWOSTOPBIT           2
#define COMP_SB_ONE5STOPBIT          3

/* AXCOMP. Boolean value */
#define COMP_FALSE                   0
#define COMP_TRUE                    1


/****************** TYPE DEFINITION SECTION *********************************/

/****************** EXPORTED FUNCTION DECLARATION SECTION *******************/



/* Handling of COM Port Control specific commands */
int handle_axcomp_command(tnet_handler_type *tnet, 
                            unsigned char *cmd, size_t size);

void axcomp_send_notify_modemstate(tnet_handler_struct_type *tnet,
                                   unsigned char state,
                                   unsigned char changed);

/* Send the telnet COM command using param as parameter */
void axcomp_send_int_command(tnet_handler_struct_type *tnet,
                              unsigned char operation, 
                              unsigned char command,
                              unsigned long param);

void axcomp_send_str_command(tnet_handler_struct_type *tnet,
                              unsigned char operation,
                              unsigned char command,
                              unsigned char param);
#endif /* AXCOMP_H */
/************************* END OF FILE axcomp.h ******************************/
