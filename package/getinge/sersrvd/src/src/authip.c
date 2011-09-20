/*!***************************************************************************
*!
*! FILE NAME  : authip.c
*!
*! DESCRIPTION: Authenticate IP address
*!
*! FUNCTIONS  : authenticate_ip_address
*! (EXTERNAL)
*!
*! FUNCTIONS  : <The names of the local functions in this file.>
*! (LOCAL)
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Nov 14 2000  Johan Adolfsson    Initial version
*! $Log:
*!  1    NetCOM     1.0         4/16/2007 1:51:35 PM   Gabe Siftar     Initial
*!       version
*! $
*! Revision 1.1  2003/12/18 22:25:39  gsiftar
*! Initial version from Axis
*!
*! Revision 1.1  2000/11/14 14:33:55  johana
*! Added support for user authentication and IP number verification.
*!
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 2000, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: authip.c,v 1.0, 2007-04-16 17:51:35Z, Gabe Siftar$ */

/****************** INCLUDE FILES SECTION ***********************************/

#include "authip.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/****************** CONSTANT AND MACRO SECTION ******************************/
#define SECURITYD(x)

/****************** TYPE DEFINITION SECTION *********************************/

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

/****************** FUNCTION DEFINITION SECTION *****************************/

/*#***************************************************************************
*#
*# FUNCTION NAME: cgi_authenticate_ip_address
*#
*# PARAMETERS   : const char *ip_list, - String with allowed IP nbrs
*#                struct in_addr ip    - the IP to check
*#
*# RETURNS      : int, >0 if authenticated ok.
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Check an ip_addr against allowed ip-addresses in in_list
*#                Allowed ip-addresses:
*#                123.45.67.89,
*#                123.45.68.*,
*#                123.45.67.80-85,
*#                123.45.67.80-68.85
*#
*#----------------------------------------------------------------------------
*# HISTORY
*#
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# Nov 14 2000  Johan Adolfsson    Initial version, based on function in
*#                                 cgi_authentication.c
*#
*#***************************************************************************/
int authenticate_ip_address(const char *ip_list,
                            struct in_addr ip)
{

  int ok = 0;
  const char *s = ip_list;
  int num_ip_entrys = 0;
  unsigned char ip_addr[4];
  memcpy(ip_addr, &ip, 4);
  
  SECURITYD(printf("authenticate ip_addr: %u.%u.%u.%u list: %s\n",
                   ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3],
                   ip_list));
  /* Find start of IP address */
  while (!isdigit(*s) && *s != '\0')
  {
    s++;
  }
  if (s != '\0')
  {
    unsigned char ip_min[4];
    unsigned char ip_max[4];
    const char *ip_str;
    int i;
    int ip_pos;
    const char *s_start;

    do
    {
      /* make sure we're at start of IP: */
      s_start = s;

      SECURITYD(printf("checking IP: '%s'\n", s));
      num_ip_entrys++;
      ip_str = s;
      ip_pos = 0;
      
      i = 0;
      while (i < 4)
      {
        if (ip_str[ip_pos] == '*')
        {
          ip_min[i] = 0;
          ip_max[i] = 255;
          ip_pos += 2;
        }
        else
        {
          ip_min[i] = atoi(&ip_str[ip_pos]);
          ip_max[i] = ip_min[i];
          while (isdigit(ip_str[ip_pos]))
          {
            ip_pos++;
          }
          if (i < 3)
          {
            ip_pos++; /* skip the . */
          }
        }
        i++;
      }
      
      if (ip_str[ip_pos] == '-')
      {
        unsigned char ip_range[4];
        SECURITYD(printf("parsing range: %s\n",&ip_str[ip_pos]));
        i = 0;
        
        do
        {
          ip_pos++; /* skip first '-' or following '.' */
          
          ip_range[i] = atoi(&ip_str[ip_pos]);
          while (isdigit(ip_str[ip_pos]))
          {
            ip_pos++;
          }
          i++;
        }while(i < 4 && ip_str[ip_pos] == '.');
        SECURITYD(printf("ip_range : %u.%u.%u.%u\n",
                         ip_range[0], ip_range[1],
                         ip_range[2], ip_range[3]));
        /* modify ip_max */
        /* 1.2.3.4 --> 1.2.3.4 in ip_range --> 1.2.3.4 in ip_max */
        /* 1.2 --> 1.2 in ip_range --> x.x.1.2 in ip_max */
        {
          int pos = 0;
          while (pos < i)
          {
            ip_max[4 - i + pos] = ip_range[pos];
            pos++;
          }
        }
      }
      SECURITYD(printf("ip_addr: %u.%u.%u.%u\n",
                       ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]));
      SECURITYD(printf("ip_min : %u.%u.%u.%u\n",
                       ip_min[0],ip_min[1],ip_min[2],ip_min[3]));
      SECURITYD(printf("ip_max : %u.%u.%u.%u\n",
                       ip_max[0],ip_max[1],ip_max[2],ip_max[3]));
      
      i=0;
      while (i < 4 && ((ip_min[i] <= ip_addr[i]) &&
                       (ip_addr[i] <= ip_max[i])) )
      {
        i++;
      }
      if (i == 4) /* all digits checked ok */
      {
        /* ok */
        SECURITYD(printf("IP ok\n"));
        ok = 1;
      }
      else
      {
        SECURITYD(printf("IP failed i=%i\n",i));
      }
      /* go past */
      s += ip_pos;
      while (*s && !isdigit(*s) )
      {
        s++;
      }
    }while(!ok && s!= NULL && *s != '\0');
  }

  if (num_ip_entrys == 0)
  {
    ok = 1;
    SECURITYD(printf("IP ok, no IP entrys\n"));
  }
  return ok;
} /* authenticate_ip_address */


/****************** END OF FILE authip.c ************************************/
