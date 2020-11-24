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
# CREATE:   Fri 10 Mar 2017
# REVISION:
#

{
	"variables":	{
		"coverage":	"false"
	},
	"targets":		[
		{
			"target_name":	"k2hdkc",
			"sources":		[
				"src/k2hdkc.cc",
				"src/k2hdkc_node.cc",
				"src/k2hdkc_cbs.cc"
			],
			"cflags":		[
				#
				# We get too many deprecated message building with nodejs 0.12.x.
				# Those messages are depricated functions/methods, but we do not use those.
				# So that we ignore these deprecated warning here.
				#
				"-Wno-deprecated",
				#
				# For nodejs 9.x/10.x, it puts about MakeCallback / CreateDataProperty / DefineOwnProperty
				#
				"-Wno-deprecated-declarations",
				#
				# For nodejs 12.x/..., it suppress warnings: "'deprecated' attribute directive ignored"
				#
				"-Wno-attributes"
			],
			"include_dirs":	[
				"<!(node -e \"require('nan')\")"
			],
			"libraries":	[
				"-lk2hdkc"
			]
		}
	]
}

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim600: expandtab sw=4 ts=4 fdm=marker
# vim<600: expandtab sw=4 ts=4
#
