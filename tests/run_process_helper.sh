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

##############################################################
# Check pid file and initialize it
#
# $1 :	pid file path
#
initialize_pid_file()
{
	if [ "X$1" = "X" ]; then
		echo -n " [ERROR] there is no parameter."
		return 1
	fi

	if [ -f $1 ]; then
		#
		# get pids from file
		#
		PIDS=`cat $1`

		#
		# HUP
		#
		kill -HUP ${PIDS} > /dev/null 2>&1
		sleep 1

		#
		# force kill if exists yet
		#
		for tgpid in ${PIDS}; do
			ps -p ${tgpid} > /dev/null 2>&1
			if [ $? -eq 0 ]; then
				kill -9 ${tgpid} > /dev/null 2>&1
				sleep 1
			fi
		done

		#
		# recheck pid
		#
		RESULT=""
		ZOMBIE=""
		for tgpid in ${PIDS}; do
			PSRES=`ps -p ${tgpid} > /dev/null 2>&1`
			if [ $? -eq 0 ]; then
				PSRES=`echo ${PSRES} | grep ${tgpid} | grep -v "defunct"`
				if [ "X${PSRES}" != "X" ]; then
					RESULT="${RESULT} ${tgpid}"
				else
					ZOMBIE="${ZOMBIE} ${tgpid}"
				fi
			fi
		done

		if [ "X${RESULT}" != "X" ]; then
			echo -n " [ERROR] could not stop process(${RESULT})"
			return 1
		fi

		if [ "X${ZOMBIE}" != "X" ]; then
			echo -n " [WARNING] could not stop process(${ZOMBIE}) because it was zombie, but we can continue..."
		fi

		#
		# remove pid file
		#
		rm -f $1
	fi

	return 0
}

##############################################################
# Current environment
#
PROGRAM_NAME=`basename $0`
RUNDIR=`pwd`
SELFSCRIPTDIR=`dirname $0`
if [ "X$SELFSCRIPTDIR" = "X" -o "X$SELFSCRIPTDIR" = "X." ]; then
	TMP_BASENAME=`basename $0`
	TMP_FIRSTWORD=`echo $0 | awk -F"/" '{print $1}'`

	if [ "X$TMP_BASENAME" = "X$TMP_FIRSTWORD" ]; then
		# search path
		SELFSCRIPTDIR=`which $0`
		SELFSCRIPTDIR=`dirname $SELFSCRIPTDIR`
	else
		SELFSCRIPTDIR=.
	fi
fi
SELFSCRIPTDIR=`cd -P ${SELFSCRIPTDIR}; pwd`
SRCTOP=`cd -P ${SELFSCRIPTDIR}/..; pwd`
SRCDIR=${SRCTOP}/src
TESTDIR=${SELFSCRIPTDIR}

#
# Pid files
#
CHMPX1_SERVER_PID_FILE=/tmp/test_helper_chmpx_server1.pid
CHMPX2_SERVER_PID_FILE=/tmp/test_helper_chmpx_server2.pid
K2HDKC1_SERVER_PID_FILE=/tmp/test_helper_k2hdkc_server1.pid
K2HDKC2_SERVER_PID_FILE=/tmp/test_helper_k2hdkc_server2.pid
CHMPX_SLAVE_PID_FILE=/tmp/test_helper_chmpx_slave.pid

#
# Log files & debug option for default
#
CHMPX_DBG_OPTION=
CHMPX_SVR1_LOGFILE=/dev/null
CHMPX_SVR2_LOGFILE=/dev/null
CHMPX_SLV_LOGFILE=/dev/null
K2HDKC_DBG_OPTION=
K2HDKC1_LOGFILE=/dev/null
K2HDKC2_LOGFILE=/dev/null

##############################################################
# Script mode
#
# Variables take value as run/stop/no
#
#
CHMPX_SERVER=no
K2HDKC_SERVER=no
CHMPX_SLAVE=no

if [ "X$1" = "X" ]; then
	echo "[ERROR] parameter is not specified."
	echo ""
	echo "Usage: ${PROGRAM_NAME} [ start_all | start_chmpx_server | start_k2hdkc_server | start_chmpx_slave | stop_chmpx_server | stop_k2hdkc_server | stop_chmpx_slave | stop_all ] {-d [ err | wan | info | dump ] }"
	echo ""
	exit 1

elif [ "X$1" = "X-h" -o "X$1" = "X-help" ]; then
	echo "Usage: ${PROGRAM_NAME} [ start_all | start_chmpx_server | start_k2hdkc_server | start_chmpx_slave | stop_chmpx_server | stop_k2hdkc_server | stop_chmpx_slave | stop_all ] {-d [ err | wan | info | dump ] }"
	echo ""
	exit 0
fi

while [ $# -ne 0 ]; do
	if [ "X$1" = "Xstart_all" -o "X$1" = "XSTART_ALL" ]; then
		CHMPX_SERVER=run
		K2HDKC_SERVER=run
		CHMPX_SLAVE=run
	elif [ "X$1" = "Xstart_chmpx_server" -o "X$1" = "XSTART_CHMPX_SERVER" ]; then
		CHMPX_SERVER=run
	elif [ "X$1" = "Xstart_k2hdkc_server" -o "X$1" = "XSTART_K2HDKC_SERVER" ]; then
		K2HDKC_SERVER=run
	elif [ "X$1" = "Xstart_chmpx_slave" -o "X$1" = "XSTART_CHMPX_SLAVE" ]; then
		CHMPX_SLAVE=run
	elif [ "X$1" = "Xstop_chmpx_server" -o "X$1" = "XSTOP_CHMPX_SERVER" ]; then
		CHMPX_SERVER=stop
	elif [ "X$1" = "Xstop_k2hdkc_server" -o "X$1" = "XSTOP_K2HDKC_SERVER" ]; then
		K2HDKC_SERVER=stop
	elif [ "X$1" = "Xstop_chmpx_slave" -o "X$1" = "XSTOP_CHMPX_SLAVE" ]; then
		CHMPX_SLAVE=stop
	elif [ "X$1" = "Xstop_all" -o "X$1" = "XSTOP_ALL" ]; then
		CHMPX_SERVER=stop
		K2HDKC_SERVER=stop
		CHMPX_SLAVE=stop
	elif [ "X$1" = "X-d" -o "X$1" = "X-D" ]; then
		shift
		if [ "X$1" = "Xerr" -o "X$1" = "XERR" ]; then
			CHMPX_DBG_OPTION="-d err"
			K2HDKC_DBG_OPTION="-d err"
		elif [ "X$1" = "Xwan" -o "X$1" = "XWAN" ]; then
			CHMPX_DBG_OPTION="-d wan"
			K2HDKC_DBG_OPTION="-d wan"
		elif [ "X$1" = "Xinfo" -o "X$1" = "XINFO" ]; then
			CHMPX_DBG_OPTION="-d info"
			K2HDKC_DBG_OPTION="-d info"
		elif [ "X$1" = "Xdump" -o "X$1" = "XDUMP" ]; then
			CHMPX_DBG_OPTION="-d dump"
			K2HDKC_DBG_OPTION="-d dump"
		else
			echo "ERROR: Unknown option -d parameter"
			echo ""
			exit 1
		fi
		CHMPX_SVR1_LOGFILE=/tmp/test_helper_chmpx_server1.log
		CHMPX_SVR2_LOGFILE=/tmp/test_helper_chmpx_server2.log
		CHMPX_SLV_LOGFILE=/tmp/test_helper_chmpx_slave.log
		K2HDKC1_LOGFILE=/tmp/test_helper_k2hdkc_server1.log
		K2HDKC2_LOGFILE=/tmp/test_helper_k2hdkc_server2.log
	else
		echo "ERROR: Unknown option $1"
		echo ""
		exit 1
	fi

	shift
done

##############################################################
# Do work
#
cd ${TESTDIR}

#
# Set mqueue size
#
if [ ! -f /proc/sys/fs/mqueue/msg_max ]; then
	echo -n " [ERROR] /proc/sys/fs/mqueue/msg_max is not existed."
	exit 1
fi
CURRENT_MSGMAX=`cat /proc/sys/fs/mqueue/msg_max`
if [ "X${CURRENT_MSGMAX}" = "X" ]; then
	CURRENT_MSGMAX=0
fi
if [ ${CURRENT_MSGMAX} -lt 128 ]; then
	echo 128 | sudo tee /proc/sys/fs/mqueue/msg_max
fi

#
# Start chmpx server
#
if [ "X${CHMPX_SERVER}" = "Xrun" ]; then
	echo -n "    -> Run chmpx two server processes :"

	#
	# Process check - chmpx(1)
	#
	initialize_pid_file ${CHMPX1_SERVER_PID_FILE}
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not stop old chmpx(1) server process."
		exit 1
	fi

	#
	# Process check - chmpx(2)
	#
	initialize_pid_file ${CHMPX2_SERVER_PID_FILE}
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not stop old chmpx(2) server process."
		exit 1
	fi

	#
	# Run chmpx(1) server process
	#
	chmpx -conf ${TESTDIR}/test_server.ini -ctlport 8021 ${CHMPX_DBG_OPTION} > ${CHMPX_SVR1_LOGFILE} 2>&1 &
	CHMPX_SERVER_PID=$!

	#
	# Check process chmpx(1)
	#
	sleep 1
	ps -p ${CHMPX_SERVER_PID} > /dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not run chmpx(1) server process."
		exit 1
	fi

	#
	# set pid(1) to file
	#
	echo ${CHMPX_SERVER_PID} >> ${CHMPX1_SERVER_PID_FILE}
	echo -n " [SUCCEED] chmpx(1) server pid = ${CHMPX_SERVER_PID}"

	#
	# Run chmpx(2) server process
	#
	chmpx -conf ${TESTDIR}/test_server.ini -ctlport 8023 ${CHMPX_DBG_OPTION} > ${CHMPX_SVR2_LOGFILE} 2>&1 &
	CHMPX_SERVER_PID=$!

	#
	# Check process chmpx(2)
	#
	sleep 1
	ps -p ${CHMPX_SERVER_PID} > /dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not run chmpx(2) server process."
		exit 1
	fi

	#
	# set pid(2) to file
	#
	echo ${CHMPX_SERVER_PID} >> ${CHMPX2_SERVER_PID_FILE}
	echo " [SUCCEED] chmpx(2) server pid = ${CHMPX_SERVER_PID}"
fi

#
# Start k2hdkc server
#
if [ "X${K2HDKC_SERVER}" = "Xrun" ]; then
	echo -n "    -> Run k2hdkc two server processes : "

	#
	# Process check - k2hdkc(1)
	#
	initialize_pid_file ${K2HDKC1_SERVER_PID_FILE}
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not stop old k2hdkc(1) server process."
		exit 1
	fi

	#
	# Process check - k2hdkc(2)
	#
	initialize_pid_file ${K2HDKC2_SERVER_PID_FILE}
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not stop old k2hdkc(2) server process."
		exit 1
	fi

	#
	# Run k2hdkc(1) server process
	#
	k2hdkc -conf ${TESTDIR}/test_server.ini -ctlport 8021 ${K2HDKC_DBG_OPTION} > ${K2HDKC1_LOGFILE} 2>&1 &
	K2HDKC_SERVER_PID=$!

	#
	# Check process - k2hdkc(1)
	#
	sleep 1
	ps -p ${K2HDKC_SERVER_PID} > /dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not run k2hdkc(1) server process."
		exit 1
	fi

	#
	# set pid(1) to file
	#
	echo ${K2HDKC_SERVER_PID} >> ${K2HDKC1_SERVER_PID_FILE}
	echo -n " [SUCCEED] k2hdkc(1) server pid = ${K2HDKC_SERVER_PID}"

	#
	# Run k2hdkc(2) server process
	#
	k2hdkc -conf ${TESTDIR}/test_server.ini -ctlport 8023 ${K2HDKC_DBG_OPTION} > ${K2HDKC2_LOGFILE} 2>&1 &
	K2HDKC_SERVER_PID=$!

	#
	# Check process - k2hdkc(2)
	#
	sleep 1
	ps -p ${K2HDKC_SERVER_PID} > /dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not run k2hdkc(2) server process."
		exit 1
	fi

	#
	# set pid(2) to file
	#
	echo ${K2HDKC_SERVER_PID} >> ${K2HDKC2_SERVER_PID_FILE}
	echo " [SUCCEED] k2hdkc(2) server pid = ${K2HDKC_SERVER_PID}"
fi

#
# Start chmpx slave
#
if [ "X${CHMPX_SLAVE}" = "Xrun" ]; then
	echo -n "    -> Run chmpx one slave process : "

	#
	# Process check
	#
	initialize_pid_file ${CHMPX_SLAVE_PID_FILE}
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not stop old chmpx slave process."
		exit 1
	fi

	#
	# Run chmpx slave process
	#
	chmpx -conf ${TESTDIR}/test_slave.ini ${CHMPX_DBG_OPTION} > ${CHMPX_SLV_LOGFILE} 2>&1 &
	CHMPX_SLAVE_PID=$!

	#
	# Check process
	#
	sleep 1
	ps -p ${CHMPX_SLAVE_PID} > /dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not run chmpx slave process."
		exit 1
	fi

	#
	# set pid to file
	#
	echo ${CHMPX_SLAVE_PID} >> ${CHMPX_SLAVE_PID_FILE}
	echo " [SUCCEED] chmpx slave pid = ${CHMPX_SLAVE_PID}"
fi

#
# Stop chmpx slave
#
if [ "X${CHMPX_SLAVE}" = "Xstop" ]; then
	echo -n "    -> Stop chmpx one slave process : "

	#
	# Stop process
	#
	initialize_pid_file ${CHMPX_SLAVE_PID_FILE}
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not stop chmpx slave process."
		exit 1
	fi
	echo " [SUCCEED] stop chmpx slave"
fi

#
# Stop k2hdkc server
#
if [ "X${K2HDKC_SERVER}" = "Xstop" ]; then
	echo -n "    -> Stop k2hdkc two server processes : "

	#
	# Stop process - k2hdkc(1)
	#
	initialize_pid_file ${K2HDKC1_SERVER_PID_FILE}
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not stop k2hdkc(1) server process."
		exit 1
	fi
	echo -n " [SUCCEED] stop stop k2hdkc(1) server process."

	#
	# Stop process - k2hdkc(2)
	#
	initialize_pid_file ${K2HDKC2_SERVER_PID_FILE}
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not stop k2hdkc(2) server process."
		exit 1
	fi
	echo " [SUCCEED] stop stop k2hdkc(2) server process."
fi

#
# Stop chmpx server
#
if [ "X${CHMPX_SERVER}" = "Xstop" ]; then
	echo -n "    -> Stop chmpx two server processes : "

	#
	# Stop process - chmpx(1)
	#
	initialize_pid_file ${CHMPX1_SERVER_PID_FILE}
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not stop chmpx(1) server process."
		exit 1
	fi
	echo -n " [SUCCEED] stop chmpx(1) server"

	#
	# Stop process - chmpx(2)
	#
	initialize_pid_file ${CHMPX2_SERVER_PID_FILE}
	if [ $? -ne 0 ]; then
		echo -n " [ERROR] could not stop chmpx(2) server process."
		exit 1
	fi
	echo " [SUCCEED] stop chmpx(2) server"
fi

exit 0

#
# VIM modelines
#
# vim:set ts=4 fenc=utf-8:
#
