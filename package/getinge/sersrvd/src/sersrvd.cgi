#!/bin/sh
#
# CGI-script menu for system and application configuration
#

# Check our code to see if we have 305 (0x0131)

PAGE_TITLE="REMOTE SERIAL CONFIGURATION"
FUNCTION_AVAILABLE=nclic -f 131

/bin/websetup sersrvd
