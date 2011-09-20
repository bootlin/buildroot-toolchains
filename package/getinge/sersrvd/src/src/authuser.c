/*!***************************************************************************
*!
*! FILE NAME  : authuser.c
*!
*! DESCRIPTION: Authentication of users
*!
*! FUNCTIONS  : check_login
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
*! Nov 13 2000  Johan Adolfsson    Initial version
*! 
*! ---------------------------------------------------------------------------
*! (C) Copyright 2000, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: authuser.c,v 1.1 2003/12/18 22:25:39 gsiftar Exp $ */

/****************** INCLUDE FILES SECTION ***********************************/

#include "authuser.h"

#include <pwd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>



/****************** CONSTANT AND MACRO SECTION ******************************/
#ifdef CRIS
#define SHADOW_PASSWD_SUPPORT 0
#else
#define SHADOW_PASSWD_SUPPORT 1
#define SHADOW_PASSWD_FILE "/etc/shadow"
#endif

#define D(x)
/****************** TYPE DEFINITION SECTION *********************************/

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

/****************** FUNCTION DEFINITION SECTION *****************************/

/* Taken from apps/sftpd/ */
int
check_login( const char* user, const char* pass, uid_t *uid, gid_t *gid )
{
  struct passwd *ptr;

  /* get passwords for each user */
  setpwent();
  ptr = getpwent();
#if SHADOW_PASSWD_SUPPORT
  if(strcmp(ptr->pw_passwd, "x") == 0) {
    /* Yes, use shadow password file */
    FILE *f;
    endpwent();
    D(printf("Using shadow passwords\n"));
    
    f = fopen(SHADOW_PASSWD_FILE, "r");
    if(f == NULL){
      D(printf("Failed to open shadow password file\n"));
      return -1;
    }
    ptr = fgetpwent(f);
    while ( ptr != NULL ) {
      if(strcmp(user, ptr->pw_name) == 0 ) 
      {
        D(printf("Found user in shadow password\n"));
        /* found user in passwd */
        /* Check against the encrypted password.  */
        if(strcmp(crypt(pass, ptr->pw_passwd),
                  ptr->pw_passwd ) == 0 ) {
          *uid = ptr->pw_uid;
          *gid = ptr->pw_gid;
          fclose(f);
          return 0;
        }
        /* bad password */
        D(printf("Bad password\n"));
        fclose(f);
        return -1;
      }
      ptr = fgetpwent(f);
    }
    fclose(f);
    /* username not found */
    return -1;
  }
#endif /* SHADOW_PASSWD_SUPPORT */
  while ( ptr != NULL ) {
    if(strcmp(user, ptr->pw_name) == 0) {
      D(printf("Found user\n"));
      /* found user in passwd */
      /* Check against the encrypted password.  */
      if (strcmp(crypt(pass, ptr->pw_passwd),
                 ptr->pw_passwd) == 0) {
        *uid = ptr->pw_uid;
        *gid = ptr->pw_gid;
        endpwent();
        return 0;
      }
      /* bad password */
      endpwent();
      return -1;
    }
    ptr = getpwent();
  }
  endpwent();
  /* username not found */
  return -1;
}


int authenticate_user(const char *username, const char *passwd, 
                      const char *user_list)
{
  int ok = 0;
  const char *s = user_list;
  int len = strlen(username);
  
  /* Check if username is in user_list */
  s = strstr(s, username);
  while (s != NULL)
  {
    /* Is end of name ok? */
    if (s != NULL && (s[len] == '\0' || 
                      s[len] == ' ' || 
                      s[len] == ',' ||
                      s[len] == ';'))
    {
      /* yes, check character before s */
      s--;
      if (s < user_list || 
          *s == '\0' || 
          *s == ' ' || 
          *s == ',' ||
          *s == ';')
      {
        uid_t uid;
        gid_t gid;
        D(printf("checking user: %s:%s\n", username, passwd));
        /* Yes, the user is ok, check password */
        if (check_login(username, passwd, &uid, &gid) == 0)
        {
          return 1; /* ok! */
        }
        else
        {
          D(printf("Authorisation failed!\n"));
          return 0;
        }
      }
    }
    s += len;
    s = strstr(s, username);
  }
  
  /* User ok, check if the password is ok */

  return ok;
}




/****************** END OF FILE authuser.c ***********************************/
