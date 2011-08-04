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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include "respawnd.h"



/*
 * Converts an argv to a cmdline. Returns size of the cmdline or 0 if it fails
 * (fails if cmdline gets longer than RESPAWND_CMDLINE_MAX characters or argc
 * is zero).
 */
size_t argv2cmdline(int argc, char *const *argv, char *cmdline)
{
	size_t cmdsize_left = RESPAWND_CMDLINE_MAX;
	size_t argsize;

	while (argc--) {
		argsize = strlen(*argv) + 1;
		if (argsize > cmdsize_left) {
			/* Not enough space in cmdline. */
			return 0;
		}
		cmdsize_left -= argsize;
		memcpy(cmdline, *argv, argsize);
		cmdline += argsize;
		argv++;
	}

	return RESPAWND_CMDLINE_MAX - cmdsize_left;
}



/*
 * Sends a message to respawnd.
 */
int send_respawnd(const struct respawnd_message *msg)
{
	struct sockaddr_un sunx;
	int fd;

	memset(&sunx, 0, sizeof(sunx));
	sunx.sun_family = AF_UNIX;
	if ((strlen(RESPAWND_SOCKET_PATH) + 1) > (sizeof sunx.sun_path)) {
		fprintf(stderr, "Socket path to long: \"%s\"!\n",
			RESPAWND_SOCKET_PATH);
		return 0;
	}
	strcpy(sunx.sun_path, RESPAWND_SOCKET_PATH);

	if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("socket");

		return 0;
	}
	if (sendto(fd, msg, sizeof *msg, 0, (struct sockaddr *)&sunx,
		   sizeof sunx) < 0) {
		perror("sendto");
		close(fd);

		return 0;
	}
	close(fd);

	return 1;
}



extern int respawnd_main(int argc, char *const *argv);

/*
 * The program can be invoked as "respawn-on", "respawn-off" or other
 * ("respawnd").
 *
 * respawnd is the supervising daemon, respawn-on registers cmdlines for
 * supervision and respawn-off unregisters cmdlines or process-names (i.e. all
 * cmdlines that matches that process-name).
 */
int main(int argc, char **argv)
{
	const char *name;

	if (argc < 1) {
		fprintf(stderr, "Need at least one argument: respawnd, "
				"respawn-on or respawn-off!\n");
		exit(EXIT_FAILURE);
	}

	/* Find out the basename of this program. */
	name = strrchr(argv[0], '/');
	if (name) {
		name++; /* skip the slash */
	} else {
		name = argv[0];
	}

	if ((strcmp(name, "respawn-on") == 0) ||
	    (strcmp(name, "respawn-off") == 0)) {
		/*
		 * Main function when invoked as respawn-on or respawn-off.
		 * Sends a message to respawnd, telling it to ignore a cmdline
		 * or process-name.
		 */
		struct respawnd_message msg;

		if (argc < 2) {
			fprintf(stderr, "Usage: %s program [args]\n", name);
			return EXIT_FAILURE;
		}

		msg.magic = RESPAWND_MESSAGE_MAGIC;
		msg.action = (strcmp(name, "respawn-on") == 0)?
				RESPAWND_ACTION_RESPAWN_ON :
				RESPAWND_ACTION_RESPAWN_OFF;
		msg.cmdsize = argv2cmdline(argc - 1, argv + 1, msg.cmdline);
		if (!msg.cmdsize) {
			fprintf(stderr, "%s: Command line too long! "
					"Max %d characters.\n",
				name, RESPAWND_CMDLINE_MAX);
			return EXIT_FAILURE;
		}

		if (!send_respawnd(&msg)) {
			fprintf(stderr, "%s: Could not send message to "
					"respawnd!\n", name);
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	/*
	 * Not invoked as respawn-on or respawn-off. Be respawnd no matter what
	 * argv says.
	 */
	return respawnd_main(argc, argv);
}
