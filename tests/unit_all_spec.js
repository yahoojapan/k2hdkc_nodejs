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

var	common			= require('./unit_common');				// Common objects for Chai
var	chai			= common.chai;							// eslint-disable-line no-unused-vars
var	k2hdkc			= common.k2hdkc;						// eslint-disable-line no-unused-vars
var	testdir			= common.testdir;						// eslint-disable-line no-unused-vars
var	assert			= common.assert;						// eslint-disable-line no-unused-vars
var	expect			= common.expect;						// eslint-disable-line no-unused-vars
var	subproc			= require('./unit_run_process');		// Utility objects for sub processes
var	startProcs		= subproc.start;						// eslint-disable-line no-unused-vars
var	stopProcs		= subproc.stop;							// eslint-disable-line no-unused-vars

//--------------------------------------------------------------
// Before in global section
//--------------------------------------------------------------
before(function(done){										// eslint-disable-line no-undef
	// Nothing to do
	startProcs(this, testdir);
	done();
});

//--------------------------------------------------------------
// After in global section
//--------------------------------------------------------------
after(function(done){										// eslint-disable-line no-undef
	// Nothing to do
	stopProcs(this, testdir);
	done();
});

//--------------------------------------------------------------
// BeforeEach in global section
//--------------------------------------------------------------
beforeEach(function(){										// eslint-disable-line no-undef
	// Nothing to do
});

//--------------------------------------------------------------
// AfterEach in global section
//--------------------------------------------------------------
afterEach(function(){										// eslint-disable-line no-undef
	// Nothing to do
});

//--------------------------------------------------------------
// Main describe section
//--------------------------------------------------------------
describe('ALL TEST', function(){							// eslint-disable-line no-undef
	//
	// Before in describe section
	//
	before(function(){										// eslint-disable-line no-undef
		// Nothing to do
	});

	//
	// After in describe section
	//
	after(function(){										// eslint-disable-line no-undef
		// Nothing to do
	});

	//
	// Sub testing scripts
	//
	describe('SUB TEST: K2HDKC', function(){			// eslint-disable-line no-undef
		require('./unit_k2hdkc');
	});
});

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
