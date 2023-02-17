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
# CREATE:   Fri 10 Mar 2017
# REVISION:
#

{
    "variables":    {
        "coverage": "false"
    },
    "targets":      [
        {
            "target_name":  "k2hdkc",
            "sources":      [
                "k2hdkc.cc",
                "k2hdkc_node.cc",
                "k2hdkc_cbs.cc"
            ],
            "cflags":       [
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
                "-Wno-attributes",
                #
                # nodejs/nan#807(https://github.com/nodejs/nan/issues/807#issuecomment-455750192)
                # recommends using the "-Wno-cast-function-type" to silence deprecations warnings
                # that appear with GCC 8.
                #
                "-Wno-cast-function-type"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],
            "libraries":    [
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
