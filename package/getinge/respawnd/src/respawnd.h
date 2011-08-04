/*
 *  respawnd - supervises and respawns processes that dies unexpectedly
 *  Author: Jonas Holmberg <jonashg@axis.com>
 *  Copyright (C) 2003  Axis Communications AB
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stddef.h>
#include <stdint.h>
#include <syslog.h>

#define RESPAWND_MESSAGE_MAGIC 0x20030129
#define RESPAWND_ACTION_RESPAWN_ON 0x01
#define RESPAWND_ACTION_RESPAWN_OFF 0x02
#define RESPAWND_CMDLINE_MAX 255
#define RESPAWND_SOCKET_PATH "/var/run/respawnd/control"

#ifdef DEBUG
#define debug(...) syslog(LOG_DEBUG, __VA_ARGS__)
#else
#define debug(...) ((void)0)
#endif

#define warning(...) syslog(LOG_WARNING, __VA_ARGS__)
#define error(...) syslog(LOG_ERR, __VA_ARGS__)
#define syserror(x) syslog(LOG_ERR, "%s: %m", (x))

struct respawnd_message {
	uint32_t magic;
	uint8_t action;
	size_t cmdsize;
	char cmdline[RESPAWND_CMDLINE_MAX];
};

size_t argv2cmdline(int argc, char *const *argv, char *cmdline);
