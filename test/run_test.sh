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
# CREATE:   Fri 9 Nov 2018
# REVISION:
#

##############################################################
# Common
#
PROGRAM_NAME=`basename $0`
MYSCRIPTDIR=`dirname $0`
MYSCRIPTDIR=`cd ${MYSCRIPTDIR}; pwd`
SRCTOP=`cd ${MYSCRIPTDIR}/..; pwd`
if [ -f ${SRCTOP}/src/binding.gyp ]; then
	BUILDDIR=${SRCTOP}/src/build/Release
else
	BUILDDIR=${SRCTOP}/build/Release
fi

##############################################################
# Commands
#
COMMANDS="
	all
	k2hdkc
"

CheckCommands()
{
	for command in ${COMMANDS}; do
		if [ "X$1" = "X${command}" ]; then
			echo ${command}
			return
		fi
	done
	echo ""
}

##############################################################
# Parse arguments
#
PrintUsage()
{
	echo "Usage:   $1 [--debuglevel(-d)	DUMP/INFO/WAN/ERR/SILENT(default)] [-logfile(-l) <path>] Command"
	echo ""
	echo "Command: all          - All test"
	echo "         k2hdkc       - k2hdkc test"
	echo ""
}

CMD_PREFIX="unit_"
CMD_SUFFIX="_spec.js"
DEBUG_MODE=""
DEBUG_LOG=""
COMMAND=""

while [ $# -ne 0 ]; do
	if [ "X$1" = "X" ]; then
		break

	elif [ "X$1" = "X--help" -o "X$1" = "X--HELP" -o "X$1" = "X-h" -o "X$1" = "X-H" ]; then
		PrintUsage ${PROGRAM_NAME}
		exit 0

	elif [ "X$1" = "X--debuglevel" -o "X$1" = "X--DEBUGLEVEL" -o "X$1" = "X-d" -o "X$1" = "X-D" ]; then
		#
		# DEBUG option
		#
		shift
		if [ $# -eq 0 ]; then
			echo "ERROR: --debuglevel(-d) option needs parameter(info/warn/err/silent)"
			exit 1
		fi
		if [ "X${DEBUG_MODE}" != "X" ]; then
			echo "ERROR: Already specified --debuglevel(-d) option (${DEBUG_MODE}), thus could not set debug level $1."
			exit 1
		fi

		if [ "X$1" = "Xdump" -o "X$1" = "XDUMP" -o "X$1" = "Xdmp" -o "X$1" = "XDMP" ]; then
			DEBUG_MODE="DUMP"
		elif [ "X$1" = "Xinfo" -o "X$1" = "XINFO" ]; then
			DEBUG_MODE="INFO"
		elif [ "X$1" = "Xwan" -o "X$1" = "XWAN" -o "X$1" = "Xwarn" -o "X$1" = "XWARN" -o "X$1" = "Xwarning" -o "X$1" = "XWARNING" ]; then
			DEBUG_MODE="WAN"
		elif [ "X$1" = "Xerr" -o "X$1" = "XERR" -o "X$1" = "Xerror" -o "X$1" = "XERROR" ]; then
			DEBUG_MODE="ERR"
		elif [ "X$1" = "Xsilent" -o "X$1" = "XSILENT" -o "X$1" = "Xslt" -o "X$1" = "XSLT" ]; then
			DEBUG_MODE="SILENT"
		else
			echo "ERROR: Unknown --debuglevel(-d) option parameter(info/warn/err/silent) : $1"
			exit 1
		fi

	elif [ "X$1" = "X--logfile" -o "X$1" = "X--LOGFILE" -o "X$1" = "X-l" -o "X$1" = "X-L" ]; then
		#
		# LOG FILE option
		#
		shift
		if [ $# -eq 0 ]; then
			echo "ERROR: --logfile(-l) option needs parameter"
			exit 1
		fi
		if [ "X${DEBUG_LOG}" != "X" ]; then
			echo "ERROR: Already specified --logfile(-l) option (${DEBUG_LOG}), thus could not set log file path $1."
			exit 1
		fi
		DEBUG_LOG=$1

	else
		#
		# Run test command
		#
		if [ "X${COMMAND}" != "X" ]; then
			echo "ERROR: Already specified command name(${COMMAND}), could not specify multi command $1"
			exit 1
		fi

		COMMAND=`CheckCommands $1`
		if [ "X${COMMAND}" = "X" ]; then
			echo "ERROR: $1 is not command name"
			exit 1
		fi
	fi

	shift
done

if [ "X${COMMAND}" = "X" ]; then
	COMMAND="all"
fi

##############################################################
# Set default parameter if needs
#
if [ "X${DEBUG_MODE}" = "X" ]; then
	DEBUG_MODE="SILENT"
fi
if [ "X${DEBUG_LOG}" = "X" ]; then
	DEBUG_LOG="/dev/null"
fi

##############################################################
# node path(relative path from SRCTOP) for chmpx
#
if [ "X${NODE_PATH}" != "X" ]; then
	K2HDKC_NODE_PATH=${NODE_PATH}:
fi
K2HDKC_NODE_PATH=${K2HDKC_NODE_PATH}${BUILDDIR}

##############################################################
# Executing(current at SRCTOP)
#
cd ${SRCTOP}
if [ "X${DEBUG_MODE}" = "XINFO" ]; then
	echo "***** RUN *****"
	echo "NODE_PATH=${K2HDKC_NODE_PATH} DKCDBGMODE=${DEBUG_MODE} DKCDBGFILE=${DEBUG_LOG} ${SRCTOP}/node_modules/.bin/mocha ${MYSCRIPTDIR}/${CMD_PREFIX}${COMMAND}${CMD_SUFFIX}"
	echo "***************"
fi
NODE_PATH=${K2HDKC_NODE_PATH} DKCDBGMODE=${DEBUG_MODE} DKCDBGFILE=${DEBUG_LOG} ${SRCTOP}/node_modules/.bin/mocha ${MYSCRIPTDIR}/${CMD_PREFIX}${COMMAND}${CMD_SUFFIX}

exit $?

#
# VIM modelines
#
# vim:set ts=4 fenc=utf-8:
#
