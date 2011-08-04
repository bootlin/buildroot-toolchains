#
# Shell functions to be used by the scripts in /etc/init.d/.
#

# Terminal colors
NORMAL='\033[0m'
GOOD='\033[32;01m'
WARN='\033[33;01m'
BAD='\033[31;01m'
BRACKET='\033[34;01m'

# Terminal cursor positions.
# It's tempting to use absolute column positions (Esc [ n G sequences), but
# some terminal emulators can't handle them so let's use newline followed by
# relative up and right movement instead.
OKPOS='\n\033[2A\033[72C'
FAILPOS='\n\033[2A\033[71C'
INFOPOS='\n\033[A\033[4C'

#
# Prints warning message(s) to stdout.
#
warning() {
	echo -e "${INFOPOS}${WARN}!${NORMAL} $*"
}

#
# Prints error message(s) to stdout and exits.
#
error() {
	echo -e "${INFOPOS}${BAD}!!!${NORMAL} $*"

	exit 1
}

#
# Prints info message(s) to stdout.
#
information() {
	echo -e "${INFOPOS}${GOOD}+${NORMAL} $*"
}

#
# Prints the beginning of an initscript's output.
#
begin() {
	echo -e " ${GOOD}*${NORMAL} $*... "
}

#
# Prints the end of an initscript's output.
#
end() {
	if [ "$1" = 0 ]; then
		echo -e "${OKPOS}${BRACKET}[ ${GOOD}ok${BRACKET} ]${NORMAL}"
	else
		shift
		if [ -n "$1" ]; then
			echo -e "${INFOPOS}${BAD}!${NORMAL} $*"
		fi

		echo -e "${FAILPOS}${BRACKET}[ ${BAD}fail${BRACKET} ]${NORMAL}"
		exit 1
	fi
}

#
# Checks if any of the arguments are pids of processes that are alive.
#
check_pid() {
	while [ -n "$1" ]; do
		[ -d "/proc/$1" ] && return 0
		shift
	done

	return 1
}

#
# Checks if all arguments exactly matches the command line used to start an
# existing processes.
#
check_cmd() {
	__basename="${1##*/}"
	__basename="${__basename%% *}"

	__pids=`pidof "$__basename"`
	[ -z "$__pids" ] && return 1

	__cmdline=
	for __arg in "$@"; do
		__cmdline="$__cmdline$__arg "
	done

	for __pid in $__pids; do
		[ "`cat /proc/$__pid/cmdline | \
		    tr '\000' ' '`" = "$__cmdline" ] && return 0
	done

	return 1
}

#
# Kills all processes whose pids are among the arguments.
#
kill_pid() {
	kill -TERM "$@" || return 1

	# Any survivors?
	if check_pid "$@" && sleep 1 &&
	   check_pid "$@" && sleep 2 &&
	   check_pid "$@" && sleep 3 &&
	   check_pid "$@"; then
		kill -KILL "$@"
	fi
}

#
# Starts the daemon whose filename is in the first argument. If the last
# argument is an ampersand (&) the daemon will be started in the background.
# Any other arguments will be passed on to the daemon as command line options.
#
start_daemon() {
	_basename="${1##*/}"
	_basename="${_basename%% *}"

	_exec="$1"
	shift

	case "$*" in
		*\ \&)
			if [ "$#" -ge 1 ]; then
				_args="\"$1\""
				_argc=1
				shift
				for _arg in "$@"; do
					if [ "$_argc" -lt "$#" ]; then
						_args="$_args \"$_arg\""
					fi
					_argc=$(($_argc + 1))
				done
			fi
			if eval start-stop-daemon --start --background \
			   --exec \"$_exec\" -- $_args; then
				eval /sbin/respawn-on \"$_exec\" $_args ||
					warning "respawn-on \"$_exec\" " \
						"$_args failed!"
			else
				return 1
			fi
			;;
		*)
			if start-stop-daemon --start \
			   --exec "$_exec" -- "$@"; then
				/sbin/respawn-on "$_exec" "$@" || warning \
					"respawn-on $_exec $* failed!"
			else
				return 1
			fi
			;;
	esac
}

#
# Stop a daemon. The method used to stop the daemon depends on the arguments:
#
# * Only one argument that does not contain any slashes (/): stop all processes
#   with that process-name.
# * Only one argument that contains slashes: stop all processes that are
#   instances of that executable.
# * More than one argument: stop all processes that are started by the
#   commandline specified by all arguments together.
#
stop_daemon() {
	/sbin/respawn-off "$@" || warning "$0: respawn-off $@ failed!"

	_basename="${1##*/}"
	_basename="${_basename%% *}"

	if [ -f "/var/run/$_basename.pid" ]; then
		start-stop-daemon --stop --pidfile "/var/run/$_basename.pid" \
			&& return 0

		# That didn't work. Remove the pidfile and try other methods.
		rm -f "/var/run/$_basename.pid"
	fi

	if [ "$#" = 1 ]; then
		if [ "$1" = "$_basename" ]; then
			_ssd_options="--name $1"
		else
			_ssd_options="--exec $1"
		fi

		start-stop-daemon --stop --retry 5 $_ssd_options
	else
		_cmdline=
		for _arg in "$@"; do
			_cmdline="$_cmdline$_arg "
		done

		_ret=1
		_retries=0
		while [ "$_retries" -le 1 ]; do
			_pids=`pidof "$_basename"`
			[ -z "$_pids" ] && return 1

			for _pid in $_pids; do
				if [ "`cat /proc/$_pid/cmdline | \
				       tr '\000' ' '`" = "$_cmdline" ]; then
					kill_pid $_pid && _ret=0
				fi
			done

			# Only try again if we failed. Then it might have been
			# a process started in the background that forked and
			# therefore got a new pid between pidof and kill.
			if [ "$_ret" = 0 ]; then
				return 0
			fi

			_retries=$(($_retries + 1))
		done

		return 1
	fi
}

# This will start DHCP for one interface.
# DHCP_CLIENT should have a value for the chosen interface.
start_dhcp() {
	if [ -n "$DHCP_CLIENT" ]; then
		information "starting DHCP client"
		if check_cmd $DHCP_CLIENT; then
			end 1 "$0: DHCP client already running!"
		else
			$DHCP_CLIENT &
			/sbin/respawn-on $DHCP_CLIENT || end $?
		fi
	else
		end 1 "$0: BOOTPROTO is dhcp but " \
		      "DHCP_CLIENT is not defined!"
	fi
}

# This will stop DHCP for one interface.
# DHCP_CLIENT should have a value for the chosen interface.
stop_dhcp() {
	if [ -n "$DHCP_CLIENT" ]; then

	if check_cmd $DHCP_CLIENT; then
	    if stop_daemon $DHCP_CLIENT; then
		information "DHCP client stopped"
	    else
		warning "$0: could not stop DHCP client!"
		return 1
	    fi
	fi

	else
		end 1 "$0: BOOTPROTO is dhcp but " \
		      "DHCP_CLIENT is not defined!"
	fi
}
