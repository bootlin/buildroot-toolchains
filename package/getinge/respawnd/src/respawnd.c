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

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>

#include "respawnd.h"

#define TIMEOUT_SECS 3

/* Enable OVERDEBUG if you want periodical debug output. */
#if 0 && defined DEBUG
#define OVERDEBUG
#endif



/*
 * Compares two cmdlines. Evaluates to 1 if cmdlines (and sizes) are equal,
 * 0 otherwise.
 */
#define CMDLINES_EQUAL(cmdline1, cmdsize1, cmdline2, cmdsize2) \
	(((cmdsize1) == (cmdsize2)) && \
	 (memcmp((cmdline1), (cmdline2), (cmdsize1)) == 0))



/* A linked list of processes to supervise (or ignore). */
static struct node {
	size_t cmdsize;    /* size of cmdline */
	char *cmdline;     /* the cmdline to supervise (or ignore) */
	int ignore;        /* ignore or supervise (boolean) */
	size_t pids_size;  /* size of pids */
	pid_t *pids;       /* dynamically allocated array of pids */
	int numpids;       /* number of pids found when checking processes */
	struct node *next; /* next node in list */
} *head = NULL;



/*
 * Translates a cmdline to printable form (it may contain null characters).
 * A call to this function destroys the value at a previously returned pointer.
 */
static const char *cmdline2str(const char *cmdline, size_t cmdsize)
{
	static char ret[RESPAWND_CMDLINE_MAX];
	size_t len = 0;

	assert(cmdsize <= RESPAWND_CMDLINE_MAX);

	if (cmdline && (cmdsize > 0)) {
		memcpy(ret, cmdline, cmdsize);

		len += (strlen(ret + len) + 1);
		while (len < cmdsize) {
			ret[len - 1] = ' ';
			len += (strlen(ret + len) + 1);
		}
	} else {
		ret[0] = 0;
	}

	return ret;
}



/*
 * Allocates and initializes a new node.
 */
struct node *new_node(const char *cmdline, size_t cmdsize)
{
	struct node *new;

	if (!(new = malloc(sizeof *new))) {
		error("malloc failed!\n");
		return NULL;
	}
	new->cmdsize = cmdsize;
	new->cmdline = malloc(cmdsize);
	if (!new->cmdline) {
		error("malloc failed!\n");
		free(new);
		return NULL;
	}
	memcpy(new->cmdline, cmdline, cmdsize);
	new->ignore = 0;
	new->pids_size = 0;
	new->pids = NULL;
	new->next = NULL;

	return new;
}



/*
 * Put a cmdline at the end of the linked list of supervised nodes.
 */
static void supervise_cmdline(const char *cmdline, size_t cmdsize)
{
	struct node *tmp = head;
	struct node *tail = head;
	struct node *new = NULL;

	/* See if it already exists in list and find the tail node. */
	while (tmp) {
		if (CMDLINES_EQUAL(tmp->cmdline, tmp->cmdsize,
				   cmdline, cmdsize)) {
			debug("Found \"%s\"\n", cmdline2str(cmdline, cmdsize));
			tmp->ignore = 0;

			/* Remove it from list. */
			if (tmp == head) {
				head = tmp->next;
			} else {
				tail->next = tmp->next;
			}

			/* Save the address in order to reuse the node. */
			new = tmp;
		} else {
			tail = tmp;
		}
		tmp = tmp->next;
	}

	if (new) {
		new->next = NULL;
		debug("Reusing old node for \"%s\".\n",
		      cmdline2str(cmdline, cmdsize));
	} else if (!(new = new_node(cmdline, cmdsize))) {
		error("Failed to allocate new node for \"%s\"!\n",
		      cmdline2str(cmdline, cmdsize));
		return;
	}

	/* Put it at the end of list. */
	if (head) {
		tail->next = new;
	} else {
		head = new;
	}
	debug("Supervising \"%s\".\n", cmdline2str(cmdline, cmdsize));
}



/*
 * Mark the cmdline as ignored in the linked list of supervised nodes.
 */
static void ignore_cmdline(const char *cmdline, size_t cmdsize)
{
	struct node *tmp = head;

	/* Find it. */
	while (tmp) {
		if (CMDLINES_EQUAL(tmp->cmdline, tmp->cmdsize,
				   cmdline, cmdsize)) {
			/* Found exact match. */
			tmp->ignore = 1;
			debug("No longer supervising \"%s\".\n",
			      cmdline2str(cmdline, cmdsize));
			break;
		}

		if (strchr(cmdline, '/')) {
			/* Look for argv[0]. */
			if (strcmp(tmp->cmdline, cmdline) == 0) {
				/* Found it. */
				tmp->ignore = 1;
				debug("No longer supervising name \"%s\"\n",
				      cmdline2str(cmdline, cmdsize));
			}
		} else {
			/* Look for process-name. */
			const char *basename;

			if ((basename = strrchr(tmp->cmdline, '/'))) {
				basename++; /* skip '/' */
			} else {
				basename = tmp->cmdline;
			}
			if (strcmp(cmdline, basename) == 0) {
				/* Found the process-name. */
				tmp->ignore = 1;
				debug("No longer supervising name \"%s\"\n",
				      cmdline2str(cmdline, cmdsize));
			}
		}
		tmp = tmp->next;
	}
}



/*
 * Start a new instance (process) of cmdline.
 */
static int start(char *cmdline, size_t cmdsize)
{
	pid_t pid;
	int status;

	if ((pid = fork()) < 0) {
		syserror("fork");
		return 0;
	} else if (!pid) {
		/* Child */
		int argc;
		char **argv;
		int idx;

		idx = 0;
		argc = 0;
		while (idx < cmdsize) {
			idx += strlen(cmdline + idx);
			idx++; /* skip null character */
			argc++;
		}

		argv = malloc((argc + 1) * (sizeof *argv));
		if (!argv) {
			error("malloc of argv failed!\n");
			exit(EXIT_FAILURE);
		}

		idx = 0;
		argc = 0;
		while (idx < cmdsize) {
			argv[argc] = cmdline + idx;
			idx += strlen(cmdline + idx);
			idx++; /* skip null character */
			argc++;
		}
		argv[argc] = NULL;

		execv(argv[0], argv);
		error("execv failed!\n");
		exit(EXIT_FAILURE);
	}

	/* Parent */
	debug("Waiting for child (pid = %ld) to die.\n", (long)pid);

	/*
	 * To make sure that we respawn processes in the same order they were
	 * registered we must wait for this child to exit before we respawn the
	 * next process.
	 */
	while (waitpid(pid, &status, 0) < 0) {
		syserror("waitpid");
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != EXIT_SUCCESS) {
			warning("Child pid %ld exited with status %d.\n",
				(long)pid, (int)WEXITSTATUS(status));
		} else {
			debug("Child pid %ld exited with status %d.\n",
			      (long)pid, (int)WEXITSTATUS(status));
		}
	} else {
		/* Terminated by signal */
		warning("Child pid %ld died from signal %d.\n", (long)pid,
			(int)WTERMSIG(status));
	}

	return 1;
}



/*
 * Stop all instances of a cmdline by sending them SIGTERM (and if that doesn't
 * help: SIGKILL).
 */
static int stop(const pid_t *pids, int numpids,
		const char *cmdline, size_t cmdsize)
{
	int alive = 1;
	int signum = SIGTERM;
	int sleep_secs = 1;

	while (alive) {
		int idx;

		if (signum) {
			for (idx = 0; idx < numpids; idx++) {
				debug("Sending signal %d to \"%s\" "
				      "(pid = %ld).\n", signum,
				      cmdline2str(cmdline, cmdsize),
				      (long)pids[idx]);
				if (kill(pids[idx], signum) < 0) {
					if (errno != ESRCH) {
						syserror("kill");
						return 0;
					}
				}
			}
		}

		alive = 0;
		for (idx = 0; !alive && (idx < numpids); idx++) {
			char name[32];
			int fd;
			char buf[RESPAWND_CMDLINE_MAX];
			ssize_t count;

			sprintf(name, "/proc/%ld/cmdline", (long)pids[idx]);
			if ((fd = open(name, O_RDONLY)) < 0) {
				if (errno == ENOENT) {
					debug("\"%s\" (pid = %ld) is dead.\n",
					      cmdline2str(cmdline, cmdsize),
					      (long)pids[idx]);
					continue;
				} else {
					syserror("open /proc/<pid>/cmdline");
					if (signum == SIGKILL) {
						return 0;
					}
					alive = 1;
					break;
				}
			}
			if ((count = read(fd, buf, sizeof buf)) < 0) {
				syserror("read /proc/<pid>/cmdline");
				close(fd);
				if (signum == SIGKILL) {
					return 0;
				}
				alive = 1;
				break;
			}
			if (CMDLINES_EQUAL(buf, count, cmdline, cmdsize)) {
				debug("\"%s\" (pid = %ld) is still alive.\n",
				      cmdline2str(cmdline, cmdsize),
				      (long)pids[idx]);
				alive = 1;
			} else {
				debug("\"%s\" (pid = %ld) is dead.\n",
				      cmdline2str(cmdline, cmdsize),
				      (long)pids[idx]);
			}
			close(fd);
		}
		if (alive) {
			sleep(sleep_secs);
			if (sleep_secs > 2) {
				/* Die, damn it! */
				signum = SIGKILL;
			} else {
				/* Just check if it's alive. */
				signum = 0;
			}
			sleep_secs++;
		}
	}

	return 1;
}



/*
 * Check if all supervised cmdlines are alive. If a cmdline cannot be found,
 * restart that process and all other supervised processes started after the
 * one that died.
 */
static void check_processes(void)
{
	struct node *tmp;
	DIR *procdir;
	struct dirent *entry;
	int proc_empty;
	int kill_next;

	tmp = head;
	while (tmp) {
		tmp->numpids = 0;
		tmp = tmp->next;
	}
	
	if (!(procdir = opendir("/proc"))) {
		syserror("opendir /proc");
		return;
	}

	proc_empty = 1; /* until proven otherwise */
	while ((entry = readdir(procdir))) {
		long pid; /* long type instead of pid_t for portability (%ld) */

		proc_empty = 0;

		if (sscanf(entry->d_name, "%ld", &pid) == 1) {
			int fd;
			char name[32];
			char cmdline[RESPAWND_CMDLINE_MAX];
			ssize_t cmdsize;

			sprintf(name, "/proc/%ld/cmdline", pid);

			if ((fd = open(name, O_RDONLY)) < 0) {
				/* Ignore this pid this time. */
				debug("open %s: %s\n", name, strerror(errno));
				continue;
			}

			if ((cmdsize = read(fd, cmdline, sizeof cmdline)) < 0) {
				debug("read %s: %s\n", name, strerror(errno));
				/* Ignore this pid this time. */
				close(fd);
				continue;
			}

			if (!cmdsize) {
				/* Probably a kernel thread. */
				close(fd);
				continue;
			}

			tmp = head;
			while (tmp) {
				if (CMDLINES_EQUAL(tmp->cmdline, tmp->cmdsize,
						   cmdline, cmdsize)) {
#ifdef OVERDEBUG
					debug("\"%s\": alive (pid = %ld)\n",
					      cmdline2str(cmdline, cmdsize),
					      pid);
#endif
					assert((tmp->numpids *
						sizeof *(tmp->pids)) <=
					       tmp->pids_size);

					if ((tmp->numpids *
					     sizeof *(tmp->pids)) ==
					    tmp->pids_size) {
						/* Need to allocate memory for
						 * storing one more pid. Always
						 * realloc and never free
						 * (reuse instead).
						 */
						pid_t *new_pids;
						size_t new_size;

						new_size = tmp->pids_size +
							   sizeof *(tmp->pids);
						if ((new_pids =
						     realloc(tmp->pids,
							     new_size))) {
							tmp->pids = new_pids;
							tmp->pids_size =
								new_size;
						} else {
							error("Failed to "
							      "reallocate pids "
							      "storage!\n");
							continue;
						}
					}
					tmp->pids[tmp->numpids] = pid;
					tmp->numpids++;

					break;
				}
				tmp = tmp->next;
			}
			close(fd);
		}
	}
	closedir(procdir);

	if (proc_empty) {
		warning("/proc is empty, not mounted?\n");
		return;
	}

	tmp = head;
	kill_next = 0;
	while (tmp) {
		if (!tmp->ignore) {
			if (kill_next && tmp->numpids) {
				error("Stopping \"%s\".\n",
				      cmdline2str(tmp->cmdline, tmp->cmdsize));
				if (stop(tmp->pids, tmp->numpids, tmp->cmdline,
					 tmp->cmdsize)) {
					tmp->numpids = 0;
				} else {
					error("Failed to stop \"%s\"\n",
					      cmdline2str(tmp->cmdline,
							  tmp->cmdsize));
				}
			}
			if (!tmp->numpids) {
				error("Respawning \"%s\".\n",
				      cmdline2str(tmp->cmdline, tmp->cmdsize));
				if (start(tmp->cmdline, tmp->cmdsize)) {
					kill_next = 1;
				} else {
					error("Failed to respawn \"%s\"\n",
					      cmdline2str(tmp->cmdline,
							  tmp->cmdsize));
				}
#ifdef OVERDEBUG
			} else {
				debug("Just checking \"%s\".\n",
				      cmdline2str(tmp->cmdline, tmp->cmdsize));
#endif
			}
#ifdef OVERDEBUG
		} else {
			debug("Ignoring \"%s\"\n",
			      cmdline2str(tmp->cmdline, tmp->cmdsize));
#endif
		}
		tmp = tmp->next;
	}
}



/*
 * Main function when invoked as respawnd. Starts a new instance of respawnd
 * to be able to restart it if respawnd itself dies.
 */
int respawnd_main(int argc, char *const *argv)
{
	const char *socket_path = RESPAWND_SOCKET_PATH;
	char *socket_dir;
	char *last_slash;
	struct sockaddr_un sunx;
	int fd;

	if (argc != 1) {
		fprintf(stderr, "Usage: respawnd\n");
		return EXIT_FAILURE;
	}

	/* Clear permissions for group and other in file creation. */
	umask(S_IRWXG | S_IRWXO);

	openlog("respawnd", LOG_PID | LOG_PERROR, LOG_DAEMON);

	/* Create a directory for the socket. */
	socket_dir = strdup(RESPAWND_SOCKET_PATH);
	if (!socket_dir) {
		error("Could not allocate memory for socket path!\n");
		return EXIT_FAILURE;
	}
	last_slash = strrchr(socket_dir, '/');
	if (last_slash) {
		/* Terminate path before socket name. */
		*last_slash = 0;

		if ((mkdir(socket_dir, S_IRWXU) < 0) && (errno != EEXIST)) {
			syserror("mkdir for " RESPAWND_SOCKET_PATH);
			free(socket_dir);
			return EXIT_FAILURE;
		}
	}
	free(socket_dir);

	memset(&sunx, 0, sizeof sunx);
	sunx.sun_family = AF_UNIX;
	if ((strlen(socket_path) + 1) > (sizeof sunx.sun_path)) {
		error("Socket path to long: \"%s\"!\n", socket_path);
		return EXIT_FAILURE;
	}
	strcpy(sunx.sun_path, socket_path);

	/* Make sure any old sockets are removed. */
	unlink(socket_path);

	if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
		syserror("socket");
		return EXIT_FAILURE;
	}

	if (bind(fd, (struct sockaddr *)&sunx, sizeof sunx) < 0) {
		syserror("bind");
		close(fd);
		return EXIT_FAILURE;
	}

	if (daemon(0, 0) < 0) {
		syserror("daemon");
		close(fd);
		return EXIT_FAILURE;
	}

	while (1) {
		struct timeval tv;
		fd_set readfds;
		int ret;

		tv.tv_sec = TIMEOUT_SECS;
		tv.tv_usec = 0;
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		if ((ret = select(fd + 1, &readfds, NULL, NULL, &tv)) < 0) {
			syserror("select");
			continue;
		}

		if (ret && FD_ISSET(fd, &readfds)) {
			struct respawnd_message msg;
			ssize_t msgsize;

			if ((msgsize = recv(fd, &msg, sizeof msg, 0)) < 0) {
				syserror("recv");
				continue;
			}
			if ((msgsize != sizeof msg) ||
			    (msg.magic != RESPAWND_MESSAGE_MAGIC)) {
				warning("Got bogus message.\n");
				continue;
			}
			debug("Got a message: action = %d, cmdline = \"%s\", "
			      "cmdsize = %lu\n", msg.action,
			      cmdline2str(msg.cmdline, msg.cmdsize),
			      (unsigned long)msg.cmdsize);

			switch (msg.action) {
				case RESPAWND_ACTION_RESPAWN_ON:
					supervise_cmdline(msg.cmdline,
							  msg.cmdsize);
					break;
				case RESPAWND_ACTION_RESPAWN_OFF:
					ignore_cmdline(msg.cmdline,
						       msg.cmdsize);
					break;
				default:
					warning("Invalid action: %d\n",
						msg.action);
					continue;
			}
		} else if (!ret) {
#ifdef OVERDEBUG
			debug("select timeout.\n");
#endif
			check_processes();
		} else {
			/* This can never happen. */
			assert(0);
		}
	}
}
