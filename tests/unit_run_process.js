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

'use strict';

//
// Common Start/Stop sub-processes for before/after in mocha
//
var	execSync = require('child_process').execSync;			// For before section to launching sub processes

//
// Before : Start sub processes(server chmpx/slave chmpx/server node) for slave test
//
exports.start = function(parentobj, testdir)
{
	console.log('  START SUB PROCESSES FOR TESTING :');

	//
	// Change timeout for running sub-processes
	//
	var	orgTimeout = parentobj.timeout(30000);

	//
	// Run all sub processes
	//
	var	result = execSync(testdir + '/run_process_helper.sh start_all');
	console.log(String(result).replace(/\r?\n$/g, ''));
	console.log('');

	//
	// Reset timeout
	//
	parentobj.timeout(orgTimeout);
};

//
// After : Stop all sub processes
//
exports.stop = function(parentobj, testdir)
{
	console.log('');
	console.log('  STOP ALL SUB PROCESSES:');

	//
	// Change timeout for running sub-processes
	//
	var	orgTimeout = parentobj.timeout(30000);

	//
	// Stop all sub processes
	//
	var	result = execSync(testdir + '/run_process_helper.sh stop_all');
	console.log(String(result).replace(/\r?\n$/g, ''));
	console.log('');

	//
	// Reset timeout
	//
	parentobj.timeout(orgTimeout);
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
