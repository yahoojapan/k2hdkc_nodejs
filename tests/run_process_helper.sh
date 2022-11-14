#!/bin/sh
#
# K2HDKC
#
# Copyright 2017 Yahoo! JAPAN corporation.
#
# K2HDKC is k2hash based distributed KVS cluster.
# K2HDKC uses K2HASH, CHMPX, FULLOCK libraries. K2HDKC supports
# distributed KVS cluster server program and client libraries.
#
# For the full copyright and license information, please view
# the license file that was distributed with this source code.
#
# AUTHOR:   Takeshi Nakatani
# CREATE:   Wed 15 Mar 2017
# REVISION:
#

#==========================================================
# Common Variables
#==========================================================
PRGNAME=$(basename "$0")
SCRIPTDIR=$(dirname "$0")
SCRIPTDIR=$(cd "${SCRIPTDIR}" || exit 1; pwd)
SRCTOP=$(cd "${SCRIPTDIR}/.." || exit 1; pwd)
TESTSDIR=$(cd "${SRCTOP}/tests" || exit 1; pwd)

#
# Pid files
#
CHMPX1_SERVER_PID_FILE="/tmp/test_helper_chmpx_server1.pid"
CHMPX2_SERVER_PID_FILE="/tmp/test_helper_chmpx_server2.pid"
K2HDKC1_SERVER_PID_FILE="/tmp/test_helper_k2hdkc_server1.pid"
K2HDKC2_SERVER_PID_FILE="/tmp/test_helper_k2hdkc_server2.pid"
CHMPX_SLAVE_PID_FILE="/tmp/test_helper_chmpx_slave.pid"

#==========================================================
# Utilities
#==========================================================
#
# Check pid file and initialize it
#
# $1 :	pid file path
#
initialize_pid_file()
{
	if [ -z "$1" ]; then
		printf '[ERROR] there is no parameter. : '
		return 1
	fi

	if [ -f "$1" ]; then
		#
		# get pids from file
		#
		PIDS="$(tr '\n' ' ' < "$1")"

		#
		# HUP
		#
		/bin/sh -c "kill -HUP ${PIDS}" >/dev/null 2>&1
		sleep 1

		#
		# force kill if exists yet
		#
		for _one_pid in ${PIDS}; do
			if ps -p "${_one_pid}" >/dev/null 2>&1; then
				kill -9 "${_one_pid}" >/dev/null 2>&1
				sleep 1
			fi
		done

		#
		# recheck pid
		#
		PROC_NOT_ZOMBIE=""
		PROC_ZOMBIE=""
		for _one_pid in ${PIDS}; do
			# shellcheck disable=SC2009
			if PSRESULT=$(ps -p "${_one_pid}" 2>&1 | grep -v 'PID'); then
				if echo "${PSRESULT}" | grep "${_one_pid}" | grep -q 'defunct'; then
					PROC_ZOMBIE="${PROC_ZOMBIE} ${_one_pid}"
				else
					PROC_NOT_ZOMBIE="${PROC_NOT_ZOMBIE} ${_one_pid}"
				fi
			fi
		done

		if [ -n "${PROC_NOT_ZOMBIE}" ]; then
			printf '[ERROR] could not stop process(%s) : ' "${PROC_NOT_ZOMBIE}"
			return 1
		fi

		if [ -n "${PROC_ZOMBIE}" ]; then
			printf '[WARNING] could not stop process(%s) because it was zombie, but we can continue... : ' "${PROC_ZOMBIE}"
		fi

		#
		# remove pid file
		#
		rm -f "$1"
	fi
	return 0
}

#==========================================================
# Parse arguments
#==========================================================
PrintUsage()
{
	echo ""
	echo "Usage: $1 [--help(-h)] [--debug(-d) {err | wan | info | dump} ] [ start_all | start_chmpx_server | start_k2hdkc_server | start_chmpx_slave | stop_chmpx_server | stop_k2hdkc_server | stop_chmpx_slave | stop_all ]"
	echo ""
}

#
# option
#
CHMPX_SERVER=""
K2HDKC_SERVER=""
CHMPX_SLAVE=""

#
# Log files & debug option for default
#
CHMDBGMODE_ENV_VAL=""
CHMPX_SVR1_LOGFILE="/dev/null"
CHMPX_SVR2_LOGFILE="/dev/null"
CHMPX_SLV_LOGFILE="/dev/null"
DKCDBGMODE_ENV_VAL=""
K2HDKC1_LOGFILE="/dev/null"
K2HDKC2_LOGFILE="/dev/null"

while [ $# -ne 0 ]; do
	if [ -z "$1" ]; then
		break

	elif [ "$1" = "-h" ] || [ "$1" = "-H" ] || [ "$1" = "--help" ] || [ "$1" = "--HELP" ]; then
		PrintUsage "${PRGNAME}"
		exit 0

	elif [ "$1" = "start_all" ] || [ "$1" = "START_ALL" ]; then
		if [ -n "${CHMPX_SERVER}" ] || [ -n "${K2HDKC_SERVER}" ] || [ -n "${CHMPX_SLAVE}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SERVER="start"
		K2HDKC_SERVER="start"
		CHMPX_SLAVE="start"

	elif [ "$1" = "start_chmpx_server" ] || [ "$1" = "START_CHMPX_SERVER" ]; then
		if [ -n "${CHMPX_SERVER}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SERVER="start"

	elif [ "$1" = "start_k2hdkc_server" ] || [ "$1" = "START_K2HDKC_SERVER" ]; then
		if [ -n "${K2HDKC_SERVER}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		K2HDKC_SERVER="start"

	elif [ "$1" = "start_chmpx_slave" ] || [ "$1" = "START_CHMPX_SLAVE" ]; then
		if [ -n "${CHMPX_SLAVE}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SLAVE="start"

	elif [ "$1" = "stop_all" ] || [ "$1" = "STOP_ALL" ]; then
		if [ -n "${CHMPX_SERVER}" ] || [ -n "${K2HDKC_SERVER}" ] || [ -n "${CHMPX_SLAVE}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SERVER="stop"
		K2HDKC_SERVER="stop"
		CHMPX_SLAVE="stop"

	elif [ "$1" = "stop_chmpx_server" ] || [ "$1" = "STOP_CHMPX_SERVER" ]; then
		if [ -n "${CHMPX_SERVER}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SERVER="stop"

	elif [ "$1" = "stop_k2hdkc_server" ] || [ "$1" = "STOP_K2HDKC_SERVER" ]; then
		if [ -n "${K2HDKC_SERVER}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		K2HDKC_SERVER="stop"

	elif [ "$1" = "stop_chmpx_slave" ] || [ "$1" = "STOP_CHMPX_SLAVE" ]; then
		if [ -n "${CHMPX_SLAVE}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SLAVE="stop"

	elif [ "$1" = "-d" ] || [ "$1" = "-D" ] || [ "$1" = "--debug" ] || [ "$1" = "--DEBUG" ]; then
		if [ -n "${CHMDBGMODE_ENV_VAL}" ] || [ -n "${DKCDBGMODE_ENV_VAL}" ]; then
			echo "[ERROR] Options conflicted. Another debug option has already been specified."
			exit 1
		fi
		shift
		if [ "$1" = "err" ] || [ "$1" = "ERR" ]; then
			CHMDBGMODE_ENV_VAL="ERR"
			DKCDBGMODE_ENV_VAL="ERR"
		elif [ "$1" = "wan" ] || [ "$1" = "WAN" ]; then
			CHMDBGMODE_ENV_VAL="WAN"
			DKCDBGMODE_ENV_VAL="WAN"
		elif [ "$1" = "info" ] || [ "$1" = "INFO" ]; then
			CHMDBGMODE_ENV_VAL="INFO"
			DKCDBGMODE_ENV_VAL="INFO"
		elif [ "$1" = "dump" ] || [ "$1" = "DUMP" ]; then
			CHMDBGMODE_ENV_VAL="DUMP"
			DKCDBGMODE_ENV_VAL="DUMP"
		else
			echo "[ERROR] Unknown --debug(-d) option parameter($1)"
			exit 1
		fi
		CHMPX_SVR1_LOGFILE="/tmp/test_helper_chmpx_server1.log"
		CHMPX_SVR2_LOGFILE="/tmp/test_helper_chmpx_server2.log"
		CHMPX_SLV_LOGFILE="/tmp/test_helper_chmpx_slave.log"
		K2HDKC1_LOGFILE="/tmp/test_helper_k2hdkc_server1.log"
		K2HDKC2_LOGFILE="/tmp/test_helper_k2hdkc_server2.log"

	else
		echo "[ERROR] Unknown option $1"
		exit 1
	fi
	shift
done

#
# Check mode
#
if [ -z "${CHMPX_SERVER}" ] && [ -z "${K2HDKC_SERVER}" ] && [ -z "${CHMPX_SLAVE}" ]; then
	echo "[ERROR] Any action option is not specified, you must specify one or more of options(start_all/start_chmpx_server/start_k2hdkc_server/start_chmpx_slave/stop_all/stop_chmpx_server/stop_k2hdkc_server/stop_chmpx_slave)"
	exit 1
fi

#
# Set default values
#
if [ -z "${CHMDBGMODE_ENV_VAL}" ]; then
	CHMDBGMODE_ENV_VAL="silent"
fi
if [ -z "${DKCDBGMODE_ENV_VAL}" ]; then
	DKCDBGMODE_ENV_VAL="silent"
fi

#==========================================================
# Executing(current at TESTDIR)
#==========================================================
cd "${TESTSDIR}" || exit 1

#
# Set mqueue size
#
if [ ! -f /proc/sys/fs/mqueue/msg_max ]; then
	printf '[ERROR] /proc/sys/fs/mqueue/msg_max is not existed. : '
	exit 1
fi

CURRENT_MSGMAX="$(tr -d '\n' < /proc/sys/fs/mqueue/msg_max)"
if [ -z "${CURRENT_MSGMAX}" ]; then
	CURRENT_MSGMAX=0
fi
if [ "${CURRENT_MSGMAX}" -lt 128 ]; then
	if ! echo "128" | sudo tee /proc/sys/fs/mqueue/msg_max; then
		printf '[WARNING] could not set 128 to /proc/sys/fs/mqueue/msg_max (maybe it read-only file system), but continue... : '
	fi
fi

#----------------------------------------------------------
# Start chmpx server
#----------------------------------------------------------
if [ -n "${CHMPX_SERVER}" ] && [ "${CHMPX_SERVER}" = "start" ]; then
	printf '    -> Run chmpx two server processes : '

	#
	# Process check - chmpx(1)
	#
	if ! initialize_pid_file "${CHMPX1_SERVER_PID_FILE}"; then
		printf '[ERROR] could not stop old chmpx(1) server process. : '
		exit 1
	fi

	#
	# Process check - chmpx(2)
	#
	if ! initialize_pid_file "${CHMPX2_SERVER_PID_FILE}"; then
		printf '[ERROR] could not stop old chmpx(2) server process. : '
		exit 1
	fi

	#
	# Run chmpx(1) server process
	#
	CHMDBGMODE="${CHMDBGMODE_ENV_VAL}" chmpx -conf "${TESTSDIR}"/test_server.ini -ctlport 8021 > "${CHMPX_SVR1_LOGFILE}" 2>&1 &
	CHMPX_SERVER_PID=$!

	#
	# Check process chmpx(1)
	#
	sleep 1
	if ! ps -p "${CHMPX_SERVER_PID}" >/dev/null 2>&1; then
		printf '[ERROR] could not run chmpx(1) server process. : '
		exit 1
	fi

	#
	# set pid(1) to file
	#
	echo "${CHMPX_SERVER_PID}" >> "${CHMPX1_SERVER_PID_FILE}"
	printf '[SUCCEED] chmpx(1) server pid = %s : ' "${CHMPX_SERVER_PID}"

	#
	# Run chmpx(2) server process
	#
	CHMDBGMODE="${CHMDBGMODE_ENV_VAL}" chmpx -conf "${TESTSDIR}"/test_server.ini -ctlport 8023 > "${CHMPX_SVR2_LOGFILE}" 2>&1 &
	CHMPX_SERVER_PID=$!

	#
	# Check process chmpx(2)
	#
	sleep 1
	if ! ps -p "${CHMPX_SERVER_PID}" >/dev/null 2>&1; then
		printf '[ERROR] could not run chmpx(2) server process. : '
		exit 1
	fi

	#
	# set pid(2) to file
	#
	echo "${CHMPX_SERVER_PID}" >> "${CHMPX2_SERVER_PID_FILE}"

	echo "[SUCCEED] chmpx(2) server pid = ${CHMPX_SERVER_PID}"
fi

#
#----------------------------------------------------------
# Start k2hdkc server
#----------------------------------------------------------
#
if [ -n "${K2HDKC_SERVER}" ] && [ "${K2HDKC_SERVER}" = "start" ]; then
	printf '    -> Run k2hdkc two server processes : '

	#
	# Process check - k2hdkc(1)
	#
	if ! initialize_pid_file "${K2HDKC1_SERVER_PID_FILE}"; then
		printf '[ERROR] could not stop old k2hdkc(1) server process. : '
		exit 1
	fi

	#
	# Process check - k2hdkc(2)
	#
	if ! initialize_pid_file "${K2HDKC2_SERVER_PID_FILE}"; then
		printf '[ERROR] could not stop old k2hdkc(2) server process. : '
		exit 1
	fi

	#
	# Run k2hdkc(1) server process
	#
	DKCDBGMODE="${DKCDBGMODE_ENV_VAL}" k2hdkc -conf "${TESTSDIR}"/test_server.ini -ctlport 8021 > "${K2HDKC1_LOGFILE}" 2>&1 &
	K2HDKC_SERVER_PID=$!

	#
	# Check process - k2hdkc(1)
	#
	sleep 1
	if ! ps -p "${K2HDKC_SERVER_PID}" >/dev/null 2>&1; then
		printf '[ERROR] could not run k2hdkc(1) server process. : '
		exit 1
	fi

	#
	# set pid(1) to file
	#
	echo "${K2HDKC_SERVER_PID}" >> "${K2HDKC1_SERVER_PID_FILE}"
	printf '[SUCCEED] k2hdkc(1) server pid = %s : ' "${K2HDKC_SERVER_PID}"

	#
	# Run k2hdkc(2) server process
	#
	DKCDBGMODE="${DKCDBGMODE_ENV_VAL}" k2hdkc -conf "${TESTSDIR}"/test_server.ini -ctlport 8023 > "${K2HDKC2_LOGFILE}" 2>&1 &
	K2HDKC_SERVER_PID=$!

	#
	# Check process - k2hdkc(2)
	#
	sleep 1
	if ! ps -p "${K2HDKC_SERVER_PID}" >/dev/null 2>&1; then
		printf '[ERROR] could not run k2hdkc(2) server process. : '
		exit 1
	fi

	#
	# set pid(2) to file
	#
	echo "${K2HDKC_SERVER_PID}" >> "${K2HDKC2_SERVER_PID_FILE}"

	echo "[SUCCEED] k2hdkc(2) server pid = ${K2HDKC_SERVER_PID}"
fi

#
#----------------------------------------------------------
# Start chmpx slave
#----------------------------------------------------------
#
if [ -n "${CHMPX_SLAVE}" ] && [ "${CHMPX_SLAVE}" = "start" ]; then
	printf '    -> Run chmpx one slave process : '

	#
	# Process check
	#
	if ! initialize_pid_file "${CHMPX_SLAVE_PID_FILE}"; then
		printf '[ERROR] could not stop old chmpx slave process. : '
		exit 1
	fi

	#
	# Run chmpx slave process
	#
	CHMDBGMODE="${CHMDBGMODE_ENV_VAL}" chmpx -conf "${TESTSDIR}"/test_slave.ini > "${CHMPX_SLV_LOGFILE}" 2>&1 &
	CHMPX_SLAVE_PID=$!

	#
	# Check process
	#
	sleep 1
	if ! ps -p "${CHMPX_SLAVE_PID}" >/dev/null 2>&1; then
		printf '[ERROR] could not run chmpx slave process. : '
		exit 1
	fi

	#
	# set pid to file
	#
	echo "${CHMPX_SLAVE_PID}" >> "${CHMPX_SLAVE_PID_FILE}"

	echo "[SUCCEED] chmpx slave pid = ${CHMPX_SLAVE_PID}"
fi

#
#----------------------------------------------------------
# Stop chmpx slave
#----------------------------------------------------------
#
if [ -n "${CHMPX_SLAVE}" ] && [ "${CHMPX_SLAVE}" = "stop" ]; then
	printf '    -> Stop chmpx one slave process : '

	#
	# Stop process
	#
	if ! initialize_pid_file "${CHMPX_SLAVE_PID_FILE}"; then
		printf '[ERROR] could not stop chmpx slave process. : '
		exit 1
	fi
	echo "[SUCCEED] stop chmpx slave"
fi

#
#----------------------------------------------------------
# Stop k2hdkc server
#----------------------------------------------------------
#
if [ -n "${K2HDKC_SERVER}" ] && [ "${K2HDKC_SERVER}" = "stop" ]; then
	printf '    -> Stop k2hdkc two server processes : '

	#
	# Stop process - k2hdkc(1)
	#
	if ! initialize_pid_file "${K2HDKC1_SERVER_PID_FILE}"; then
		printf '[ERROR] could not stop k2hdkc(1) server process. : '
		exit 1
	fi
	printf '[SUCCEED] stop stop k2hdkc(1) server process. : '

	#
	# Stop process - k2hdkc(2)
	#
	if ! initialize_pid_file "${K2HDKC2_SERVER_PID_FILE}"; then
		printf '[ERROR] could not stop k2hdkc(2) server process. : '
		exit 1
	fi
	printf '[SUCCEED] stop stop k2hdkc(2) server process. : '
fi

#
#----------------------------------------------------------
# Stop chmpx server
#----------------------------------------------------------
#
if [ -n "${CHMPX_SERVER}" ] && [ "${CHMPX_SERVER}" = "stop" ]; then
	printf '    -> Stop chmpx two server processes : '

	#
	# Stop process - chmpx(1)
	#
	if ! initialize_pid_file "${CHMPX1_SERVER_PID_FILE}"; then
		printf '[ERROR] could not stop chmpx(1) server process. : '
		exit 1
	fi
	printf '[SUCCEED] stop chmpx(1) server : '

	#
	# Stop process - chmpx(2)
	#
	if ! initialize_pid_file "${CHMPX2_SERVER_PID_FILE}"; then
		printf '[ERROR] could not stop chmpx(2) server process. : '
		exit 1
	fi
	echo "[SUCCEED] stop chmpx(2) server"
fi

exit 0

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim600: noexpandtab sw=4 ts=4 fdm=marker
# vim<600: noexpandtab sw=4 ts=4
#
