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

//---------------------------------------------------------
// CommonJS Smoke Test
//---------------------------------------------------------
// [Purpose]
//	Verify that the module can be loaded correctly in a CommonJS
//	environment(require) and that its main exports and basic
//	construction/method calls work(smoke test).
//
// [Outline]
//	- Load the project root package with require('../').
//	- Print the module key( Object.keys(mod) ) and verify that
//	  the expected exports (ex, K2hdkcNode) exist.
//	- If K2hdkcNode is exported, attempt to instantiate it with
//	  new, and if the getQueue method exists, call it and verify
//	  the return value.
//	- Catch exceptions and send them to standard error output
//	  (to avoid crashes).
//
// [Expected]
//		module keys: [ 'K2hdkcNode', 'default' ]
//		K2hdkcNode constructed -> typeof: object
//
// [Meaning of Failure]
//	- require failed
//		Not built / problem with main entry / unable to load
//		native module(.node load error).
//	- exception in new K2hdkcNode()
//		native code initialization error or binding inconsistency.
//	- getQueue not found
//		The name expected by the API is different from the actual
//		implementation.
//---------------------------------------------------------

try{
	const	mod = require('../');		// project root package
	console.log('module keys:', Object.keys(mod));

	if(mod.K2hdkcNode){
		try{
			const	k2hdkcobj = new mod.K2hdkcNode();
			console.log('K2hdkcNode constructed -> typeof:', typeof k2hdkcobj);
		}catch(err2){
			console.error('K2hdkcNode constructor threw:', err2 && err2.message);
			process.exit(1);
		}
	}else{
		console.error('K2hdkcNode not exported by module');
		process.exit(1);
	}
}catch(err1){
	console.error('require failed:', err1 && err1.message);
	process.exit(1);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
