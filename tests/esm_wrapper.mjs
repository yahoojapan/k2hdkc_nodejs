/*
 * K2HDKC
 * 
 * Copyright 2017 Yahoo Japan Corporation.
 * 
 * K2HDKC is k2hash based distributed KVS cluster.
 * K2HDKC uses K2HASH, CHMPX, FULLOCK libraries. K2HDKC supports
 * distributed KVS cluster server program and client libraries.
 * 
 * For the full copyright and license information, please view
 * the license file that was distributed with this source code.
 *
 * AUTHOR:   Takeshi Nakatani
 * CREATE:   Fri 9 Nov 2018
 * REVISION:
 */

//--------------------------------------------------------------
// Warning Suppression Wrapper
//--------------------------------------------------------------
// This file is a wrapper that enables or disables the 
// "--experimental-loader warning when running tests as follows:
//
//	$ node --loader esm.mjs --experimental-specifier-resolution=node mocha --extensions ts unit_k2hdkc.ts
//	  (node:XXXX) ExperimentalWarning: `--experimental-loader` may be removed in the future; instead use `register()`:
//
//--------------------------------------------------------------

import { register } from 'node:module';
import { pathToFileURL } from 'node:url';

register(new URL('../node_modules/ts-node/esm.mjs', import.meta.url), pathToFileURL('./'));

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
