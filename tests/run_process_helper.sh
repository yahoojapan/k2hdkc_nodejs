#!/bin/sh
#
# K2HDKC
#
# Copyright 2017 Yahoo Japan Corporation.
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
TESTS_DIR="${SRCTOP}/tests"

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
		TMP_RUN_KILL=0
		for _one_pid in ${PIDS}; do
			# shellcheck disable=SC2009
			if ( ps -o pid,stat ax 2>/dev/null | grep -v 'PID' | awk '$2~/^[^Z]/ { print $1 }' | grep -q "^${_one_pid}$" || exit 1 && exit 0 ); then
				kill -KILL "${_one_pid}" >/dev/null 2>&1
				TMP_RUN_KILL=1
			fi
		done
		if [ "${TMP_RUN_KILL}" -eq 1 ]; then
			sleep 2
		fi

		#
		# recheck pid
		#
		PROC_NOT_ZOMBIE=""
		PROC_ZOMBIE=""
		for _one_pid in ${PIDS}; do
			# shellcheck disable=SC2009
			if ( ps -o pid,stat ax 2>/dev/null | grep -v 'PID' | awk '$2~/^[^Z]/ { print $1 }' | grep -q "^${_one_pid}$" || exit 1 && exit 0 ); then
				#
				# Found process id(not zombie)
				#
				PROC_NOT_ZOMBIE="${PROC_NOT_ZOMBIE} ${_one_pid}"
			else
				# shellcheck disable=SC2009
				if ( ps -o pid,stat ax 2>/dev/null | grep -v 'PID' | grep -q "^${_one_pid}$" || exit 1 && exit 0 ); then
					#
					# Found process id(zombie)
					#
					PROC_ZOMBIE="${PROC_ZOMBIE} ${_one_pid}"
				else
					#
					# Not found process id
					#
					:
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

	elif echo "$1" | grep -q -i -e "^-h$" -e "^--help$"; then
		PrintUsage "${PRGNAME}"
		exit 0

	elif echo "$1" | grep -q -i "^start_all$"; then
		if [ -n "${CHMPX_SERVER}" ] || [ -n "${K2HDKC_SERVER}" ] || [ -n "${CHMPX_SLAVE}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SERVER="start"
		K2HDKC_SERVER="start"
		CHMPX_SLAVE="start"

	elif echo "$1" | grep -q -i "^start_chmpx_server$"; then
		if [ -n "${CHMPX_SERVER}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SERVER="start"

	elif echo "$1" | grep -q -i "^start_k2hdkc_server$"; then
		if [ -n "${K2HDKC_SERVER}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		K2HDKC_SERVER="start"

	elif echo "$1" | grep -q -i "^start_chmpx_slave$"; then
		if [ -n "${CHMPX_SLAVE}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SLAVE="start"

	elif echo "$1" | grep -q -i "^stop_all$"; then
		if [ -n "${CHMPX_SERVER}" ] || [ -n "${K2HDKC_SERVER}" ] || [ -n "${CHMPX_SLAVE}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SERVER="stop"
		K2HDKC_SERVER="stop"
		CHMPX_SLAVE="stop"

	elif echo "$1" | grep -q -i "^stop_chmpx_server$"; then
		if [ -n "${CHMPX_SERVER}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SERVER="stop"

	elif echo "$1" | grep -q -i "^stop_k2hdkc_server$"; then
		if [ -n "${K2HDKC_SERVER}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		K2HDKC_SERVER="stop"

	elif echo "$1" | grep -q -i "^stop_chmpx_slave$"; then
		if [ -n "${CHMPX_SLAVE}" ]; then
			echo "[ERROR] Options conflicted. Another action option has already been specified."
			exit 1
		fi
		CHMPX_SLAVE="stop"

	elif echo "$1" | grep -q -i -e "^-d$" -e "^--debug$"; then
		if [ -n "${CHMDBGMODE_ENV_VAL}" ] || [ -n "${DKCDBGMODE_ENV_VAL}" ]; then
			echo "[ERROR] Options conflicted. Another debug option has already been specified."
			exit 1
		fi
		shift

		if echo "$1" | grep -q -i -e "^err$" -e "^error$"; then
			CHMDBGMODE_ENV_VAL="ERR"
			DKCDBGMODE_ENV_VAL="ERR"
		elif echo "$1" | grep -q -i -e "^wan$" -e "^warn$" -e "^warning$"; then
			CHMDBGMODE_ENV_VAL="WAN"
			DKCDBGMODE_ENV_VAL="WAN"
		elif echo "$1" | grep -q -i -e "^inf$" -e "^info$" -e "^msg$"; then
			CHMDBGMODE_ENV_VAL="INFO"
			DKCDBGMODE_ENV_VAL="INFO"
		elif echo "$1" | grep -q -i -e "^dmp$" -e "^dump$"; then
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
# Default values from env
#
if [ -z "${CHMDBGMODE_ENV_VAL}" ] && [ -n "${CHMDBGMODE}" ]; then
	if echo "${CHMDBGMODE}" | grep -q -i -e "^err$" -e "^error$"; then
		CHMDBGMODE_ENV_VAL="ERR"
	elif echo "${CHMDBGMODE}" | grep -q -i -e "^wan$" -e "^warn$" -e "^warning$"; then
		CHMDBGMODE_ENV_VAL="WAN"
	elif echo "${CHMDBGMODE}" | grep -q -i -e "^inf$" -e "^info$" -e "^msg$"; then
		CHMDBGMODE_ENV_VAL="INFO"
	elif echo "${CHMDBGMODE}" | grep -q -i -e "^dmp$" -e "^dump$"; then
		CHMDBGMODE_ENV_VAL="DUMP"
	fi
fi
if [ -z "${DKCDBGMODE_ENV_VAL}" ] && [ -n "${DKCDBGMODE}" ]; then
	if echo "${DKCDBGMODE}" | grep -q -i -e "^err$" -e "^error$"; then
		DKCDBGMODE_ENV_VAL="ERR"
	elif echo "${DKCDBGMODE}" | grep -q -i -e "^wan$" -e "^warn$" -e "^warning$"; then
		DKCDBGMODE_ENV_VAL="WAN"
	elif echo "${DKCDBGMODE}" | grep -q -i -e "^inf$" -e "^info$" -e "^msg$"; then
		DKCDBGMODE_ENV_VAL="INFO"
	elif echo "${DKCDBGMODE}" | grep -q -i -e "^dmp$" -e "^dump$"; then
		DKCDBGMODE_ENV_VAL="DUMP"
	fi
fi

if [ -n "${CHMDBGMODE_ENV_VAL}" ]; then
	if [ -z "${CHMPX_SVR1_LOGFILE}" ] || [ "${CHMPX_SVR1_LOGFILE}" = "/dev/null" ]; then
		CHMPX_SVR1_LOGFILE="/tmp/test_helper_chmpx_server1.log"
	fi
	if [ -z "${CHMPX_SVR2_LOGFILE}" ] || [ "${CHMPX_SVR2_LOGFILE}" = "/dev/null" ]; then
		CHMPX_SVR2_LOGFILE="/tmp/test_helper_chmpx_server2.log"
	fi
	if [ -z "${CHMPX_SLV_LOGFILE}" ] || [ "${CHMPX_SLV_LOGFILE}" = "/dev/null" ]; then
		CHMPX_SLV_LOGFILE="/tmp/test_helper_chmpx_slave.log"
	fi
fi

if [ -n "${DKCDBGMODE_ENV_VAL}" ]; then
	if [ -z "${K2HDKC1_LOGFILE}" ] || [ "${K2HDKC1_LOGFILE}" = "/dev/null" ]; then
		K2HDKC1_LOGFILE="/tmp/test_helper_k2hdkc_server1.log"
	fi
	if [ -z "${K2HDKC2_LOGFILE}" ] || [ "${K2HDKC2_LOGFILE}" = "/dev/null" ]; then
		K2HDKC2_LOGFILE="/tmp/test_helper_k2hdkc_server2.log"
	fi
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

#----------------------------------------------------------
# node path(relative path from SRCTOP) for k2hdkc
#----------------------------------------------------------
if [ -n "${NODE_PATH}" ]; then
	K2HDKC_NODE_PATH="${NODE_PATH}:"
fi
K2HDKC_NODE_PATH="${K2HDKC_NODE_PATH}${SRCTOP}/build/Release"

#==========================================================
# Executing(current at TESTDIR)
#==========================================================
cd "${TESTS_DIR}" || exit 1

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
	CHMDBGMODE="${CHMDBGMODE_ENV_VAL}" chmpx -conf "${TESTS_DIR}"/test_server.ini -ctlport 8021 > "${CHMPX_SVR1_LOGFILE}" 2>&1 &
	CHMPX_SERVER_PID=$!
	sleep 1

	#
	# Check process chmpx(1)
	#
	# shellcheck disable=SC2009
	if ! ( ps -o pid,stat ax 2>/dev/null | grep -v 'PID' | awk '$2~/^[^Z]/ { print $1 }' | grep -q "^${CHMPX_SERVER_PID}$" || exit 1 && exit 0 ); then
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
	CHMDBGMODE="${CHMDBGMODE_ENV_VAL}" chmpx -conf "${TESTS_DIR}"/test_server.ini -ctlport 8023 > "${CHMPX_SVR2_LOGFILE}" 2>&1 &
	CHMPX_SERVER_PID=$!
	sleep 1

	#
	# Check process chmpx(2)
	#
	# shellcheck disable=SC2009
	if ! ( ps -o pid,stat ax 2>/dev/null | grep -v 'PID' | awk '$2~/^[^Z]/ { print $1 }' | grep -q "^${CHMPX_SERVER_PID}$" || exit 1 && exit 0 ); then
		printf '[ERROR] could not run chmpx(2) server process. : '
		exit 1
	fi

	#
	# set pid(2) to file
	#
	echo "${CHMPX_SERVER_PID}" >> "${CHMPX2_SERVER_PID_FILE}"

	echo "[SUCCEED] chmpx(2) server pid = ${CHMPX_SERVER_PID}"
fi

#----------------------------------------------------------
# Start k2hdkc server
#----------------------------------------------------------
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
	DKCDBGMODE="${DKCDBGMODE_ENV_VAL}" k2hdkc -conf "${TESTS_DIR}"/test_server.ini -ctlport 8021 > "${K2HDKC1_LOGFILE}" 2>&1 &
	K2HDKC_SERVER_PID=$!
	sleep 1

	#
	# Check process - k2hdkc(1)
	#
	# shellcheck disable=SC2009
	if ! ( ps -o pid,stat ax 2>/dev/null | grep -v 'PID' | awk '$2~/^[^Z]/ { print $1 }' | grep -q "^${K2HDKC_SERVER_PID}$" || exit 1 && exit 0 ); then
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
	DKCDBGMODE="${DKCDBGMODE_ENV_VAL}" k2hdkc -conf "${TESTS_DIR}"/test_server.ini -ctlport 8023 > "${K2HDKC2_LOGFILE}" 2>&1 &
	K2HDKC_SERVER_PID=$!
	sleep 1

	#
	# Check process - k2hdkc(2)
	#
	# shellcheck disable=SC2009
	if ! ( ps -o pid,stat ax 2>/dev/null | grep -v 'PID' | awk '$2~/^[^Z]/ { print $1 }' | grep -q "^${K2HDKC_SERVER_PID}$" || exit 1 && exit 0 ); then
		printf '[ERROR] could not run k2hdkc(2) server process. : '
		exit 1
	fi

	#
	# set pid(2) to file
	#
	echo "${K2HDKC_SERVER_PID}" >> "${K2HDKC2_SERVER_PID_FILE}"

	echo "[SUCCEED] k2hdkc(2) server pid = ${K2HDKC_SERVER_PID}"
fi

#----------------------------------------------------------
# Start chmpx slave
#----------------------------------------------------------
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
	CHMDBGMODE="${CHMDBGMODE_ENV_VAL}" chmpx -conf "${TESTS_DIR}"/test_slave.ini > "${CHMPX_SLV_LOGFILE}" 2>&1 &
	CHMPX_SLAVE_PID=$!
	sleep 1

	#
	# Check process
	#
	# shellcheck disable=SC2009
	if ! ( ps -o pid,stat ax 2>/dev/null | grep -v 'PID' | awk '$2~/^[^Z]/ { print $1 }' | grep -q "^${CHMPX_SLAVE_PID}$" || exit 1 && exit 0 ); then
		printf '[ERROR] could not run chmpx slave process. : '
		exit 1
	fi

	#
	# set pid to file
	#
	echo "${CHMPX_SLAVE_PID}" >> "${CHMPX_SLAVE_PID_FILE}"

	echo "[SUCCEED] chmpx slave pid = ${CHMPX_SLAVE_PID}"
fi

#----------------------------------------------------------
# Stop chmpx slave
#----------------------------------------------------------
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

#----------------------------------------------------------
# Stop k2hdkc server
#----------------------------------------------------------
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

#----------------------------------------------------------
# Stop chmpx server
#----------------------------------------------------------
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
