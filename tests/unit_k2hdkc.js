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
 * CREATE:   Fri 10 Mar 2017
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
// Main describe section
//--------------------------------------------------------------
describe('K2HDKC', function(){								// eslint-disable-line no-undef
	//
	// Global
	//
	var	slvconf	= testdir + '/test_slave.ini';
	var	slvport	= 8031;

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

	//-------------------------------------------------------------------
	// Test constructor
	//-------------------------------------------------------------------
	//
	// k2hdkc::k2hdkc(param) - No Callback
	//
	it('k2hdkc - k2hash::k2hdkc(param) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		expect(dkcobj.isPermanent()).to.be.a('boolean').to.be.true;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::k2hdkc(new param) - No Callback
	//
	it('k2hdkc - k2hash::k2hdkc(new param) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = new k2hdkc(slvconf, slvport, true, false);

		expect(dkcobj.isPermanent()).to.be.a('boolean').to.be.true;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::k2hdkc(copy param) - No Callback
	//
	it('k2hdkc - k2hash::k2hdkc(copy param) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj1 = new k2hdkc(slvconf, slvport, true, false);
		var	dkcobj2 = dkcobj1;

		expect(dkcobj2.isPermanent()).to.be.a('boolean').to.be.true;
		dkcobj2.clean();
		expect(dkcobj1.isPermanent()).to.be.a('boolean').to.be.false;

		done();
	});

	//-------------------------------------------------------------------
	// Test init
	//-------------------------------------------------------------------
	//
	// k2hdkc::init(no param) - No Callback
	//
	it('k2hdkc - k2hash::init(no param) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		expect(dkcobj.init()).to.be.a('boolean').to.be.true;
		expect(dkcobj.isPermanent()).to.be.a('boolean').to.be.false;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::init(no param) - on Callback
	//
	it('k2hdkc - k2hash::init(no param) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on callback
		expect(dkcobj.on('init', function(error)
		{
			expect(error).to.equal('No object is associated to async worker');
			dkcobj.off('init');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// do
		expect(dkcobj.init()).to.be.a('boolean').to.be.true;
		expect(dkcobj.isPermanent()).to.be.a('boolean').to.be.false;
	});

	//
	// k2hdkc::init(no param) - onXXXX Callback
	//
	it('k2hdkc - k2hash::init(no param) - onXXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on callback
		expect(dkcobj.onInit(function(error)
		{
			expect(error).to.equal('No object is associated to async worker');
			dkcobj.offInit();

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// do
		expect(dkcobj.init()).to.be.a('boolean').to.be.true;
		expect(dkcobj.isPermanent()).to.be.a('boolean').to.be.false;
	});

	//
	// k2hdkc::init(no param) - does not allow inline callback
	//

	//
	// k2hdkc::init(param) - No Callback
	//
	it('k2hdkc - k2hash::init(param) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		expect(dkcobj.init(slvconf, slvport, true, false)).to.be.a('boolean').to.be.true;
		expect(dkcobj.isPermanent()).to.be.a('boolean').to.be.true;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::init(param) - on Callback
	//
	it('k2hdkc - k2hash::init(param) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on callback
		expect(dkcobj.on('init', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('init');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// do
		expect(dkcobj.init(slvconf, slvport, true, false)).to.be.a('boolean').to.be.true;
		expect(dkcobj.isPermanent()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::init(param) - onXXXX Callback
	//
	it('k2hdkc - k2hash::init(param) - onXXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on callback
		expect(dkcobj.onInit(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offInit();

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// do
		expect(dkcobj.init(slvconf, slvport, true, false)).to.be.a('boolean').to.be.true;
		expect(dkcobj.isPermanent()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::init(param) - inline Callback
	//
	it('k2hdkc - k2hash::init(param) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// inline callback
		expect(dkcobj.init(slvconf, slvport, true, false, function(error)
		{
			expect(error).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
		expect(dkcobj.isPermanent()).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test GetValue/SetValue
	//-------------------------------------------------------------------
	//
	// k2hdkc::GetValue/SetValue(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::GetValue/SetValue(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getvalue-onetime-key-001', 'getvalue-onetime-subval-001', 'getvalue-onetime-subkey-001', null, 0)).to.be.a('boolean').to.be.true;

		// get value
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'getvalue-onetime-subkey-001', null, true, null)).to.equal('getvalue-onetime-subval-001');
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'getvalue-onetime-key-001', 'getvalue-onetime-subkey-001', true, null)).to.equal('getvalue-onetime-subval-001');

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::GetValue/SetValue(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::GetValue/SetValue(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on(set) callback
		expect(dkcobj.on('set', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('set');

			// Registered on(get - key) callback
			expect(dkcobj.on('get', function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.equal('getvalue-onetime-subval-002');
				dkcobj.off('get');

				// Registered on(get - subkey) callback
				expect(dkcobj.on('get', function(error, value)
				{
					expect(error).to.be.null;
					expect(value).to.equal('getvalue-onetime-subval-002');
					dkcobj.off('get');

					dkcobj.clean();
					done();
				})).to.be.a('boolean').to.be.true;

				// get value 2
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'getvalue-onetime-key-002', 'getvalue-onetime-subkey-002', true, null)).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// get value 1
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'getvalue-onetime-subkey-002', null, true, null)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// set value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getvalue-onetime-key-002', 'getvalue-onetime-subval-002', 'getvalue-onetime-subkey-002', null, 0)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetValue/SetValue(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::GetValue/SetValue(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered onSet callback
		expect(dkcobj.onSet(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offSet();

			// Registered onGet(key) callback
			expect(dkcobj.onGet(function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.equal('getvalue-onetime-subval-003');
				dkcobj.offGet();

				// Registered onGet(subkey) callback
				expect(dkcobj.onGet(function(error, value)
				{
					expect(error).to.be.null;
					expect(value).to.equal('getvalue-onetime-subval-003');
					dkcobj.offGet();

					dkcobj.clean();
					done();
				})).to.be.a('boolean').to.be.true;

				// get value 2
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'getvalue-onetime-key-003', 'getvalue-onetime-subkey-003', true, null)).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// get value 1
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'getvalue-onetime-subkey-003', null, true, null)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// set value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getvalue-onetime-key-003', 'getvalue-onetime-subval-003', 'getvalue-onetime-subkey-003', null, 0)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetValue/SetValue(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::GetValue/SetValue(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// inline setValue callback
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getvalue-onetime-key-004', 'getvalue-onetime-subval-004', 'getvalue-onetime-subkey-004', null, 0, function(error)
		{
			expect(error).to.be.null;

			// inline getValue(key) callback
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'getvalue-onetime-subkey-004', null, true, null, function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.equal('getvalue-onetime-subval-004');

				// inline getValue(subkey) callback
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'getvalue-onetime-key-004', 'getvalue-onetime-subkey-004', true, null, function(error, value)
				{
					expect(error).to.be.null;
					expect(value).to.equal('getvalue-onetime-subval-004');

					dkcobj.clean();
					done();
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetValue/SetValue(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::GetValue/SetValue(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set value
		expect(dkcobj.setValue('getvalue-permanent-key-001', 'getvalue-permanent-subval-001', 'getvalue-permanent-subkey-001', null, 0)).to.be.a('boolean').to.be.true;

		// get value
		expect(dkcobj.getValue('getvalue-permanent-subkey-001', null, true, null)).to.equal('getvalue-permanent-subval-001');
		expect(dkcobj.getValue('getvalue-permanent-key-001', 'getvalue-permanent-subkey-001', true, null)).to.equal('getvalue-permanent-subval-001');

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::GetValue/SetValue(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::GetValue/SetValue(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered on(set) callback
		expect(dkcobj.on('set', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('set');

			// Registered on(get - key) callback
			expect(dkcobj.on('get', function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.equal('getvalue-permanent-subval-002');
				dkcobj.off('get');

				// Registered on(get - subkey) callback
				expect(dkcobj.on('get', function(error, value)
				{
					expect(error).to.be.null;
					expect(value).to.equal('getvalue-permanent-subval-002');
					dkcobj.off('get');

					dkcobj.clean();
					done();
				})).to.be.a('boolean').to.be.true;

				// get value 2
				expect(dkcobj.getValue('getvalue-permanent-key-002', 'getvalue-permanent-subkey-002', true, null)).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// get value 1
			expect(dkcobj.getValue('getvalue-permanent-subkey-002', null, true, null)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// set value
		expect(dkcobj.setValue('getvalue-permanent-key-002', 'getvalue-permanent-subval-002', 'getvalue-permanent-subkey-002', null, 0)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetValue/SetValue(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::GetValue/SetValue(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered onSet callback
		expect(dkcobj.onSet(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offSet();

			// Registered onGet(key) callback
			expect(dkcobj.onGet(function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.equal('getvalue-permanent-subval-003');
				dkcobj.offGet();

				// Registered onGet(subkey) callback
				expect(dkcobj.onGet(function(error, value)
				{
					expect(error).to.be.null;
					expect(value).to.equal('getvalue-permanent-subval-003');
					dkcobj.offGet();

					dkcobj.clean();
					done();
				})).to.be.a('boolean').to.be.true;

				// get value 2
				expect(dkcobj.getValue('getvalue-permanent-key-003', 'getvalue-permanent-subkey-003', true, null)).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// get value 1
			expect(dkcobj.getValue('getvalue-permanent-subkey-003', null, true, null)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// set value
		expect(dkcobj.setValue('getvalue-permanent-key-003', 'getvalue-permanent-subval-003', 'getvalue-permanent-subkey-003', null, 0)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetValue/SetValue(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::GetValue/SetValue(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// inline setValue callback
		expect(dkcobj.setValue('getvalue-permanent-key-004', 'getvalue-permanent-subval-004', 'getvalue-permanent-subkey-004', null, 0, function(error)
		{
			expect(error).to.be.null;

			// inline getValue(key) callback
			expect(dkcobj.getValue('getvalue-permanent-subkey-004', null, true, null, function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.equal('getvalue-permanent-subval-004');

				// inline getValue(subkey) callback
				expect(dkcobj.getValue('getvalue-permanent-key-004', 'getvalue-permanent-subkey-004', true, null, function(error, value)
				{
					expect(error).to.be.null;
					expect(value).to.equal('getvalue-permanent-subval-004');

					dkcobj.clean();
					done();
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test GetSubkeys/SetSubkeys
	//-------------------------------------------------------------------
	//
	// k2hdkc::GetSubkeys/SetSubkeys(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::GetSubkeys/SetSubkeys(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set key-value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getsubkeys-onetime-subkey-101', 'getsubkeys-onetime-subval-101', null, null, 0)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getsubkeys-onetime-subkey-102', 'getsubkeys-onetime-subval-102', null, null, 0)).to.be.a('boolean').to.be.true;

		// set subkeys[2]
		var	subkeys = ['getsubkeys-onetime-subkey-101', 'getsubkeys-onetime-subkey-102'];
		expect(dkcobj.setSubkeys(slvconf, slvport, true, false, 'getsubkeys-onetime-key-101', subkeys)).to.be.a('boolean').to.be.true;

		// get subkeys
		var	skeylist = dkcobj.getSubkeys(slvconf, slvport, true, false, 'getsubkeys-onetime-key-101', true);
		expect(skeylist instanceof Array, 'k2hdkc::GetSubkeys/SetSubkeys(onetime) - No Callback : getSubkeys() : got not array');
		expect(skeylist.length).to.equal(2);
		expect(skeylist[0]).to.equal('getsubkeys-onetime-subkey-101');
		expect(skeylist[1]).to.equal('getsubkeys-onetime-subkey-102');

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::GetSubkeys/SetSubkeys(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::GetSubkeys/SetSubkeys(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set key-value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getsubkeys-onetime-subkey-111', 'getsubkeys-onetime-subval-111', null, null, 0)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getsubkeys-onetime-subkey-112', 'getsubkeys-onetime-subval-112', null, null, 0)).to.be.a('boolean').to.be.true;

		// Registered on(setSubkeys) callback
		expect(dkcobj.on('setSubkeys', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('setSubkeys');

			// get subkeys
			expect(dkcobj.getSubkeys(slvconf, slvport, true, false, 'getsubkeys-onetime-key-111', true)).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;

		// Registered on(getSubkeys) callback
		expect(dkcobj.on('getSubkeys', function(error, skeylist)
		{
			expect(error).to.be.null;

			// check
			expect(skeylist instanceof Array, 'k2hdkc::GetSubkeys/SetSubkeys(onetime) - on(getSubkeys) Callback : getSubkeys() : got not array');
			expect(skeylist.length).to.equal(2);
			expect(skeylist[0]).to.equal('getsubkeys-onetime-subkey-111');
			expect(skeylist[1]).to.equal('getsubkeys-onetime-subkey-112');

			dkcobj.off('getSubkeys');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set subkeys[2]
		var	subkeys = ['getsubkeys-onetime-subkey-111', 'getsubkeys-onetime-subkey-112'];
		expect(dkcobj.setSubkeys(slvconf, slvport, true, false, 'getsubkeys-onetime-key-111', subkeys)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetSubkeys/SetSubkeys(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::GetSubkeys/SetSubkeys(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set key-value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getsubkeys-onetime-subkey-121', 'getsubkeys-onetime-subval-121', null, null, 0)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getsubkeys-onetime-subkey-122', 'getsubkeys-onetime-subval-122', null, null, 0)).to.be.a('boolean').to.be.true;

		// Registered onSetSubkeys callback
		expect(dkcobj.onSetSubkeys(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offSetSubkeys();

			// get subkeys
			expect(dkcobj.getSubkeys(slvconf, slvport, true, false, 'getsubkeys-onetime-key-121', true)).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;

		// Registered onGetSubkeys callback
		expect(dkcobj.onGetSubkeys(function(error, skeylist)
		{
			expect(error).to.be.null;

			// check
			expect(skeylist instanceof Array, 'k2hdkc::GetSubkeys/SetSubkeys(onetime) - onGetSubkeys Callback : getSubkeys() : got not array');
			expect(skeylist.length).to.equal(2);
			expect(skeylist[0]).to.equal('getsubkeys-onetime-subkey-121');
			expect(skeylist[1]).to.equal('getsubkeys-onetime-subkey-122');

			dkcobj.offGetSubkeys();

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set subkeys[2]
		var	subkeys = ['getsubkeys-onetime-subkey-121', 'getsubkeys-onetime-subkey-122'];
		expect(dkcobj.setSubkeys(slvconf, slvport, true, false, 'getsubkeys-onetime-key-121', subkeys)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetSubkeys/SetSubkeys(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::GetSubkeys/SetSubkeys(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set key-value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getsubkeys-onetime-subkey-131', 'getsubkeys-onetime-subval-131', null, null, 0)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getsubkeys-onetime-subkey-132', 'getsubkeys-onetime-subval-132', null, null, 0)).to.be.a('boolean').to.be.true;

		// set subkeys[2]
		var	subkeys = ['getsubkeys-onetime-subkey-131', 'getsubkeys-onetime-subkey-132'];
		expect(dkcobj.setSubkeys(slvconf, slvport, true, false, 'getsubkeys-onetime-key-131', subkeys, function(error)
		{
			expect(error).to.be.null;

			// get subkeys
			expect(dkcobj.getSubkeys(slvconf, slvport, true, false, 'getsubkeys-onetime-key-131', true, function(error, skeylist)
			{
				expect(error).to.be.null;

				// check
				expect(skeylist instanceof Array, 'k2hdkc::GetSubkeys/SetSubkeys(onetime) - inline(GetSubkeys) Callback : getSubkeys() : got not array');
				expect(skeylist.length).to.equal(2);
				expect(skeylist[0]).to.equal('getsubkeys-onetime-subkey-131');
				expect(skeylist[1]).to.equal('getsubkeys-onetime-subkey-132');

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetSubkeys/SetSubkeys(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::GetSubkeys/SetSubkeys(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key-value
		expect(dkcobj.setValue('getsubkeys-permanent-subkey-151', 'getsubkeys-permanent-subval-151', null, null, 0)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('getsubkeys-permanent-subkey-152', 'getsubkeys-permanent-subval-152', null, null, 0)).to.be.a('boolean').to.be.true;

		// set subkeys[2]
		var	subkeys = ['getsubkeys-permanent-subkey-151', 'getsubkeys-permanent-subkey-152'];
		expect(dkcobj.setSubkeys('getsubkeys-permanent-key-151', subkeys)).to.be.a('boolean').to.be.true;

		// get subkeys
		var	skeylist = dkcobj.getSubkeys('getsubkeys-permanent-key-151', true);
		expect(skeylist instanceof Array, 'k2hdkc::GetSubkeys/SetSubkeys(permanent) - No Callback : getSubkeys() : got not array');
		expect(skeylist.length).to.equal(2);
		expect(skeylist[0]).to.equal('getsubkeys-permanent-subkey-151');
		expect(skeylist[1]).to.equal('getsubkeys-permanent-subkey-152');

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::GetSubkeys/SetSubkeys(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::GetSubkeys/SetSubkeys(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key-value
		expect(dkcobj.setValue('getsubkeys-permanent-subkey-161', 'getsubkeys-permanent-subval-161', null, null, 0)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('getsubkeys-permanent-subkey-162', 'getsubkeys-permanent-subval-162', null, null, 0)).to.be.a('boolean').to.be.true;

		// Registered on(setSubkeys) callback
		expect(dkcobj.on('setSubkeys', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('setSubkeys');

			// get subkeys
			expect(dkcobj.getSubkeys('getsubkeys-permanent-key-161', true)).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;

		// Registered on(getSubkeys) callback
		expect(dkcobj.on('getSubkeys', function(error, skeylist)
		{
			expect(error).to.be.null;

			// check
			expect(skeylist instanceof Array, 'k2hdkc::GetSubkeys/SetSubkeys(permanent) - on(getSubkeys) Callback : getSubkeys() : got not array');
			expect(skeylist.length).to.equal(2);
			expect(skeylist[0]).to.equal('getsubkeys-permanent-subkey-161');
			expect(skeylist[1]).to.equal('getsubkeys-permanent-subkey-162');

			dkcobj.off('getSubkeys');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set subkeys[2]
		var	subkeys = ['getsubkeys-permanent-subkey-161', 'getsubkeys-permanent-subkey-162'];
		expect(dkcobj.setSubkeys('getsubkeys-permanent-key-161', subkeys)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetSubkeys/SetSubkeys(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::GetSubkeys/SetSubkeys(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key-value
		expect(dkcobj.setValue('getsubkeys-permanent-subkey-171', 'getsubkeys-permanent-subval-171', null, null, 0)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('getsubkeys-permanent-subkey-172', 'getsubkeys-permanent-subval-172', null, null, 0)).to.be.a('boolean').to.be.true;

		// Registered onSetSubkeys callback
		expect(dkcobj.onSetSubkeys(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offSetSubkeys();

			// get subkeys
			expect(dkcobj.getSubkeys('getsubkeys-permanent-key-171', true)).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;

		// Registered onGetSubkeys callback
		expect(dkcobj.onGetSubkeys(function(error, skeylist)
		{
			expect(error).to.be.null;

			// check
			expect(skeylist instanceof Array, 'k2hdkc::GetSubkeys/SetSubkeys(permanent) - onGetSubkeys Callback : getSubkeys() : got not array');
			expect(skeylist.length).to.equal(2);
			expect(skeylist[0]).to.equal('getsubkeys-permanent-subkey-171');
			expect(skeylist[1]).to.equal('getsubkeys-permanent-subkey-172');

			dkcobj.offGetSubkeys();

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set subkeys[2]
		var	subkeys = ['getsubkeys-permanent-subkey-171', 'getsubkeys-permanent-subkey-172'];
		expect(dkcobj.setSubkeys('getsubkeys-permanent-key-171', subkeys)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetSubkeys/SetSubkeys(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::GetSubkeys/SetSubkeys(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key-value
		expect(dkcobj.setValue('getsubkeys-permanent-subkey-181', 'getsubkeys-permanent-subval-181', null, null, 0)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('getsubkeys-permanent-subkey-182', 'getsubkeys-permanent-subval-182', null, null, 0)).to.be.a('boolean').to.be.true;

		// set subkeys[2]
		var	subkeys = ['getsubkeys-permanent-subkey-181', 'getsubkeys-permanent-subkey-182'];
		expect(dkcobj.setSubkeys('getsubkeys-permanent-key-181', subkeys, function(error)
		{
			expect(error).to.be.null;

			// get subkeys
			expect(dkcobj.getSubkeys('getsubkeys-permanent-key-181', true, function(error, skeylist)
			{
				expect(error).to.be.null;

				// check
				expect(skeylist instanceof Array, 'k2hdkc::GetSubkeys/SetSubkeys(permanent) - inline(GetSubkeys) Callback : getSubkeys() : got not array');
				expect(skeylist.length).to.equal(2);
				expect(skeylist[0]).to.equal('getsubkeys-permanent-subkey-181');
				expect(skeylist[1]).to.equal('getsubkeys-permanent-subkey-182');

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test GetAttrs
	//-------------------------------------------------------------------
	//
	// k2hdkc::GetAttrs(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::GetAttrs(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set key-value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getattrs-onetime-key-201', 'getattrs-onetime-val-201', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get attributes
		var	attrlist = dkcobj.getAttrs(slvconf, slvport, true, false, 'getattrs-onetime-key-201');
		expect(attrlist instanceof Array, 'k2hdkc::GetAttrs(onetime) - No Callback : getAttrs() : got not array');
		expect(attrlist.length).to.equal(3);
		expect(attrlist[0]).to.equal('aes256_cbc_pad.pbkdf2.sha256');
		expect(attrlist[1]).to.equal('expire');
		expect(attrlist[2]).to.equal('mtime');

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::GetAttrs(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::GetAttrs(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set key-value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getattrs-onetime-key-211', 'getattrs-onetime-val-211', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(getAttrs) callback
		expect(dkcobj.on('getAttrs', function(error, attrlist)
		{
			expect(error).to.be.null;

			// check
			expect(attrlist instanceof Array, 'k2hdkc::GetAttrs(onetime) - on Callback : getAttrs() : got not array');
			expect(attrlist.length).to.equal(3);
			expect(attrlist[0]).to.equal('aes256_cbc_pad.pbkdf2.sha256');
			expect(attrlist[1]).to.equal('expire');
			expect(attrlist[2]).to.equal('mtime');
			dkcobj.off('getAttrs');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// get attributes
		expect(dkcobj.getAttrs(slvconf, slvport, true, false, 'getattrs-onetime-key-211')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetAttrs(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::GetAttrs(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set key-value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getattrs-onetime-key-221', 'getattrs-onetime-val-221', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onGetAttrs callback
		expect(dkcobj.onGetAttrs(function(error, attrlist)
		{
			expect(error).to.be.null;

			// check
			expect(attrlist instanceof Array, 'k2hdkc::GetAttrs(onetime) - onXXX Callback : getAttrs() : got not array');
			expect(attrlist.length).to.equal(3);
			expect(attrlist[0]).to.equal('aes256_cbc_pad.pbkdf2.sha256');
			expect(attrlist[1]).to.equal('expire');
			expect(attrlist[2]).to.equal('mtime');

			dkcobj.offGetAttrs();

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// get attributes
		expect(dkcobj.getAttrs(slvconf, slvport, true, false, 'getattrs-onetime-key-221')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetAttrs(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::GetAttrs(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set key-value
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'getattrs-onetime-key-231', 'getattrs-onetime-val-231', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get attributes
		expect(dkcobj.getAttrs(slvconf, slvport, true, false, 'getattrs-onetime-key-231', function(error, attrlist)
		{
			expect(error).to.be.null;

			// check
			expect(attrlist instanceof Array, 'k2hdkc::GetAttrs(onetime) - inline Callback : getAttrs() : got not array');
			expect(attrlist.length).to.equal(3);
			expect(attrlist[0]).to.equal('aes256_cbc_pad.pbkdf2.sha256');
			expect(attrlist[1]).to.equal('expire');
			expect(attrlist[2]).to.equal('mtime');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetAttrs(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::GetAttrs(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key-value
		expect(dkcobj.setValue('getattrs-permanent-key-251', 'getattrs-permanent-val-251', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get attributes
		var	attrlist = dkcobj.getAttrs('getattrs-permanent-key-251');
		expect(attrlist instanceof Array, 'k2hdkc::GetAttrs(permanent) - No Callback : getAttrs() : got not array');
		expect(attrlist.length).to.equal(3);
		expect(attrlist[0]).to.equal('aes256_cbc_pad.pbkdf2.sha256');
		expect(attrlist[1]).to.equal('expire');
		expect(attrlist[2]).to.equal('mtime');

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::GetAttrs(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::GetAttrs(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key-value
		expect(dkcobj.setValue('getattrs-permanent-key-261', 'getattrs-permanent-val-261', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(getAttrs) callback
		expect(dkcobj.on('getAttrs', function(error, attrlist)
		{
			expect(error).to.be.null;

			// check
			expect(attrlist instanceof Array, 'k2hdkc::GetAttrs(permanent) - on Callback : getAttrs() : got not array');
			expect(attrlist.length).to.equal(3);
			expect(attrlist[0]).to.equal('aes256_cbc_pad.pbkdf2.sha256');
			expect(attrlist[1]).to.equal('expire');
			expect(attrlist[2]).to.equal('mtime');
			dkcobj.off('getAttrs');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// get attributes
		expect(dkcobj.getAttrs('getattrs-permanent-key-261')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetAttrs(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::GetAttrs(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key-value
		expect(dkcobj.setValue('getattrs-permanent-key-271', 'getattrs-permanent-val-271', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onGetAttrs callback
		expect(dkcobj.onGetAttrs(function(error, attrlist)
		{
			expect(error).to.be.null;

			// check
			expect(attrlist instanceof Array, 'k2hdkc::GetAttrs(permanent) - onXXX Callback : getAttrs() : got not array');
			expect(attrlist.length).to.equal(3);
			expect(attrlist[0]).to.equal('aes256_cbc_pad.pbkdf2.sha256');
			expect(attrlist[1]).to.equal('expire');
			expect(attrlist[2]).to.equal('mtime');

			dkcobj.offGetAttrs();

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// get attributes
		expect(dkcobj.getAttrs('getattrs-permanent-key-271')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::GetAttrs(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::GetAttrs(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key-value
		expect(dkcobj.setValue('getattrs-permanent-key-281', 'getattrs-permanent-val-281', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get attributes
		expect(dkcobj.getAttrs('getattrs-permanent-key-281', function(error, attrlist)
		{
			expect(error).to.be.null;

			// check
			expect(attrlist instanceof Array, 'k2hdkc::GetAttrs(permanent) - inline Callback : getAttrs() : got not array');
			expect(attrlist.length).to.equal(3);
			expect(attrlist[0]).to.equal('aes256_cbc_pad.pbkdf2.sha256');
			expect(attrlist[1]).to.equal('expire');
			expect(attrlist[2]).to.equal('mtime');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test SetAll
	//-------------------------------------------------------------------
	//
	// k2hdkc::SetAll(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::SetAll(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set subkey
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'setall-onetime-subkey-301', 'setall-onetime-subval-301', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// set all
		var	subkeys = ['setall-onetime-subkey-301'];
		expect(dkcobj.setAll(slvconf, slvport, true, false, 'setall-onetime-key-301', 'setall-onetime-val-301', subkeys, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get value
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-key-301', null, true, 'mypass')).to.equal('setall-onetime-val-301');
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-subkey-301', null, true, 'mypass')).to.equal('setall-onetime-subval-301');
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-key-301', 'setall-onetime-subkey-301', true, 'mypass')).to.equal('setall-onetime-subval-301');

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::SetAll(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::SetAll(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on(setAll) callback
		expect(dkcobj.on('setAll', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('setAll');

			// get value
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-key-302', null, true, 'mypass')).to.equal('setall-onetime-val-302');
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-subkey-302', null, true, 'mypass')).to.equal('setall-onetime-subval-302');
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-key-302', 'setall-onetime-subkey-302', true, 'mypass')).to.equal('setall-onetime-subval-302');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set subkey
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'setall-onetime-subkey-302', 'setall-onetime-subval-302', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// set all
		var	subkeys = ['setall-onetime-subkey-302'];
		expect(dkcobj.setAll(slvconf, slvport, true, false, 'setall-onetime-key-302', 'setall-onetime-val-302', subkeys, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::SetAll(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::SetAll(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered onSetAll callback
		expect(dkcobj.onSetAll(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offSet();

			// get value
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-key-303', null, true, 'mypass')).to.equal('setall-onetime-val-303');
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-subkey-303', null, true, 'mypass')).to.equal('setall-onetime-subval-303');
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-key-303', 'setall-onetime-subkey-303', true, 'mypass')).to.equal('setall-onetime-subval-303');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set subkey
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'setall-onetime-subkey-303', 'setall-onetime-subval-303', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// set all
		var	subkeys = ['setall-onetime-subkey-303'];
		expect(dkcobj.setAll(slvconf, slvport, true, false, 'setall-onetime-key-303', 'setall-onetime-val-303', subkeys, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::SetAll(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::SetAll(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// set subkey
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'setall-onetime-subkey-304', 'setall-onetime-subval-304', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// inline setAll callback
		var	subkeys = ['setall-onetime-subkey-304'];
		expect(dkcobj.setAll(slvconf, slvport, true, false, 'setall-onetime-key-304', 'setall-onetime-val-304', subkeys, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// get value
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-key-304', null, true, 'mypass')).to.equal('setall-onetime-val-304');
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-subkey-304', null, true, 'mypass')).to.equal('setall-onetime-subval-304');
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'setall-onetime-key-304', 'setall-onetime-subkey-304', true, 'mypass')).to.equal('setall-onetime-subval-304');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::SetAll(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::SetAll(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set subkey
		expect(dkcobj.setValue('setall-permanent-subkey-305', 'setall-permanent-subval-305', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// set all
		var	subkeys = ['setall-permanent-subkey-305'];
		expect(dkcobj.setAll('setall-permanent-key-305', 'setall-permanent-val-305', subkeys, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get value
		expect(dkcobj.getValue('setall-permanent-key-305', null, true, 'mypass')).to.equal('setall-permanent-val-305');
		expect(dkcobj.getValue('setall-permanent-subkey-305', null, true, 'mypass')).to.equal('setall-permanent-subval-305');
		expect(dkcobj.getValue('setall-permanent-key-305', 'setall-permanent-subkey-305', true, 'mypass')).to.equal('setall-permanent-subval-305');

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::SetAll(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::SetAll(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set subkey
		expect(dkcobj.setValue('setall-permanent-subkey-306', 'setall-permanent-subval-306', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(setAll) callback
		expect(dkcobj.on('setAll', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('setAll');

			// get value
			expect(dkcobj.getValue('setall-permanent-key-306', null, true, 'mypass')).to.equal('setall-permanent-val-306');
			expect(dkcobj.getValue('setall-permanent-subkey-306', null, true, 'mypass')).to.equal('setall-permanent-subval-306');
			expect(dkcobj.getValue('setall-permanent-key-306', 'setall-permanent-subkey-306', true, 'mypass')).to.equal('setall-permanent-subval-306');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set all
		var	subkeys = ['setall-permanent-subkey-306'];
		expect(dkcobj.setAll('setall-permanent-key-306', 'setall-permanent-val-306', subkeys, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::SetAll(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::SetAll(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set subkey
		expect(dkcobj.setValue('setall-permanent-subkey-307', 'setall-permanent-subval-307', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onSetAll callback
		expect(dkcobj.onSetAll(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offSetAll();

			// get value
			expect(dkcobj.getValue('setall-permanent-key-307', null, true, 'mypass')).to.equal('setall-permanent-val-307');
			expect(dkcobj.getValue('setall-permanent-subkey-307', null, true, 'mypass')).to.equal('setall-permanent-subval-307');
			expect(dkcobj.getValue('setall-permanent-key-307', 'setall-permanent-subkey-307', true, 'mypass')).to.equal('setall-permanent-subval-307');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set all
		var	subkeys = ['setall-permanent-subkey-307'];
		expect(dkcobj.setAll('setall-permanent-key-307', 'setall-permanent-val-307', subkeys, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::SetAll(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::SetAll(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set subkey
		expect(dkcobj.setValue('setall-permanent-subkey-308', 'setall-permanent-subval-308', null, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// inline setAll callback
		var	subkeys = ['setall-permanent-subkey-308'];
		expect(dkcobj.setAll('setall-permanent-key-308', 'setall-permanent-val-308', subkeys, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// get value
			expect(dkcobj.getValue('setall-permanent-key-308', null, true, 'mypass')).to.equal('setall-permanent-val-308');
			expect(dkcobj.getValue('setall-permanent-subkey-308', null, true, 'mypass')).to.equal('setall-permanent-subval-308');
			expect(dkcobj.getValue('setall-permanent-key-308', 'setall-permanent-subkey-308', true, 'mypass')).to.equal('setall-permanent-subval-308');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test Remove
	//-------------------------------------------------------------------
	//
	// k2hdkc::Remove(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::Remove(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	testobj	= this;
		var orgtime	= testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// set key/subkey
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-401', 'remove-onetime-val-401', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-401', 'remove-onetime-subval-401', 'remove-onetime-subkey-401', 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-402', 'remove-onetime-val-402', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-402', 'remove-onetime-subval-402', 'remove-onetime-subkey-402', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove key/subkey
		expect(dkcobj.remove(slvconf, slvport, true, false, 'remove-onetime-key-401', false)).to.be.a('boolean').to.be.true;
		expect(dkcobj.remove(slvconf, slvport, true, false, 'remove-onetime-key-402', true)).to.be.a('boolean').to.be.true;

		// check
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-key-401', null, true, 'mypass')).to.be.null;
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-subkey-401', null, true, 'mypass')).to.equal('remove-onetime-subval-401');
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-key-402', null, true, 'mypass')).to.be.null;
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-subkey-402', null, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::Remove(onetime) - on Callback2
	//
	it('k2hdkc - k2hdkc::Remove(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime	= testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// set key/subkey
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-411', 'remove-onetime-val-411', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-411', 'remove-onetime-subval-411', 'remove-onetime-subkey-411', 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-412', 'remove-onetime-val-412', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-412', 'remove-onetime-subval-412', 'remove-onetime-subkey-412', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(remove) callback
		expect(dkcobj.on('remove', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('remove');

			// Registered on(remove) callback
			expect(dkcobj.on('remove', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('remove');

				// check
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-key-411', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-subkey-411', null, true, 'mypass')).to.equal('remove-onetime-subval-411');
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-key-412', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-subkey-412', null, true, 'mypass')).to.be.null;

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// remove key
			expect(dkcobj.remove(slvconf, slvport, true, false, 'remove-onetime-key-412', true)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove key
		expect(dkcobj.remove(slvconf, slvport, true, false, 'remove-onetime-key-411', false)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::Remove(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::Remove(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// set key/subkey
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-421', 'remove-onetime-val-421', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-421', 'remove-onetime-subval-421', 'remove-onetime-subkey-421', 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-422', 'remove-onetime-val-422', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-422', 'remove-onetime-subval-422', 'remove-onetime-subkey-422', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onRemove callback
		expect(dkcobj.onRemove(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offRemove();

			// Registered onRemove callback
			expect(dkcobj.onRemove(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offRemove();

				// check
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-key-421', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-subkey-421', null, true, 'mypass')).to.equal('remove-onetime-subval-421');
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-key-422', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-subkey-422', null, true, 'mypass')).to.be.null;

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// remove key
			expect(dkcobj.remove(slvconf, slvport, true, false, 'remove-onetime-key-422', true)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove key
		expect(dkcobj.remove(slvconf, slvport, true, false, 'remove-onetime-key-421', false)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::Remove(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::Remove(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// set key/subkey
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-431', 'remove-onetime-val-431', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-431', 'remove-onetime-subval-431', 'remove-onetime-subkey-431', 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-432', 'remove-onetime-val-432', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'remove-onetime-key-432', 'remove-onetime-subval-432', 'remove-onetime-subkey-432', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove key
		expect(dkcobj.remove(slvconf, slvport, true, false, 'remove-onetime-key-431', false, function(error)
		{
			expect(error).to.be.null;

			// remove key
			expect(dkcobj.remove(slvconf, slvport, true, false, 'remove-onetime-key-432', true, function(error)
			{
				expect(error).to.be.null;

				// check
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-key-431', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-subkey-431', null, true, 'mypass')).to.equal('remove-onetime-subval-431');
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-key-432', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue(slvconf, slvport, true, false, 'remove-onetime-subkey-432', null, true, 'mypass')).to.be.null;

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::Remove(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::Remove(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key/subkey
		expect(dkcobj.setValue('remove-permanent-key-451', 'remove-permanent-val-451', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-451', 'remove-permanent-subval-451', 'remove-permanent-subkey-451', 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-452', 'remove-permanent-val-452', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-452', 'remove-permanent-subval-452', 'remove-permanent-subkey-452', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove key/subkey
		expect(dkcobj.remove('remove-permanent-key-451', false)).to.be.a('boolean').to.be.true;
		expect(dkcobj.remove('remove-permanent-key-452', true)).to.be.a('boolean').to.be.true;

		// check
		expect(dkcobj.getValue('remove-permanent-key-451', null, true, 'mypass')).to.be.null;
		expect(dkcobj.getValue('remove-permanent-subkey-451', null, true, 'mypass')).to.equal('remove-permanent-subval-451');
		expect(dkcobj.getValue('remove-permanent-key-452', null, true, 'mypass')).to.be.null;
		expect(dkcobj.getValue('remove-permanent-subkey-452', null, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::Remove(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::Remove(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key/subkey
		expect(dkcobj.setValue('remove-permanent-key-461', 'remove-permanent-val-461', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-461', 'remove-permanent-subval-461', 'remove-permanent-subkey-461', 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-462', 'remove-permanent-val-462', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-462', 'remove-permanent-subval-462', 'remove-permanent-subkey-462', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(remove) callback
		expect(dkcobj.on('remove', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('remove');

			// Registered on(remove) callback
			expect(dkcobj.on('remove', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('remove');

				// check
				expect(dkcobj.getValue('remove-permanent-key-461', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue('remove-permanent-subkey-461', null, true, 'mypass')).to.equal('remove-permanent-subval-461');
				expect(dkcobj.getValue('remove-permanent-key-462', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue('remove-permanent-subkey-462', null, true, 'mypass')).to.be.null;

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// remove key
			expect(dkcobj.remove('remove-permanent-key-462', true)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove key
		expect(dkcobj.remove('remove-permanent-key-461', false)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::Remove(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::Remove(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key/subkey
		expect(dkcobj.setValue('remove-permanent-key-471', 'remove-permanent-val-471', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-471', 'remove-permanent-subval-471', 'remove-permanent-subkey-471', 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-472', 'remove-permanent-val-472', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-472', 'remove-permanent-subval-472', 'remove-permanent-subkey-472', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onRemove callback
		expect(dkcobj.onRemove(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offRemove();

			// Registered onRemove callback
			expect(dkcobj.onRemove(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offRemove();

				// check
				expect(dkcobj.getValue('remove-permanent-key-471', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue('remove-permanent-subkey-471', null, true, 'mypass')).to.equal('remove-permanent-subval-471');
				expect(dkcobj.getValue('remove-permanent-key-472', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue('remove-permanent-subkey-472', null, true, 'mypass')).to.be.null;

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// remove key
			expect(dkcobj.remove('remove-permanent-key-472', true)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove key
		expect(dkcobj.remove('remove-permanent-key-471', false)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::Remove(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::Remove(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key/subkey
		expect(dkcobj.setValue('remove-permanent-key-481', 'remove-permanent-val-481', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-481', 'remove-permanent-subval-481', 'remove-permanent-subkey-481', 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-482', 'remove-permanent-val-482', null, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.setValue('remove-permanent-key-482', 'remove-permanent-subval-482', 'remove-permanent-subkey-482', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove key
		expect(dkcobj.remove('remove-permanent-key-481', false, function(error)
		{
			expect(error).to.be.null;

			// remove key
			expect(dkcobj.remove('remove-permanent-key-482', true, function(error)
			{
				expect(error).to.be.null;

				// check
				expect(dkcobj.getValue('remove-permanent-key-481', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue('remove-permanent-subkey-481', null, true, 'mypass')).to.equal('remove-permanent-subval-481');
				expect(dkcobj.getValue('remove-permanent-key-482', null, true, 'mypass')).to.be.null;
				expect(dkcobj.getValue('remove-permanent-subkey-482', null, true, 'mypass')).to.be.null;

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test Rename
	//-------------------------------------------------------------------
	//
	// k2hdkc::Rename(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::Rename(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// set key
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'rename-onetime-key-501', 'rename-onetime-subval-501', 'rename-onetime-subkey-501', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// rename key/subkey
		expect(dkcobj.rename(slvconf, slvport, true, false, 'rename-onetime-subkey-501', 'rename-onetime-subkey-502', 'rename-onetime-key-501', true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check
		var	skeylist = dkcobj.getSubkeys(slvconf, slvport, true, false, 'rename-onetime-key-501', true);
		expect(skeylist instanceof Array, 'k2hdkc::Rename(onetime) - No Callback : getSubkeys got not array');
		expect(skeylist.length).to.equal(1);
		expect(skeylist[0]).to.equal('rename-onetime-subkey-502');

		expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-subkey-501', null, true, 'mypass')).to.be.null;
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-key-501', 'rename-onetime-subkey-501', true, 'mypass')).to.be.null;
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-subkey-502', null, true, 'mypass')).to.equal('rename-onetime-subval-501');
		expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-key-501', 'rename-onetime-subkey-502', true, 'mypass')).to.equal('rename-onetime-subval-501');

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::Rename(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::Rename(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// set key
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'rename-onetime-key-511', 'rename-onetime-subval-511', 'rename-onetime-subkey-511', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(rename) callback
		expect(dkcobj.on('rename', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('rename');

			// check
			var	skeylist = dkcobj.getSubkeys(slvconf, slvport, true, false, 'rename-onetime-key-511', true);
			expect(skeylist instanceof Array, 'k2hdkc::Rename(onetime) - on Callback : getSubkeys got not array');
			expect(skeylist.length).to.equal(1);
			expect(skeylist[0]).to.equal('rename-onetime-subkey-512');

			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-subkey-511', null, true, 'mypass')).to.be.null;
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-key-511', 'rename-onetime-subkey-511', true, 'mypass')).to.be.null;
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-subkey-512', null, true, 'mypass')).to.equal('rename-onetime-subval-511');
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-key-511', 'rename-onetime-subkey-512', true, 'mypass')).to.equal('rename-onetime-subval-511');

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// rename key/subkey
		expect(dkcobj.rename(slvconf, slvport, true, false, 'rename-onetime-subkey-511', 'rename-onetime-subkey-512', 'rename-onetime-key-511', true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::Rename(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::Rename(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// set key
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'rename-onetime-key-521', 'rename-onetime-subval-521', 'rename-onetime-subkey-521', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onRename callback
		expect(dkcobj.onRename(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offRename();

			// check
			var	skeylist = dkcobj.getSubkeys(slvconf, slvport, true, false, 'rename-onetime-key-521', true);
			expect(skeylist instanceof Array, 'k2hdkc::Rename(onetime) - onXXX Callback : getSubkeys got not array');
			expect(skeylist.length).to.equal(1);
			expect(skeylist[0]).to.equal('rename-onetime-subkey-522');

			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-subkey-521', null, true, 'mypass')).to.be.null;
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-key-521', 'rename-onetime-subkey-521', true, 'mypass')).to.be.null;
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-subkey-522', null, true, 'mypass')).to.equal('rename-onetime-subval-521');
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-key-521', 'rename-onetime-subkey-522', true, 'mypass')).to.equal('rename-onetime-subval-521');

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// rename key/subkey
		expect(dkcobj.rename(slvconf, slvport, true, false, 'rename-onetime-subkey-521', 'rename-onetime-subkey-522', 'rename-onetime-key-521', true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::Rename(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::Rename(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// set key
		expect(dkcobj.setValue(slvconf, slvport, true, false, 'rename-onetime-key-531', 'rename-onetime-subval-531', 'rename-onetime-subkey-531', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// inline callback
		expect(dkcobj.rename(slvconf, slvport, true, false, 'rename-onetime-subkey-531', 'rename-onetime-subkey-532', 'rename-onetime-key-531', true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check
			var	skeylist = dkcobj.getSubkeys(slvconf, slvport, true, false, 'rename-onetime-key-531', true);
			expect(skeylist instanceof Array, 'k2hdkc::Rename(onetime) - inline Callback : getSubkeys got not array');
			expect(skeylist.length).to.equal(1);
			expect(skeylist[0]).to.equal('rename-onetime-subkey-532');

			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-subkey-531', null, true, 'mypass')).to.be.null;
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-key-531', 'rename-onetime-subkey-531', true, 'mypass')).to.be.null;
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-subkey-532', null, true, 'mypass')).to.equal('rename-onetime-subval-531');
			expect(dkcobj.getValue(slvconf, slvport, true, false, 'rename-onetime-key-531', 'rename-onetime-subkey-532', true, 'mypass')).to.equal('rename-onetime-subval-531');

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::Rename(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::Rename(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key
		expect(dkcobj.setValue('rename-permanent-key-551', 'rename-permanent-subval-551', 'rename-permanent-subkey-551', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// rename key/subkey
		expect(dkcobj.rename('rename-permanent-subkey-551', 'rename-permanent-subkey-552', 'rename-permanent-key-551', true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check
		var	skeylist = dkcobj.getSubkeys('rename-permanent-key-551', true);
		expect(skeylist instanceof Array, 'k2hdkc::Rename(permanent) - No Callback : getSubkeys got not array');
		expect(skeylist.length).to.equal(1);
		expect(skeylist[0]).to.equal('rename-permanent-subkey-552');

		expect(dkcobj.getValue('rename-permanent-subkey-551', null, true, 'mypass')).to.be.null;
		expect(dkcobj.getValue('rename-permanent-key-551', 'rename-permanent-subkey-551', true, 'mypass')).to.be.null;
		expect(dkcobj.getValue('rename-permanent-subkey-552', null, true, 'mypass')).to.equal('rename-permanent-subval-551');
		expect(dkcobj.getValue('rename-permanent-key-551', 'rename-permanent-subkey-552', true, 'mypass')).to.equal('rename-permanent-subval-551');

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::Rename(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::Rename(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key
		expect(dkcobj.setValue('rename-permanent-key-561', 'rename-permanent-subval-561', 'rename-permanent-subkey-561', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(rename) callback
		expect(dkcobj.on('rename', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('rename');

			// check
			var	skeylist = dkcobj.getSubkeys('rename-permanent-key-561', true);
			expect(skeylist instanceof Array, 'k2hdkc::Rename(permanent) - on Callback : getSubkeys got not array');
			expect(skeylist.length).to.equal(1);
			expect(skeylist[0]).to.equal('rename-permanent-subkey-562');

			expect(dkcobj.getValue('rename-permanent-subkey-561', null, true, 'mypass')).to.be.null;
			expect(dkcobj.getValue('rename-permanent-key-561', 'rename-permanent-subkey-561', true, 'mypass')).to.be.null;
			expect(dkcobj.getValue('rename-permanent-subkey-562', null, true, 'mypass')).to.equal('rename-permanent-subval-561');
			expect(dkcobj.getValue('rename-permanent-key-561', 'rename-permanent-subkey-562', true, 'mypass')).to.equal('rename-permanent-subval-561');

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// rename key/subkey
		expect(dkcobj.rename('rename-permanent-subkey-561', 'rename-permanent-subkey-562', 'rename-permanent-key-561', true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::Rename(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::Rename(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key
		expect(dkcobj.setValue('rename-permanent-key-571', 'rename-permanent-subval-571', 'rename-permanent-subkey-571', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onRename callback
		expect(dkcobj.onRename(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offRename();

			// check
			var	skeylist = dkcobj.getSubkeys('rename-permanent-key-571', true);
			expect(skeylist instanceof Array, 'k2hdkc::Rename(permanent) - onXXX Callback : getSubkeys got not array');
			expect(skeylist.length).to.equal(1);
			expect(skeylist[0]).to.equal('rename-permanent-subkey-572');

			expect(dkcobj.getValue('rename-permanent-subkey-571', null, true, 'mypass')).to.be.null;
			expect(dkcobj.getValue('rename-permanent-key-571', 'rename-permanent-subkey-571', true, 'mypass')).to.be.null;
			expect(dkcobj.getValue('rename-permanent-subkey-572', null, true, 'mypass')).to.equal('rename-permanent-subval-571');
			expect(dkcobj.getValue('rename-permanent-key-571', 'rename-permanent-subkey-572', true, 'mypass')).to.equal('rename-permanent-subval-571');

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// rename key/subkey
		expect(dkcobj.rename('rename-permanent-subkey-571', 'rename-permanent-subkey-572', 'rename-permanent-key-571', true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::Rename(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::Rename(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// set key
		expect(dkcobj.setValue('rename-permanent-key-581', 'rename-permanent-subval-581', 'rename-permanent-subkey-581', 'mypass', 600)).to.be.a('boolean').to.be.true;

		// inline callback
		expect(dkcobj.rename('rename-permanent-subkey-581', 'rename-permanent-subkey-582', 'rename-permanent-key-581', true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check
			var	skeylist = dkcobj.getSubkeys('rename-permanent-key-581', true);
			expect(skeylist instanceof Array, 'k2hdkc::Rename(permanent) - inline Callback : getSubkeys got not array');
			expect(skeylist.length).to.equal(1);
			expect(skeylist[0]).to.equal('rename-permanent-subkey-582');

			expect(dkcobj.getValue('rename-permanent-subkey-581', null, true, 'mypass')).to.be.null;
			expect(dkcobj.getValue('rename-permanent-key-581', 'rename-permanent-subkey-581', true, 'mypass')).to.be.null;
			expect(dkcobj.getValue('rename-permanent-subkey-582', null, true, 'mypass')).to.equal('rename-permanent-subval-581');
			expect(dkcobj.getValue('rename-permanent-key-581', 'rename-permanent-subkey-582', true, 'mypass')).to.equal('rename-permanent-subval-581');

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test FIFO-QueuePush
	//-------------------------------------------------------------------
	//
	// k2hdkc::FIFO-QueuePush(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePush(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-601-', null, 'queuepush-onetime-value-601-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-601-', true, false, 'mypass')).to.equal('queuepush-onetime-value-601-fifo');
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-601-', true, false, 'mypass')).to.be.null;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::FIFO-QueuePush(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePush(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on(queuePush) callback
		expect(dkcobj.on('queuePush', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queuePush');

			// check(pop)
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-602-', true, false, 'mypass')).to.equal('queuepush-onetime-value-602-fifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-602-', true, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-602-', null, 'queuepush-onetime-value-602-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueuePush(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePush(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on(queuePush) callback
		expect(dkcobj.onQueuePush(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueuePush();

			// check(pop)
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-603-', true, false, 'mypass')).to.equal('queuepush-onetime-value-603-fifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-603-', true, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-603-', null, 'queuepush-onetime-value-603-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueuePush(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePush(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-604-', null, 'queuepush-onetime-value-604-fifo', true, true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check(pop)
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-604-', true, false, 'mypass')).to.equal('queuepush-onetime-value-604-fifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-604-', true, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueuePush(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePush(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepush-permanent-prefix-605-', null, 'queuepush-permanent-value-605-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		expect(dkcobj.queuePop('queuepush-permanent-prefix-605-', true, false, 'mypass')).to.equal('queuepush-permanent-value-605-fifo');
		expect(dkcobj.queuePop('queuepush-permanent-prefix-605-', true, false, 'mypass')).to.be.null;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::FIFO-QueuePush(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePush(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered on(queuePush) callback
		expect(dkcobj.on('queuePush', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queuePush');

			// check(pop)
			expect(dkcobj.queuePop('queuepush-permanent-prefix-606-', true, false, 'mypass')).to.equal('queuepush-permanent-value-606-fifo');
			expect(dkcobj.queuePop('queuepush-permanent-prefix-606-', true, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush('queuepush-permanent-prefix-606-', null, 'queuepush-permanent-value-606-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueuePush(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePush(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered on(queuePush) callback
		expect(dkcobj.onQueuePush(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueuePush();

			// check(pop)
			expect(dkcobj.queuePop('queuepush-permanent-prefix-607-', true, false, 'mypass')).to.equal('queuepush-permanent-value-607-fifo');
			expect(dkcobj.queuePop('queuepush-permanent-prefix-607-', true, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush('queuepush-permanent-prefix-607-', null, 'queuepush-permanent-value-607-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueuePush(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePush(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepush-permanent-prefix-608-', null, 'queuepush-permanent-value-608-fifo', true, true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check(pop)
			expect(dkcobj.queuePop('queuepush-permanent-prefix-608-', true, false, 'mypass')).to.equal('queuepush-permanent-value-608-fifo');
			expect(dkcobj.queuePop('queuepush-permanent-prefix-608-', true, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test LIFO-QueuePush
	//-------------------------------------------------------------------
	//
	// k2hdkc::LIFO-QueuePush(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePush(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-611-', null, 'queuepush-onetime-value-611-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-611-', false, false, 'mypass')).to.equal('queuepush-onetime-value-611-lifo');
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-611-', false, false, 'mypass')).to.be.null;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::LIFO-QueuePush(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePush(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on(queuePush) callback
		expect(dkcobj.on('queuePush', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queuePush');

			// check(pop)
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-612-', false, false, 'mypass')).to.equal('queuepush-onetime-value-612-lifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-612-', false, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-612-', null, 'queuepush-onetime-value-612-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueuePush(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePush(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on(queuePush) callback
		expect(dkcobj.onQueuePush(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueuePush();

			// check(pop)
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-613-', false, false, 'mypass')).to.equal('queuepush-onetime-value-613-lifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-613-', false, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-613-', null, 'queuepush-onetime-value-613-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueuePush(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePush(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-614-', null, 'queuepush-onetime-value-614-lifo', false, true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check(pop)
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-614-', false, false, 'mypass')).to.equal('queuepush-onetime-value-614-lifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-614-', false, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueuePush(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePush(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepush-permanent-prefix-615-', null, 'queuepush-permanent-value-615-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		expect(dkcobj.queuePop('queuepush-permanent-prefix-615-', false, false, 'mypass')).to.equal('queuepush-permanent-value-615-lifo');
		expect(dkcobj.queuePop('queuepush-permanent-prefix-615-', false, false, 'mypass')).to.be.null;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::LIFO-QueuePush(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePush(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered on(queuePush) callback
		expect(dkcobj.on('queuePush', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queuePush');

			// check(pop)
			expect(dkcobj.queuePop('queuepush-permanent-prefix-616-', false, false, 'mypass')).to.equal('queuepush-permanent-value-616-lifo');
			expect(dkcobj.queuePop('queuepush-permanent-prefix-616-', false, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush('queuepush-permanent-prefix-616-', null, 'queuepush-permanent-value-616-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueuePush(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePush(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered on(queuePush) callback
		expect(dkcobj.onQueuePush(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueuePush();

			// check(pop)
			expect(dkcobj.queuePop('queuepush-permanent-prefix-617-', false, false, 'mypass')).to.equal('queuepush-permanent-value-617-lifo');
			expect(dkcobj.queuePop('queuepush-permanent-prefix-617-', false, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush('queuepush-permanent-prefix-617-', null, 'queuepush-permanent-value-617-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueuePush(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePush(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepush-permanent-prefix-618-', null, 'queuepush-permanent-value-618-lifo', false, true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check(pop)
			expect(dkcobj.queuePop('queuepush-permanent-prefix-618-', false, false, 'mypass')).to.equal('queuepush-permanent-value-618-lifo');
			expect(dkcobj.queuePop('queuepush-permanent-prefix-618-', false, false, 'mypass')).to.be.null;

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test FIFO-QueuePop
	//-------------------------------------------------------------------
	//
	// k2hdkc::FIFO-QueuePop(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePop(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepop-onetime-prefix-621-', null, 'queuepop-onetime-value-621-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-621-', true, false, 'mypass')).to.equal('queuepop-onetime-value-621-fifo');
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-621-', true, false, 'mypass')).to.be.null;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::FIFO-QueuePop(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePop(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepop-onetime-prefix-622-', null, 'queuepop-onetime-value-622-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queuePop) callback
		expect(dkcobj.on('queuePop', function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-onetime-value-622-fifo');
			dkcobj.off('queuePop');

			// Registered on(queuePop) callback
			expect(dkcobj.on('queuePop', function(error, value)
			{
				expect(error).to.be.null;

				expect(value).to.be.null;
				dkcobj.off('queuePop');

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-622-', true, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-622-', true, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueuePop(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePop(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepop-onetime-prefix-623-', null, 'queuepop-onetime-value-623-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueuePop callback
		expect(dkcobj.onQueuePop(function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-onetime-value-623-fifo');
			dkcobj.offQueuePop();

			// Registered onQueuePop callback
			expect(dkcobj.onQueuePop(function(error, value)
			{
				expect(error).to.be.null;

				expect(value).to.be.null;
				dkcobj.offQueuePop();

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-623-', true, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-623-', true, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueuePop(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePop(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepop-onetime-prefix-624-', null, 'queuepop-onetime-value-624-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop callback
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-624-', true, false, 'mypass', function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-onetime-value-624-fifo');

			// Registered onQueuePop callback
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-624-', true, false, 'mypass', function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.be.null;

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueuePop(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePop(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepop-permanent-prefix-625-', null, 'queuepop-permanent-value-625-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('queuepop-permanent-prefix-625-', true, false, 'mypass')).to.equal('queuepop-permanent-value-625-fifo');
		expect(dkcobj.queuePop('queuepop-permanent-prefix-625-', true, false, 'mypass')).to.be.null;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::FIFO-QueuePop(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePop(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepop-permanent-prefix-626-', null, 'queuepop-permanent-value-626-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queuePop) callback
		expect(dkcobj.on('queuePop', function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-permanent-value-626-fifo');
			dkcobj.off('queuePop');

			// Registered on(queuePop) callback
			expect(dkcobj.on('queuePop', function(error, value)
			{
				expect(error).to.be.null;

				expect(value).to.be.null;
				dkcobj.off('queuePop');

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop('queuepop-permanent-prefix-626-', true, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('queuepop-permanent-prefix-626-', true, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueuePop(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePop(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepop-permanent-prefix-627-', null, 'queuepop-permanent-value-627-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueuePop callback
		expect(dkcobj.onQueuePop(function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-permanent-value-627-fifo');
			dkcobj.offQueuePop();

			// Registered onQueuePop callback
			expect(dkcobj.onQueuePop(function(error, value)
			{
				expect(error).to.be.null;

				expect(value).to.be.null;
				dkcobj.offQueuePop();

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop('queuepop-permanent-prefix-627-', true, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('queuepop-permanent-prefix-627-', true, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueuePop(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueuePop(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepop-permanent-prefix-628-', null, 'queuepop-permanent-value-628-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop callback
		expect(dkcobj.queuePop('queuepop-permanent-prefix-628-', true, false, 'mypass', function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-permanent-value-628-fifo');

			// Registered onQueuePop callback
			expect(dkcobj.queuePop('queuepop-permanent-prefix-628-', true, false, 'mypass', function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.be.null;

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test LIFO-QueuePop
	//-------------------------------------------------------------------
	//
	// k2hdkc::LIFO-QueuePop(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePop(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepop-onetime-prefix-631-', null, 'queuepop-onetime-value-631-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-631-', false, false, 'mypass')).to.equal('queuepop-onetime-value-631-lifo');
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-631-', false, false, 'mypass')).to.be.null;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::LIFO-QueuePop(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePop(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepop-onetime-prefix-632-', null, 'queuepop-onetime-value-632-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queuePop) callback
		expect(dkcobj.on('queuePop', function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-onetime-value-632-lifo');
			dkcobj.off('queuePop');

			// Registered on(queuePop) callback
			expect(dkcobj.on('queuePop', function(error, value)
			{
				expect(error).to.be.null;

				expect(value).to.be.null;
				dkcobj.off('queuePop');

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-632-', false, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-632-', false, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueuePop(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePop(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepop-onetime-prefix-633-', null, 'queuepop-onetime-value-633-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueuePop callback
		expect(dkcobj.onQueuePop(function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-onetime-value-633-lifo');
			dkcobj.offQueuePop();

			// Registered onQueuePop callback
			expect(dkcobj.onQueuePop(function(error, value)
			{
				expect(error).to.be.null;

				expect(value).to.be.null;
				dkcobj.offQueuePop();

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-633-', false, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-633-', false, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueuePop(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePop(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepop-onetime-prefix-634-', null, 'queuepop-onetime-value-634-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop callback
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-634-', false, false, 'mypass', function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-onetime-value-634-lifo');

			// Registered onQueuePop callback
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepop-onetime-prefix-634-', false, false, 'mypass', function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.be.null;

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueuePop(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePop(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepop-permanent-prefix-635-', null, 'queuepop-permanent-value-635-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('queuepop-permanent-prefix-635-', false, false, 'mypass')).to.equal('queuepop-permanent-value-635-lifo');
		expect(dkcobj.queuePop('queuepop-permanent-prefix-635-', false, false, 'mypass')).to.be.null;

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::LIFO-QueuePop(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePop(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepop-permanent-prefix-636-', null, 'queuepop-permanent-value-636-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queuePop) callback
		expect(dkcobj.on('queuePop', function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-permanent-value-636-lifo');
			dkcobj.off('queuePop');

			// Registered on(queuePop) callback
			expect(dkcobj.on('queuePop', function(error, value)
			{
				expect(error).to.be.null;

				expect(value).to.be.null;
				dkcobj.off('queuePop');

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop('queuepop-permanent-prefix-636-', false, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('queuepop-permanent-prefix-636-', false, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueuePop(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePop(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepop-permanent-prefix-637-', null, 'queuepop-permanent-value-637-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueuePop callback
		expect(dkcobj.onQueuePop(function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-permanent-value-637-lifo');
			dkcobj.offQueuePop();

			// Registered onQueuePop callback
			expect(dkcobj.onQueuePop(function(error, value)
			{
				expect(error).to.be.null;

				expect(value).to.be.null;
				dkcobj.offQueuePop();

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop('queuepop-permanent-prefix-637-', false, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('queuepop-permanent-prefix-637-', false, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueuePop(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueuePop(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queuepop-permanent-prefix-638-', null, 'queuepop-permanent-value-638-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop callback
		expect(dkcobj.queuePop('queuepop-permanent-prefix-638-', false, false, 'mypass', function(error, value)
		{
			expect(error).to.be.null;

			expect(value).to.equal('queuepop-permanent-value-638-lifo');

			// Registered onQueuePop callback
			expect(dkcobj.queuePop('queuepop-permanent-prefix-638-', false, false, 'mypass', function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.be.null;

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test FIFO-QueueRemove
	//-------------------------------------------------------------------
	//
	// k2hdkc::FIFO-QueueRemove(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueueRemove(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-641-', null, 'queueremove-onetime-value-641-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-641-', null, 'queueremove-onetime-value-641-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-641-', null, 'queueremove-onetime-value-641-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-641-', null, 'queueremove-onetime-value-641-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-641-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-641-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-641-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.false;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-641-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.false;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::FIFO-QueueRemove(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueueRemove(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-642-', null, 'queueremove-onetime-value-642-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-642-', null, 'queueremove-onetime-value-642-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-642-', null, 'queueremove-onetime-value-642-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-642-', null, 'queueremove-onetime-value-642-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queueRemove) callback
		expect(dkcobj.on('queueRemove', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queueRemove');

			// Registered on(queueRemove) callback
			expect(dkcobj.on('queueRemove', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('queueRemove');

				// Registered on(queueRemove) callback
				expect(dkcobj.on('queueRemove', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.off('queueRemove');

					// Registered on(queueRemove) callback
					expect(dkcobj.on('queueRemove', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.off('queueRemove');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-642-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-642-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-642-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-642-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueueRemove(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueueRemove(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-643-', null, 'queueremove-onetime-value-643-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-643-', null, 'queueremove-onetime-value-643-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-643-', null, 'queueremove-onetime-value-643-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-643-', null, 'queueremove-onetime-value-643-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueueRemove callback
		expect(dkcobj.onQueueRemove(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueueRemove();

			// Registered onQueueRemove callback
			expect(dkcobj.onQueueRemove(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// Registered onQueueRemove callback
				expect(dkcobj.onQueueRemove(function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.offQueueRemove();

					// Registered onQueueRemove callback
					expect(dkcobj.onQueueRemove(function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.offQueueRemove();

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-643-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-643-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-643-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-643-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueueRemove(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueueRemove(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-644-', null, 'queueremove-onetime-value-644-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-644-', null, 'queueremove-onetime-value-644-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-644-', null, 'queueremove-onetime-value-644-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-644-', null, 'queueremove-onetime-value-644-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-644-', 1, true, false, 'mypass', function(error)
		{
			expect(error).to.be.null;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-644-', 2, true, false, 'mypass', function(error)
			{
				expect(error).to.be.null;

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-644-', 2, true, false, 'mypass', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-644-', 1, true, false, 'mypass', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueueRemove(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueueRemove(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queueremove-permanent-prefix-645-', null, 'queueremove-permanent-value-645-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-645-', null, 'queueremove-permanent-value-645-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-645-', null, 'queueremove-permanent-value-645-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-645-', null, 'queueremove-permanent-value-645-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-645-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-645-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-645-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.false;
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-645-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.false;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::FIFO-QueueRemove(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueueRemove(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queueremove-permanent-prefix-646-', null, 'queueremove-permanent-value-646-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-646-', null, 'queueremove-permanent-value-646-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-646-', null, 'queueremove-permanent-value-646-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-646-', null, 'queueremove-permanent-value-646-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queueRemove) callback
		expect(dkcobj.on('queueRemove', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queueRemove');

			// Registered on(queueRemove) callback
			expect(dkcobj.on('queueRemove', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('queueRemove');

				// Registered on(queueRemove) callback
				expect(dkcobj.on('queueRemove', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.off('queueRemove');

					// Registered on(queueRemove) callback
					expect(dkcobj.on('queueRemove', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.off('queueRemove');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove('queueremove-permanent-prefix-646-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove('queueremove-permanent-prefix-646-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove('queueremove-permanent-prefix-646-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-646-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueueRemove(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueueRemove(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queueremove-permanent-prefix-647-', null, 'queueremove-permanent-value-647-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-647-', null, 'queueremove-permanent-value-647-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-647-', null, 'queueremove-permanent-value-647-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-647-', null, 'queueremove-permanent-value-647-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueueRemove callback
		expect(dkcobj.onQueueRemove(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueueRemove();

			// Registered onQueueRemove callback
			expect(dkcobj.onQueueRemove(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// Registered onQueueRemove callback
				expect(dkcobj.onQueueRemove(function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.offQueueRemove();

					// Registered onQueueRemove callback
					expect(dkcobj.onQueueRemove(function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.offQueueRemove();

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove('queueremove-permanent-prefix-647-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove('queueremove-permanent-prefix-647-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove('queueremove-permanent-prefix-647-', 2, true, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-647-', 1, true, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-QueueRemove(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-QueueRemove(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queueremove-permanent-prefix-648-', null, 'queueremove-permanent-value-648-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-648-', null, 'queueremove-permanent-value-648-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-648-', null, 'queueremove-permanent-value-648-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-648-', null, 'queueremove-permanent-value-648-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-648-', 1, true, false, 'mypass', function(error)
		{
			expect(error).to.be.null;

			// remove
			expect(dkcobj.queueRemove('queueremove-permanent-prefix-648-', 2, true, false, 'mypass', function(error)
			{
				expect(error).to.be.null;

				// remove
				expect(dkcobj.queueRemove('queueremove-permanent-prefix-648-', 2, true, false, 'mypass', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');

					// remove
					expect(dkcobj.queueRemove('queueremove-permanent-prefix-648-', 1, true, false, 'mypass', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test LIFO-QueueRemove
	//-------------------------------------------------------------------
	//
	// k2hdkc::LIFO-QueueRemove(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueueRemove(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-741-', null, 'queueremove-onetime-value-741-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-741-', null, 'queueremove-onetime-value-741-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-741-', null, 'queueremove-onetime-value-741-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-741-', null, 'queueremove-onetime-value-741-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-741-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-741-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-741-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.false;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-741-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.false;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::LIFO-QueueRemove(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueueRemove(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-742-', null, 'queueremove-onetime-value-742-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-742-', null, 'queueremove-onetime-value-742-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-742-', null, 'queueremove-onetime-value-742-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-742-', null, 'queueremove-onetime-value-742-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queueRemove) callback
		expect(dkcobj.on('queueRemove', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queueRemove');

			// Registered on(queueRemove) callback
			expect(dkcobj.on('queueRemove', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('queueRemove');

				// Registered on(queueRemove) callback
				expect(dkcobj.on('queueRemove', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.off('queueRemove');

					// Registered on(queueRemove) callback
					expect(dkcobj.on('queueRemove', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.off('queueRemove');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-742-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-742-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-742-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-742-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueueRemove(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueueRemove(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-743-', null, 'queueremove-onetime-value-743-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-743-', null, 'queueremove-onetime-value-743-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-743-', null, 'queueremove-onetime-value-743-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-743-', null, 'queueremove-onetime-value-743-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueueRemove callback
		expect(dkcobj.onQueueRemove(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueueRemove();

			// Registered onQueueRemove callback
			expect(dkcobj.onQueueRemove(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// Registered onQueueRemove callback
				expect(dkcobj.onQueueRemove(function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.offQueueRemove();

					// Registered onQueueRemove callback
					expect(dkcobj.onQueueRemove(function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.offQueueRemove();

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-743-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-743-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-743-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-743-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueueRemove(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueueRemove(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-744-', null, 'queueremove-onetime-value-744-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-744-', null, 'queueremove-onetime-value-744-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-744-', null, 'queueremove-onetime-value-744-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queueremove-onetime-prefix-744-', null, 'queueremove-onetime-value-744-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-744-', 1, false, false, 'mypass', function(error)
		{
			expect(error).to.be.null;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-744-', 2, false, false, 'mypass', function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-744-', 2, false, false, 'mypass', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'queueremove-onetime-prefix-744-', 1, false, false, 'mypass', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueueRemove(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueueRemove(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queueremove-permanent-prefix-745-', null, 'queueremove-permanent-value-745-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-745-', null, 'queueremove-permanent-value-745-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-745-', null, 'queueremove-permanent-value-745-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-745-', null, 'queueremove-permanent-value-745-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-745-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-745-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-745-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.false;
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-745-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.false;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::LIFO-QueueRemove(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueueRemove(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queueremove-permanent-prefix-746-', null, 'queueremove-permanent-value-746-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-746-', null, 'queueremove-permanent-value-746-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-746-', null, 'queueremove-permanent-value-746-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-746-', null, 'queueremove-permanent-value-746-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queueRemove) callback
		expect(dkcobj.on('queueRemove', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queueRemove');

			// Registered on(queueRemove) callback
			expect(dkcobj.on('queueRemove', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('queueRemove');

				// Registered on(queueRemove) callback
				expect(dkcobj.on('queueRemove', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.off('queueRemove');

					// Registered on(queueRemove) callback
					expect(dkcobj.on('queueRemove', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.off('queueRemove');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove('queueremove-permanent-prefix-746-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove('queueremove-permanent-prefix-746-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove('queueremove-permanent-prefix-746-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-746-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueueRemove(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueueRemove(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queueremove-permanent-prefix-747-', null, 'queueremove-permanent-value-747-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-747-', null, 'queueremove-permanent-value-747-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-747-', null, 'queueremove-permanent-value-747-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-747-', null, 'queueremove-permanent-value-747-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueueRemove callback
		expect(dkcobj.onQueueRemove(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueueRemove();

			// Registered onQueueRemove callback
			expect(dkcobj.onQueueRemove(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// Registered onQueueRemove callback
				expect(dkcobj.onQueueRemove(function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.offQueueRemove();

					// Registered onQueueRemove callback
					expect(dkcobj.onQueueRemove(function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.offQueueRemove();

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove('queueremove-permanent-prefix-747-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove('queueremove-permanent-prefix-747-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove('queueremove-permanent-prefix-747-', 2, false, false, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-747-', 1, false, false, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-QueueRemove(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-QueueRemove(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('queueremove-permanent-prefix-748-', null, 'queueremove-permanent-value-748-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-748-', null, 'queueremove-permanent-value-748-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-748-', null, 'queueremove-permanent-value-748-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('queueremove-permanent-prefix-748-', null, 'queueremove-permanent-value-748-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('queueremove-permanent-prefix-748-', 1, false, false, 'mypass', function(error)
		{
			expect(error).to.be.null;

			// remove
			expect(dkcobj.queueRemove('queueremove-permanent-prefix-748-', 2, false, false, 'mypass', function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// remove
				expect(dkcobj.queueRemove('queueremove-permanent-prefix-748-', 2, false, false, 'mypass', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');

					// remove
					expect(dkcobj.queueRemove('queueremove-permanent-prefix-748-', 1, false, false, 'mypass', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test FIFO-KeyQueuePush
	//-------------------------------------------------------------------
	//
	// k2hdkc::FIFO-KeyQueuePush(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-801-', 'queuepush-onetime-key-801-fifo-a', 'queuepush-onetime-value-801-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-801-', true, true, 'mypass');
		expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(onetime) - No Callback : QueuePop got not array');
		expect(queuelist[0]).to.equal('queuepush-onetime-key-801-fifo-a');
		expect(queuelist[1]).to.equal('queuepush-onetime-value-801-fifo');
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-801-', true, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::FIFO-KeyQueuePush(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// Registered on(queuePush) callback
		expect(dkcobj.on('queuePush', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queuePush');

			// check(pop)
			var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-802-', true, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(onetime) - on Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('queuepush-onetime-key-802-fifo-a');
			expect(queuelist[1]).to.equal('queuepush-onetime-value-802-fifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-802-', true, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-802-', 'queuepush-onetime-key-802-fifo-a', 'queuepush-onetime-value-802-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePush(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// Registered on(queuePush) callback
		expect(dkcobj.onQueuePush(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueuePush();

			// check(pop)
			var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-803-', true, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(onetime) - onXXX Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('queuepush-onetime-key-803-fifo-a');
			expect(queuelist[1]).to.equal('queuepush-onetime-value-803-fifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-803-', true, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-803-', 'queuepush-onetime-key-803-fifo-a', 'queuepush-onetime-value-803-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePush(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'queuepush-onetime-prefix-804-', 'queuepush-onetime-key-804-fifo-a', 'queuepush-onetime-value-804-fifo', true, true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check(pop)
			var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-804-', true, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(onetime) - inline Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('queuepush-onetime-key-804-fifo-a');
			expect(queuelist[1]).to.equal('queuepush-onetime-value-804-fifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'queuepush-onetime-prefix-804-', true, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test FIFO-KeyQueuePush
	//-------------------------------------------------------------------
	//
	// k2hdkc::FIFO-KeyQueuePush(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-801-', 'kqueuepush-onetime-key-801-fifo-a', 'kqueuepush-onetime-value-801-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-801-', true, true, 'mypass');
		expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(onetime) - No Callback : QueuePop got not array');
		expect(queuelist[0]).to.equal('kqueuepush-onetime-key-801-fifo-a');
		expect(queuelist[1]).to.equal('kqueuepush-onetime-value-801-fifo');
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-801-', true, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::FIFO-KeyQueuePush(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// Registered on(queuePush) callback
		expect(dkcobj.on('queuePush', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queuePush');

			// check(pop)
			var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-802-', true, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(onetime) - on Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-onetime-key-802-fifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-onetime-value-802-fifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-802-', true, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-802-', 'kqueuepush-onetime-key-802-fifo-a', 'kqueuepush-onetime-value-802-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePush(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// Registered on(queuePush) callback
		expect(dkcobj.onQueuePush(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueuePush();

			// check(pop)
			var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-803-', true, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(onetime) - onXXX Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-onetime-key-803-fifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-onetime-value-803-fifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-803-', true, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-803-', 'kqueuepush-onetime-key-803-fifo-a', 'kqueuepush-onetime-value-803-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePush(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-804-', 'kqueuepush-onetime-key-804-fifo-a', 'kqueuepush-onetime-value-804-fifo', true, true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check(pop)
			var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-804-', true, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(onetime) - inline Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-onetime-key-804-fifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-onetime-value-804-fifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-804-', true, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePush(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepush-permanent-prefix-805-', 'kqueuepush-permanent-key-805-fifo-a', 'kqueuepush-permanent-value-805-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		var	queuelist = dkcobj.queuePop('kqueuepush-permanent-prefix-805-', true, true, 'mypass');
		expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(permanent) - No Callback : QueuePop got not array');
		expect(queuelist[0]).to.equal('kqueuepush-permanent-key-805-fifo-a');
		expect(queuelist[1]).to.equal('kqueuepush-permanent-value-805-fifo');
		expect(dkcobj.queuePop('kqueuepush-permanent-prefix-805-', true, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::FIFO-KeyQueuePush(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered on(queuePush) callback
		expect(dkcobj.on('queuePush', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queuePush');

			// check(pop)
			var	queuelist = dkcobj.queuePop('kqueuepush-permanent-prefix-806-', true, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(permanent) - on Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-permanent-key-806-fifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-permanent-value-806-fifo');
			expect(dkcobj.queuePop('kqueuepush-permanent-prefix-806-', true, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush('kqueuepush-permanent-prefix-806-', 'kqueuepush-permanent-key-806-fifo-a', 'kqueuepush-permanent-value-806-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePush(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered on(queuePush) callback
		expect(dkcobj.onQueuePush(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueuePush();

			// check(pop)
			var	queuelist = dkcobj.queuePop('kqueuepush-permanent-prefix-807-', true, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(permanent) - onXXX Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-permanent-key-807-fifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-permanent-value-807-fifo');
			expect(dkcobj.queuePop('kqueuepush-permanent-prefix-807-', true, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush('kqueuepush-permanent-prefix-807-', 'kqueuepush-permanent-key-807-fifo-a', 'kqueuepush-permanent-value-807-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePush(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePush(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepush-permanent-prefix-808-', 'kqueuepush-permanent-key-808-fifo-a', 'kqueuepush-permanent-value-808-fifo', true, true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check(pop)
			var	queuelist = dkcobj.queuePop('kqueuepush-permanent-prefix-808-', true, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePush(permanent) - inline Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-permanent-key-808-fifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-permanent-value-808-fifo');
			expect(dkcobj.queuePop('kqueuepush-permanent-prefix-808-', true, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test LIFO-KeyQueuePush
	//-------------------------------------------------------------------
	//
	// k2hdkc::LIFO-KeyQueuePush(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePush(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-811-', 'kqueuepush-onetime-key-811-lifo-a', 'kqueuepush-onetime-value-811-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-811-', false, true, 'mypass');
		expect(queuelist instanceof Array, 'k2hdkc::LIFO-KeyQueuePush(onetime) - No Callback : QueuePop got not array');
		expect(queuelist[0]).to.equal('kqueuepush-onetime-key-811-lifo-a');
		expect(queuelist[1]).to.equal('kqueuepush-onetime-value-811-lifo');
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-811-', false, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::LIFO-KeyQueuePush(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePush(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// Registered on(queuePush) callback
		expect(dkcobj.on('queuePush', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queuePush');

			// check(pop)
			var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-812-', false, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::LIFO-KeyQueuePush(onetime) - on Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-onetime-key-812-lifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-onetime-value-812-lifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-812-', false, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-812-', 'kqueuepush-onetime-key-812-lifo-a', 'kqueuepush-onetime-value-812-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueuePush(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePush(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// Registered on(queuePush) callback
		expect(dkcobj.onQueuePush(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueuePush();

			// check(pop)
			var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-813-', false, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::LIFO-KeyQueuePush(onetime) - onXXX Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-onetime-key-813-lifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-onetime-value-813-lifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-813-', false, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-813-', 'kqueuepush-onetime-key-813-lifo-a', 'kqueuepush-onetime-value-813-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueuePush(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePush(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-814-', 'kqueuepush-onetime-key-814-lifo-a', 'kqueuepush-onetime-value-814-lifo', false, true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check(pop)
			var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-814-', false, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::LIFO-KeyQueuePush(onetime) - inline Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-onetime-key-814-lifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-onetime-value-814-lifo');
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepush-onetime-prefix-814-', false, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueuePush(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePush(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepush-permanent-prefix-815-', 'kqueuepush-permanent-key-815-lifo-a', 'kqueuepush-permanent-value-815-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		var	queuelist = dkcobj.queuePop('kqueuepush-permanent-prefix-815-', false, true, 'mypass');
		expect(queuelist instanceof Array, 'k2hdkc::LIFO-KeyQueuePush(permanent) - No Callback : QueuePop got not array');
		expect(queuelist[0]).to.equal('kqueuepush-permanent-key-815-lifo-a');
		expect(queuelist[1]).to.equal('kqueuepush-permanent-value-815-lifo');
		expect(dkcobj.queuePop('kqueuepush-permanent-prefix-815-', false, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::LIFO-KeyQueuePush(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePush(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered on(queuePush) callback
		expect(dkcobj.on('queuePush', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queuePush');

			// check(pop)
			var	queuelist = dkcobj.queuePop('kqueuepush-permanent-prefix-816-', false, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::LIFO-KeyQueuePush(permanent) - on Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-permanent-key-816-lifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-permanent-value-816-lifo');
			expect(dkcobj.queuePop('kqueuepush-permanent-prefix-816-', false, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush('kqueuepush-permanent-prefix-816-', 'kqueuepush-permanent-key-816-lifo-a', 'kqueuepush-permanent-value-816-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueuePush(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePush(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered on(queuePush) callback
		expect(dkcobj.onQueuePush(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueuePush();

			// check(pop)
			var	queuelist = dkcobj.queuePop('kqueuepush-permanent-prefix-817-', false, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::LIFO-KeyQueuePush(permanent) - onXXX Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-permanent-key-817-lifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-permanent-value-817-lifo');
			expect(dkcobj.queuePop('kqueuepush-permanent-prefix-817-', false, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;

		// push
		expect(dkcobj.queuePush('kqueuepush-permanent-prefix-817-', 'kqueuepush-permanent-key-817-lifo-a', 'kqueuepush-permanent-value-817-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueuePush(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePush(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepush-permanent-prefix-818-', 'kqueuepush-permanent-key-818-lifo-a', 'kqueuepush-permanent-value-818-lifo', false, true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// check(pop)
			var	queuelist = dkcobj.queuePop('kqueuepush-permanent-prefix-818-', false, true, 'mypass');
			expect(queuelist instanceof Array, 'k2hdkc::LIFO-KeyQueuePush(permanent) - inline Callback : QueuePop got not array');
			expect(queuelist[0]).to.equal('kqueuepush-permanent-key-818-lifo-a');
			expect(queuelist[1]).to.equal('kqueuepush-permanent-value-818-lifo');
			expect(dkcobj.queuePop('kqueuepush-permanent-prefix-818-', false, true, 'mypass')).to.be.null;

			dkcobj.clean();
			testobj.timeout(orgtime);
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test FIFO-KeyQueuePop
	//-------------------------------------------------------------------
	//
	// k2hdkc::FIFO-KeyQueuePop(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePop(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-821-', 'kqueuepop-onetime-key-821-fifo-a', 'kqueuepop-onetime-value-821-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-821-', true, true, 'mypass');
		expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePop(onetime) - No Callback : QueuePop got not array');
		expect(queuelist[0]).to.equal('kqueuepop-onetime-key-821-fifo-a');
		expect(queuelist[1]).to.equal('kqueuepop-onetime-value-821-fifo');
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-821-', true, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::FIFO-KeyQueuePop(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePop(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-822-', 'kqueuepop-onetime-key-822-fifo-a', 'kqueuepop-onetime-value-822-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queuePop) callback
		expect(dkcobj.on('queuePop', function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-onetime-key-822-fifo-a');
			expect(value).to.equal('kqueuepop-onetime-value-822-fifo');
			dkcobj.off('queuePop');

			// Registered on(queuePop) callback
			expect(dkcobj.on('queuePop', function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;
				dkcobj.off('queuePop');

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-822-', true, true, 'mypass')).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-822-', true, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePop(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePop(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-823-', 'kqueuepop-onetime-key-823-fifo-a', 'kqueuepop-onetime-value-823-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueuePop callback
		expect(dkcobj.onQueuePop(function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-onetime-key-823-fifo-a');
			expect(value).to.equal('kqueuepop-onetime-value-823-fifo');
			dkcobj.offQueuePop();

			// Registered onQueuePop callback
			expect(dkcobj.onQueuePop(function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;
				dkcobj.offQueuePop();

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-823-', true, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-823-', true, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePop(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePop(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-824-', 'kqueuepop-onetime-key-824-fifo-a', 'kqueuepop-onetime-value-824-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-824-', true, true, 'mypass', function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-onetime-key-824-fifo-a');
			expect(value).to.equal('kqueuepop-onetime-value-824-fifo');

			// pop
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-824-', true, true, 'mypass', function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePop(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePop(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepop-permanent-prefix-825-', 'kqueuepop-permanent-key-825-fifo-a', 'kqueuepop-permanent-value-825-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		var	queuelist = dkcobj.queuePop('kqueuepop-permanent-prefix-825-', true, true, 'mypass');
		expect(queuelist instanceof Array, 'k2hdkc::FIFO-KeyQueuePop(permanent) - No Callback : QueuePop got not array');
		expect(queuelist[0]).to.equal('kqueuepop-permanent-key-825-fifo-a');
		expect(queuelist[1]).to.equal('kqueuepop-permanent-value-825-fifo');
		expect(dkcobj.queuePop('kqueuepop-permanent-prefix-825-', true, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::FIFO-KeyQueuePop(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePop(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepop-permanent-prefix-826-', 'kqueuepop-permanent-key-826-fifo-a', 'kqueuepop-permanent-value-826-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queuePop) callback
		expect(dkcobj.on('queuePop', function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-permanent-key-826-fifo-a');
			expect(value).to.equal('kqueuepop-permanent-value-826-fifo');
			dkcobj.off('queuePop');

			// Registered on(queuePop) callback
			expect(dkcobj.on('queuePop', function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;
				dkcobj.off('queuePop');

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop('kqueuepop-permanent-prefix-826-', true, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('kqueuepop-permanent-prefix-826-', true, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePop(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePop(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepop-permanent-prefix-827-', 'kqueuepop-permanent-key-827-fifo-a', 'kqueuepop-permanent-value-827-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueuePop callback
		expect(dkcobj.onQueuePop(function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-permanent-key-827-fifo-a');
			expect(value).to.equal('kqueuepop-permanent-value-827-fifo');
			dkcobj.offQueuePop();

			// Registered onQueuePop callback
			expect(dkcobj.onQueuePop(function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;
				dkcobj.offQueuePop();

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop('kqueuepop-permanent-prefix-827-', true, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('kqueuepop-permanent-prefix-827-', true, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueuePop(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueuePop(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepop-permanent-prefix-828-', 'kqueuepop-permanent-key-828-fifo-a', 'kqueuepop-permanent-value-828-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('kqueuepop-permanent-prefix-828-', true, true, 'mypass', function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-permanent-key-828-fifo-a');
			expect(value).to.equal('kqueuepop-permanent-value-828-fifo');

			// pop
			expect(dkcobj.queuePop('kqueuepop-permanent-prefix-828-', true, true, 'mypass', function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test LIFO-KeyQueuePop
	//-------------------------------------------------------------------
	//
	// k2hdkc::LIFO-KeyQueuePop(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePop(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-831-', 'kqueuepop-onetime-key-831-lifo-a', 'kqueuepop-onetime-value-831-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		var	queuelist = dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-831-', false, true, 'mypass');
		expect(queuelist instanceof Array, 'k2hdkc::LIFO-KeyQueuePop(onetime) - No Callback : QueuePop got not array');
		expect(queuelist[0]).to.equal('kqueuepop-onetime-key-831-lifo-a');
		expect(queuelist[1]).to.equal('kqueuepop-onetime-value-831-lifo');
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-831-', false, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::LIFO-KeyQueuePop(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePop(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-832-', 'kqueuepop-onetime-key-832-lifo-a', 'kqueuepop-onetime-value-832-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queuePop) callback
		expect(dkcobj.on('queuePop', function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-onetime-key-832-lifo-a');
			expect(value).to.equal('kqueuepop-onetime-value-832-lifo');
			dkcobj.off('queuePop');

			// Registered on(queuePop) callback
			expect(dkcobj.on('queuePop', function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;
				dkcobj.off('queuePop');

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-832-', false, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-832-', false, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueuePop(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePop(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-833-', 'kqueuepop-onetime-key-833-lifo-a', 'kqueuepop-onetime-value-833-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueuePop callback
		expect(dkcobj.onQueuePop(function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-onetime-key-833-lifo-a');
			expect(value).to.equal('kqueuepop-onetime-value-833-lifo');
			dkcobj.offQueuePop();

			// Registered onQueuePop callback
			expect(dkcobj.onQueuePop(function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;
				dkcobj.offQueuePop();

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-833-', false, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-833-', false, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueuePop(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePop(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-834-', 'kqueuepop-onetime-key-834-lifo-a', 'kqueuepop-onetime-value-834-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-834-', false, true, 'mypass', function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-onetime-key-834-lifo-a');
			expect(value).to.equal('kqueuepop-onetime-value-834-lifo');

			// pop
			expect(dkcobj.queuePop(slvconf, slvport, true, false, 'kqueuepop-onetime-prefix-834-', false, true, 'mypass', function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueuePop(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePop(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepop-permanent-prefix-835-', 'kqueuepop-permanent-key-835-lifo-a', 'kqueuepop-permanent-value-835-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// check(pop)
		var	queuelist = dkcobj.queuePop('kqueuepop-permanent-prefix-835-', false, true, 'mypass');
		expect(queuelist instanceof Array, 'k2hdkc::LIFO-KeyQueuePop(permanent) - No Callback : QueuePop got not array');
		expect(queuelist[0]).to.equal('kqueuepop-permanent-key-835-lifo-a');
		expect(queuelist[1]).to.equal('kqueuepop-permanent-value-835-lifo');
		expect(dkcobj.queuePop('kqueuepop-permanent-prefix-835-', false, true, 'mypass')).to.be.null;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::LIFO-KeyQueuePop(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePop(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepop-permanent-prefix-836-', 'kqueuepop-permanent-key-836-lifo-a', 'kqueuepop-permanent-value-836-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queuePop) callback
		expect(dkcobj.on('queuePop', function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-permanent-key-836-lifo-a');
			expect(value).to.equal('kqueuepop-permanent-value-836-lifo');
			dkcobj.off('queuePop');

			// Registered on(queuePop) callback
			expect(dkcobj.on('queuePop', function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;
				dkcobj.off('queuePop');

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop('kqueuepop-permanent-prefix-836-', false, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('kqueuepop-permanent-prefix-836-', false, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueuePop(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePop(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepop-permanent-prefix-837-', 'kqueuepop-permanent-key-837-lifo-a', 'kqueuepop-permanent-value-837-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueuePop callback
		expect(dkcobj.onQueuePop(function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-permanent-key-837-lifo-a');
			expect(value).to.equal('kqueuepop-permanent-value-837-lifo');
			dkcobj.offQueuePop();

			// Registered onQueuePop callback
			expect(dkcobj.onQueuePop(function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;
				dkcobj.offQueuePop();

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;

			// pop
			expect(dkcobj.queuePop('kqueuepop-permanent-prefix-837-', false, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('kqueuepop-permanent-prefix-837-', false, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueuePop(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueuePop(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueuepop-permanent-prefix-838-', 'kqueuepop-permanent-key-838-lifo-a', 'kqueuepop-permanent-value-838-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// pop
		expect(dkcobj.queuePop('kqueuepop-permanent-prefix-838-', false, true, 'mypass', function(error, key, value)
		{
			expect(error).to.be.null;

			expect(key).to.equal('kqueuepop-permanent-key-838-lifo-a');
			expect(value).to.equal('kqueuepop-permanent-value-838-lifo');

			// pop
			expect(dkcobj.queuePop('kqueuepop-permanent-prefix-838-', false, true, 'mypass', function(error, key, value)
			{
				expect(error).to.be.null;

				expect(key).to.be.null;
				expect(value).to.be.null;

				dkcobj.clean();
				testobj.timeout(orgtime);
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test FIFO-KeyQueueRemove
	//-------------------------------------------------------------------
	//
	// k2hdkc::FIFO-KeyQueueRemove(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueueRemove(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-841-', 'kqueueremove-onetime-key-841-fifo-a', 'kqueueremove-onetime-value-841-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-841-', 'kqueueremove-onetime-key-841-fifo-b', 'kqueueremove-onetime-value-841-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-841-', 'kqueueremove-onetime-key-841-fifo-c', 'kqueueremove-onetime-value-841-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-841-', 'kqueueremove-onetime-key-841-fifo-d', 'kqueueremove-onetime-value-841-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-841-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-841-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-841-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.false;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-841-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.false;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::FIFO-KeyQueueRemove(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueueRemove(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-842-', 'kqueueremove-onetime-key-842-fifo-a', 'kqueueremove-onetime-value-842-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-842-', 'kqueueremove-onetime-key-842-fifo-b', 'kqueueremove-onetime-value-842-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-842-', 'kqueueremove-onetime-key-842-fifo-c', 'kqueueremove-onetime-value-842-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-842-', 'kqueueremove-onetime-key-842-fifo-d', 'kqueueremove-onetime-value-842-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queueRemove) callback
		expect(dkcobj.on('queueRemove', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queueRemove');

			// Registered on(queueRemove) callback
			expect(dkcobj.on('queueRemove', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('queueRemove');

				// Registered on(queueRemove) callback
				expect(dkcobj.on('queueRemove', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.off('queueRemove');


					// Registered on(queueRemove) callback
					expect(dkcobj.on('queueRemove', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.off('queueRemove');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-842-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-842-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-842-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-842-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueueRemove(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueueRemove(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-843-', 'kqueueremove-onetime-key-843-fifo-a', 'kqueueremove-onetime-value-843-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-843-', 'kqueueremove-onetime-key-843-fifo-b', 'kqueueremove-onetime-value-843-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-843-', 'kqueueremove-onetime-key-843-fifo-c', 'kqueueremove-onetime-value-843-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-843-', 'kqueueremove-onetime-key-843-fifo-d', 'kqueueremove-onetime-value-843-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueueRemove callback
		expect(dkcobj.onQueueRemove(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueueRemove();

			// Registered onQueueRemove callback
			expect(dkcobj.onQueueRemove(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// Registered onQueueRemove callback
				expect(dkcobj.onQueueRemove(function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.offQueueRemove();

					// Registered onQueueRemove callback
					expect(dkcobj.onQueueRemove(function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.offQueueRemove();

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-843-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-843-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-843-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-843-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueueRemove(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueueRemove(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-844-', 'kqueueremove-onetime-key-844-fifo-a', 'kqueueremove-onetime-value-844-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-844-', 'kqueueremove-onetime-key-844-fifo-b', 'kqueueremove-onetime-value-844-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-844-', 'kqueueremove-onetime-key-844-fifo-c', 'kqueueremove-onetime-value-844-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-844-', 'kqueueremove-onetime-key-844-fifo-d', 'kqueueremove-onetime-value-844-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-844-', 1, true, true, 'mypass', function(error)
		{
			expect(error).to.be.null;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-844-', 2, true, true, 'mypass', function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-844-', 2, true, true, 'mypass', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-844-', 1, true, true, 'mypass', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueueRemove(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueueRemove(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-845-', 'kqueueremove-permanent-key-845-fifo-a', 'kqueueremove-permanent-value-845-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-845-', 'kqueueremove-permanent-key-845-fifo-b', 'kqueueremove-permanent-value-845-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-845-', 'kqueueremove-permanent-key-845-fifo-c', 'kqueueremove-permanent-value-845-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-845-', 'kqueueremove-permanent-key-845-fifo-d', 'kqueueremove-permanent-value-845-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-845-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-845-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-845-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.false;
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-845-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.false;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::FIFO-KeyQueueRemove(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueueRemove(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-846-', 'kqueueremove-permanent-key-846-fifo-a', 'kqueueremove-permanent-value-846-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-846-', 'kqueueremove-permanent-key-846-fifo-b', 'kqueueremove-permanent-value-846-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-846-', 'kqueueremove-permanent-key-846-fifo-c', 'kqueueremove-permanent-value-846-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-846-', 'kqueueremove-permanent-key-846-fifo-d', 'kqueueremove-permanent-value-846-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queueRemove) callback
		expect(dkcobj.on('queueRemove', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queueRemove');

			// Registered on(queueRemove) callback
			expect(dkcobj.on('queueRemove', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('queueRemove');

				// Registered on(queueRemove) callback
				expect(dkcobj.on('queueRemove', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.off('queueRemove');

					// Registered on(queueRemove) callback
					expect(dkcobj.on('queueRemove', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.off('queueRemove');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-846-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-846-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-846-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-846-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueueRemove(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueueRemove(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-847-', 'kqueueremove-permanent-key-847-fifo-a', 'kqueueremove-permanent-value-847-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-847-', 'kqueueremove-permanent-key-847-fifo-b', 'kqueueremove-permanent-value-847-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-847-', 'kqueueremove-permanent-key-847-fifo-c', 'kqueueremove-permanent-value-847-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-847-', 'kqueueremove-permanent-key-847-fifo-d', 'kqueueremove-permanent-value-847-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueueRemove callback
		expect(dkcobj.onQueueRemove(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueueRemove();

			// Registered onQueueRemove callback
			expect(dkcobj.onQueueRemove(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// Registered onQueueRemove callback
				expect(dkcobj.onQueueRemove(function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.offQueueRemove();

					// Registered onQueueRemove callback
					expect(dkcobj.onQueueRemove(function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.offQueueRemove();

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-847-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-847-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-847-', 2, true, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-847-', 1, true, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::FIFO-KeyQueueRemove(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::FIFO-KeyQueueRemove(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-848-', 'kqueueremove-permanent-key-848-fifo-a', 'kqueueremove-permanent-value-848-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-848-', 'kqueueremove-permanent-key-848-fifo-b', 'kqueueremove-permanent-value-848-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-848-', 'kqueueremove-permanent-key-848-fifo-c', 'kqueueremove-permanent-value-848-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-848-', 'kqueueremove-permanent-key-848-fifo-d', 'kqueueremove-permanent-value-848-fifo', true, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-848-', 1, true, true, 'mypass', function(error)
		{
			expect(error).to.be.null;

			// remove
			expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-848-', 2, true, true, 'mypass', function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// remove
				expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-848-', 2, true, true, 'mypass', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');

					// remove
					expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-848-', 1, true, true, 'mypass', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test LIFO-KeyQueueRemove
	//-------------------------------------------------------------------
	//
	// k2hdkc::LIFO-KeyQueueRemove(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueueRemove(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-851-', 'kqueueremove-onetime-key-851-lifo-a', 'kqueueremove-onetime-value-851-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-851-', 'kqueueremove-onetime-key-851-lifo-b', 'kqueueremove-onetime-value-851-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-851-', 'kqueueremove-onetime-key-851-lifo-c', 'kqueueremove-onetime-value-851-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-851-', 'kqueueremove-onetime-key-851-lifo-d', 'kqueueremove-onetime-value-851-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-851-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-851-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-851-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.false;
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-851-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.false;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::LIFO-KeyQueueRemove(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueueRemove(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-852-', 'kqueueremove-onetime-key-852-lifo-a', 'kqueueremove-onetime-value-852-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-852-', 'kqueueremove-onetime-key-852-lifo-b', 'kqueueremove-onetime-value-852-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-852-', 'kqueueremove-onetime-key-852-lifo-c', 'kqueueremove-onetime-value-852-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-852-', 'kqueueremove-onetime-key-852-lifo-d', 'kqueueremove-onetime-value-852-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queueRemove) callback
		expect(dkcobj.on('queueRemove', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queueRemove');

			// Registered on(queueRemove) callback
			expect(dkcobj.on('queueRemove', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('queueRemove');

				// Registered on(queueRemove) callback
				expect(dkcobj.on('queueRemove', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.off('queueRemove');

					// Registered on(queueRemove) callback
					expect(dkcobj.on('queueRemove', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.off('queueRemove');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-852-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-852-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-852-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-852-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueueRemove(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueueRemove(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-853-', 'kqueueremove-onetime-key-853-lifo-a', 'kqueueremove-onetime-value-853-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-853-', 'kqueueremove-onetime-key-853-lifo-b', 'kqueueremove-onetime-value-853-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-853-', 'kqueueremove-onetime-key-853-lifo-c', 'kqueueremove-onetime-value-853-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-853-', 'kqueueremove-onetime-key-853-lifo-d', 'kqueueremove-onetime-value-853-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueueRemove callback
		expect(dkcobj.onQueueRemove(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueueRemove();

			// Registered onQueueRemove callback
			expect(dkcobj.onQueueRemove(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// Registered onQueueRemove callback
				expect(dkcobj.onQueueRemove(function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.offQueueRemove();


					// Registered onQueueRemove callback
					expect(dkcobj.onQueueRemove(function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.offQueueRemove();

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-853-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-853-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-853-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-853-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueueRemove(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueueRemove(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc();

		// push
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-854-', 'kqueueremove-onetime-key-854-lifo-a', 'kqueueremove-onetime-value-854-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-854-', 'kqueueremove-onetime-key-854-lifo-b', 'kqueueremove-onetime-value-854-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-854-', 'kqueueremove-onetime-key-854-lifo-c', 'kqueueremove-onetime-value-854-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-854-', 'kqueueremove-onetime-key-854-lifo-d', 'kqueueremove-onetime-value-854-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-854-', 1, false, true, 'mypass', function(error)
		{
			expect(error).to.be.null;

			// remove
			expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-854-', 2, false, true, 'mypass', function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// remove
				expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-854-', 2, false, true, 'mypass', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');

					// remove
					expect(dkcobj.queueRemove(slvconf, slvport, true, false, 'kqueueremove-onetime-prefix-854-', 1, false, true, 'mypass', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueueRemove(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueueRemove(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-855-', 'kqueueremove-permanent-key-855-lifo-a', 'kqueueremove-permanent-value-855-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-855-', 'kqueueremove-permanent-key-855-lifo-b', 'kqueueremove-permanent-value-855-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-855-', 'kqueueremove-permanent-key-855-lifo-c', 'kqueueremove-permanent-value-855-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-855-', 'kqueueremove-permanent-key-855-lifo-d', 'kqueueremove-permanent-value-855-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-855-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-855-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.true;
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-855-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.false;
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-855-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.false;

		dkcobj.clean();
		testobj.timeout(orgtime);
		done();
	});

	//
	// k2hdkc::LIFO-KeyQueueRemove(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueueRemove(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-856-', 'kqueueremove-permanent-key-856-lifo-a', 'kqueueremove-permanent-value-856-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-856-', 'kqueueremove-permanent-key-856-lifo-b', 'kqueueremove-permanent-value-856-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-856-', 'kqueueremove-permanent-key-856-lifo-c', 'kqueueremove-permanent-value-856-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-856-', 'kqueueremove-permanent-key-856-lifo-d', 'kqueueremove-permanent-value-856-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(queueRemove) callback
		expect(dkcobj.on('queueRemove', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('queueRemove');

			// Registered on(queueRemove) callback
			expect(dkcobj.on('queueRemove', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('queueRemove');

				// Registered on(queueRemove) callback
				expect(dkcobj.on('queueRemove', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.off('queueRemove');

					// Registered on(queueRemove) callback
					expect(dkcobj.on('queueRemove', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.off('queueRemove');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-856-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-856-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-856-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-856-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueueRemove(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueueRemove(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-857-', 'kqueueremove-permanent-key-857-lifo-a', 'kqueueremove-permanent-value-857-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-857-', 'kqueueremove-permanent-key-857-lifo-b', 'kqueueremove-permanent-value-857-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-857-', 'kqueueremove-permanent-key-857-lifo-c', 'kqueueremove-permanent-value-857-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-857-', 'kqueueremove-permanent-key-857-lifo-d', 'kqueueremove-permanent-value-857-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onQueueRemove callback
		expect(dkcobj.onQueueRemove(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offQueueRemove();

			// Registered onQueueRemove callback
			expect(dkcobj.onQueueRemove(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// Registered onQueueRemove callback
				expect(dkcobj.onQueueRemove(function(error)
				{
					expect(error).to.equal('Failed to remove queue.');
					dkcobj.offQueueRemove();

					// Registered onQueueRemove callback
					expect(dkcobj.onQueueRemove(function(error)
					{
						expect(error).to.equal('Failed to remove queue.');
						dkcobj.offQueueRemove();

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;

					// remove
					expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-857-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.true;

				})).to.be.a('boolean').to.be.true;

				// remove
				expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-857-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.true;

			})).to.be.a('boolean').to.be.true;

			// remove
			expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-857-', 2, false, true, 'mypass')).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-857-', 1, false, true, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::LIFO-KeyQueueRemove(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::LIFO-KeyQueueRemove(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var testobj = this;
		var orgtime = testobj.timeout(10000);
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// push
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-858-', 'kqueueremove-permanent-key-858-lifo-a', 'kqueueremove-permanent-value-858-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-858-', 'kqueueremove-permanent-key-858-lifo-b', 'kqueueremove-permanent-value-858-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-858-', 'kqueueremove-permanent-key-858-lifo-c', 'kqueueremove-permanent-value-858-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;
		expect(dkcobj.queuePush('kqueueremove-permanent-prefix-858-', 'kqueueremove-permanent-key-858-lifo-d', 'kqueueremove-permanent-value-858-lifo', false, true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// remove
		expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-858-', 1, false, true, 'mypass', function(error)
		{
			expect(error).to.be.null;

			// remove
			expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-858-', 2, false, true, 'mypass', function(error)
			{
				expect(error).to.be.null;
				dkcobj.offQueueRemove();

				// remove
				expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-858-', 2, false, true, 'mypass', function(error)
				{
					expect(error).to.equal('Failed to remove queue.');

					// remove
					expect(dkcobj.queueRemove('kqueueremove-permanent-prefix-858-', 1, false, true, 'mypass', function(error)
					{
						expect(error).to.equal('Failed to remove queue.');

						dkcobj.clean();
						testobj.timeout(orgtime);
						done();
					})).to.be.a('boolean').to.be.true;
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test CasInit
	//-------------------------------------------------------------------
	//
	// k2hdkc::CasInit(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::CasInit(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casinit-onetime-key-901', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get cas value
		expect(dkcobj.casGet(slvconf, slvport, true, false, 'casinit-onetime-key-901', 'mypass')).to.equal(1234);

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::CasInit(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::CasInit(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered on(casInit) callback
		expect(dkcobj.on('casInit', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('casInit');

			// get cas value
			expect(dkcobj.casGet(slvconf, slvport, true, false, 'casinit-onetime-key-902', 'mypass')).to.equal(1234);

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casinit-onetime-key-902', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasInit(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::CasInit(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// Registered onCasInit callback
		expect(dkcobj.onCasInit(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offCasInit();

			// get cas value
			expect(dkcobj.casGet(slvconf, slvport, true, false, 'casinit-onetime-key-903', 'mypass')).to.equal(1234);

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casinit-onetime-key-903', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasInit(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::CasInit(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casinit-onetime-key-904', 1234, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// get cas value
			expect(dkcobj.casGet(slvconf, slvport, true, false, 'casinit-onetime-key-904', 'mypass')).to.equal(1234);

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasInit(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::CasInit(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// cas value init
		expect(dkcobj.casInit('casinit-permanent-key-905', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get cas value
		expect(dkcobj.casGet('casinit-permanent-key-905', 'mypass')).to.equal(1234);

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::CasInit(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::CasInit(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered on(casInit) callback
		expect(dkcobj.on('casInit', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('casInit');

			// get cas value
			expect(dkcobj.casGet('casinit-permanent-key-906', 'mypass')).to.equal(1234);

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// cas value init
		expect(dkcobj.casInit('casinit-permanent-key-906', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasInit(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::CasInit(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// Registered onCasInit callback
		expect(dkcobj.onCasInit(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offCasInit();

			// get cas value
			expect(dkcobj.casGet('casinit-permanent-key-907', 'mypass')).to.equal(1234);

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// cas value init
		expect(dkcobj.casInit('casinit-permanent-key-907', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasInit(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::CasInit(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// cas value init
		expect(dkcobj.casInit('casinit-permanent-key-908', 1234, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// get cas value
			expect(dkcobj.casGet('casinit-permanent-key-908', 'mypass')).to.equal(1234);

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test CasSet/CasGet
	//-------------------------------------------------------------------
	//
	// k2hdkc::CasSet/CasGet(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::CasSet/CasGet(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casinit-onetime-key-911', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// set cas value
		expect(dkcobj.casSet(slvconf, slvport, true, false, 'casinit-onetime-key-911', 1234, 4321, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get cas value
		expect(dkcobj.casGet(slvconf, slvport, true, false, 'casinit-onetime-key-911', 'mypass')).to.equal(4321);

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::CasSet/CasGet(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::CasSet/CasGet(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casinit-onetime-key-912', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(casSet) callback
		expect(dkcobj.on('casSet', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('casSet');

			// get cas value
			expect(dkcobj.casGet(slvconf, slvport, true, false, 'casinit-onetime-key-912', 'mypass')).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;

		// Registered on(casGet) callback
		expect(dkcobj.on('casGet', function(error, value)
		{
			expect(error).to.be.null;
			expect(value).to.equal(4321);
			dkcobj.off('casGet');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set cas value
		expect(dkcobj.casSet(slvconf, slvport, true, false, 'casinit-onetime-key-912', 1234, 4321, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasSet/CasGet(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::CasSet/CasGet(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casinit-onetime-key-913', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onCasSet callback
		expect(dkcobj.onCasSet(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offCasSet();

			// get cas value
			expect(dkcobj.casGet(slvconf, slvport, true, false, 'casinit-onetime-key-913', 'mypass')).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;

		// Registered onCasGet callback
		expect(dkcobj.onCasGet(function(error, value)
		{
			expect(error).to.be.null;
			expect(value).to.equal(4321);
			dkcobj.offCasGet();

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set cas value
		expect(dkcobj.casSet(slvconf, slvport, true, false, 'casinit-onetime-key-913', 1234, 4321, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasSet/CasGet(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::CasSet/CasGet(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casinit-onetime-key-914', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// set cas value
		expect(dkcobj.casSet(slvconf, slvport, true, false, 'casinit-onetime-key-914', 1234, 4321, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// get cas value
			expect(dkcobj.casGet(slvconf, slvport, true, false, 'casinit-onetime-key-914', 'mypass', function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.equal(4321);

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasSet/CasGet(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::CasSet/CasGet(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// cas value init
		expect(dkcobj.casInit('casinit-permanent-key-915', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// set cas value
		expect(dkcobj.casSet('casinit-permanent-key-915', 1234, 4321, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get cas value
		expect(dkcobj.casGet('casinit-permanent-key-915', 'mypass')).to.equal(4321);

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::CasSet/CasGet(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::CasSet/CasGet(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// cas value init
		expect(dkcobj.casInit('casinit-permanent-key-916', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(casSet) callback
		expect(dkcobj.on('casSet', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('casSet');

			// get cas value
			expect(dkcobj.casGet('casinit-permanent-key-916', 'mypass')).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;

		// Registered on(casGet) callback
		expect(dkcobj.on('casGet', function(error, value)
		{
			expect(error).to.be.null;
			expect(value).to.equal(4321);
			dkcobj.off('casGet');

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set cas value
		expect(dkcobj.casSet('casinit-permanent-key-916', 1234, 4321, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasSet/CasGet(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::CasSet/CasGet(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// cas value init
		expect(dkcobj.casInit('casinit-permanent-key-917', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onCasSet callback
		expect(dkcobj.onCasSet(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offCasSet();

			// get cas value
			expect(dkcobj.casGet('casinit-permanent-key-917', 'mypass')).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;

		// Registered onCasGet callback
		expect(dkcobj.onCasGet(function(error, value)
		{
			expect(error).to.be.null;
			expect(value).to.equal(4321);
			dkcobj.offCasGet();

			dkcobj.clean();
			done();
		})).to.be.a('boolean').to.be.true;

		// set cas value
		expect(dkcobj.casSet('casinit-permanent-key-917', 1234, 4321, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasSet/CasGet(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::CasSet/CasGet(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// cas value init
		expect(dkcobj.casInit('casinit-permanent-key-918', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// set cas value
		expect(dkcobj.casSet('casinit-permanent-key-918', 1234, 4321, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// get cas value
			expect(dkcobj.casGet('casinit-permanent-key-918', 'mypass', function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.equal(4321);

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test CasIncDec
	//-------------------------------------------------------------------
	//
	// k2hdkc::CasIncDec(onetime) - No Callback
	//
	it('k2hdkc - k2hdkc::CasIncDec(onetime) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casincdec-onetime-key-921', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// increment cas value
		expect(dkcobj.casIncDec(slvconf, slvport, true, false, 'casincdec-onetime-key-921', true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get cas value
		expect(dkcobj.casGet(slvconf, slvport, true, false, 'casincdec-onetime-key-921', 'mypass')).to.equal(1235);

		// decrement cas value
		expect(dkcobj.casIncDec(slvconf, slvport, true, false, 'casincdec-onetime-key-921', false, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get cas value
		expect(dkcobj.casGet(slvconf, slvport, true, false, 'casincdec-onetime-key-921', 'mypass')).to.equal(1234);

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::CasIncDec(onetime) - on Callback
	//
	it('k2hdkc - k2hdkc::CasIncDec(onetime) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casincdec-onetime-key-922', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(casIncDec) callback
		expect(dkcobj.on('casIncDec', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('casIncDec');

			// get cas value
			expect(dkcobj.casGet(slvconf, slvport, true, false, 'casincdec-onetime-key-922', 'mypass')).to.equal(1235);

			// Registered on(casIncDec) callback
			expect(dkcobj.on('casIncDec', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('casIncDec');

				// get cas value
				expect(dkcobj.casGet(slvconf, slvport, true, false, 'casincdec-onetime-key-922', 'mypass')).to.equal(1234);

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// decrement cas value
			expect(dkcobj.casIncDec(slvconf, slvport, true, false, 'casincdec-onetime-key-922', false, 'mypass', 600)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// increment cas value
		expect(dkcobj.casIncDec(slvconf, slvport, true, false, 'casincdec-onetime-key-922', true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasIncDec(onetime) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::CasIncDec(onetime) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casincdec-onetime-key-923', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onCasIncDec callback
		expect(dkcobj.onCasIncDec(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offCasIncDec();

			// get cas value
			expect(dkcobj.casGet(slvconf, slvport, true, false, 'casincdec-onetime-key-923', 'mypass')).to.equal(1235);

			// Registered onCasIncDec callback
			expect(dkcobj.onCasIncDec(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offCasIncDec();

				// get cas value
				expect(dkcobj.casGet(slvconf, slvport, true, false, 'casincdec-onetime-key-923', 'mypass')).to.equal(1234);

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// decrement cas value
			expect(dkcobj.casIncDec(slvconf, slvport, true, false, 'casincdec-onetime-key-923', false, 'mypass', 600)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// increment cas value
		expect(dkcobj.casIncDec(slvconf, slvport, true, false, 'casincdec-onetime-key-923', true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasIncDec(onetime) - inline Callback
	//
	it('k2hdkc - k2hdkc::CasIncDec(onetime) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc();

		// cas value init
		expect(dkcobj.casInit(slvconf, slvport, true, false, 'casincdec-onetime-key-924', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// increment cas value
		expect(dkcobj.casIncDec(slvconf, slvport, true, false, 'casincdec-onetime-key-924', true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// get cas value
			expect(dkcobj.casGet(slvconf, slvport, true, false, 'casincdec-onetime-key-924', 'mypass')).to.equal(1235);

			// decrement cas value
			expect(dkcobj.casIncDec(slvconf, slvport, true, false, 'casincdec-onetime-key-924', false, 'mypass', 600, function(error)
			{
				expect(error).to.be.null;

				// get cas value
				expect(dkcobj.casGet(slvconf, slvport, true, false, 'casincdec-onetime-key-924', 'mypass')).to.equal(1234);

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasIncDec(permanent) - No Callback
	//
	it('k2hdkc - k2hdkc::CasIncDec(permanent) - No Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// cas value init
		expect(dkcobj.casInit('casincdec-permanent-key-925', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// increment cas value
		expect(dkcobj.casIncDec('casincdec-permanent-key-925', true, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get cas value
		expect(dkcobj.casGet('casincdec-permanent-key-925', 'mypass')).to.equal(1235);

		// decrement cas value
		expect(dkcobj.casIncDec('casincdec-permanent-key-925', false, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// get cas value
		expect(dkcobj.casGet('casincdec-permanent-key-925', 'mypass')).to.equal(1234);

		dkcobj.clean();
		done();
	});

	//
	// k2hdkc::CasIncDec(permanent) - on Callback
	//
	it('k2hdkc - k2hdkc::CasIncDec(permanent) - on Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// cas value init
		expect(dkcobj.casInit('casincdec-permanent-key-926', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered on(casIncDec) callback
		expect(dkcobj.on('casIncDec', function(error)
		{
			expect(error).to.be.null;
			dkcobj.off('casIncDec');

			// get cas value
			expect(dkcobj.casGet('casincdec-permanent-key-926', 'mypass')).to.equal(1235);

			// Registered on(casIncDec) callback
			expect(dkcobj.on('casIncDec', function(error)
			{
				expect(error).to.be.null;
				dkcobj.off('casIncDec');

				// get cas value
				expect(dkcobj.casGet('casincdec-permanent-key-926', 'mypass')).to.equal(1234);

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// decrement cas value
			expect(dkcobj.casIncDec('casincdec-permanent-key-926', false, 'mypass', 600)).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;

		// increment cas value
		expect(dkcobj.casIncDec('casincdec-permanent-key-926', true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasIncDec(permanent) - onXXX Callback
	//
	it('k2hdkc - k2hdkc::CasIncDec(permanent) - onXXX Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// cas value init
		expect(dkcobj.casInit('casincdec-permanent-key-927', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// Registered onCasIncDec callback
		expect(dkcobj.onCasIncDec(function(error)
		{
			expect(error).to.be.null;
			dkcobj.offCasIncDec();

			// get cas value
			expect(dkcobj.casGet('casincdec-permanent-key-927', 'mypass')).to.equal(1235);

			// Registered onCasIncDec callback
			expect(dkcobj.onCasIncDec(function(error)
			{
				expect(error).to.be.null;
				dkcobj.offCasIncDec();

				// get cas value
				expect(dkcobj.casGet('casincdec-permanent-key-927', 'mypass')).to.equal(1234);

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;

			// decrement cas value
			expect(dkcobj.casIncDec('casincdec-permanent-key-927', false, 'mypass', 600)).to.be.a('boolean').to.be.true;

		})).to.be.a('boolean').to.be.true;

		// increment cas value
		expect(dkcobj.casIncDec('casincdec-permanent-key-927', true, 'mypass', 600)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hdkc::CasIncDec(permanent) - inline Callback
	//
	it('k2hdkc - k2hdkc::CasIncDec(permanent) - inline Callback', function(done){		// eslint-disable-line no-undef
		var	dkcobj = k2hdkc(slvconf, slvport, true, false);

		// cas value init
		expect(dkcobj.casInit('casincdec-permanent-key-928', 1234, 'mypass', 600)).to.be.a('boolean').to.be.true;

		// increment cas value
		expect(dkcobj.casIncDec('casincdec-permanent-key-928', true, 'mypass', 600, function(error)
		{
			expect(error).to.be.null;

			// get cas value
			expect(dkcobj.casGet('casincdec-permanent-key-928', 'mypass')).to.equal(1235);

			// decrement cas value
			expect(dkcobj.casIncDec('casincdec-permanent-key-928', false, 'mypass', 600, function(error)
			{
				expect(error).to.be.null;

				// get cas value
				expect(dkcobj.casGet('casincdec-permanent-key-928', 'mypass')).to.equal(1234);

				dkcobj.clean();
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test PrintVersion
	//-------------------------------------------------------------------
	//
	// k2hdkc::PrintVersion
	//
	it('k2hdkc - k2hdkc::PrintVersion', function(done){		// eslint-disable-line no-undef
		var	fs		= require('fs');
		var	dkcobj	= k2hdkc();

		// no fd
		expect(dkcobj.printVersion()).to.be.a('boolean').to.be.true;

		// fd
		var	fd = fs.openSync('/tmp/k2hdkc_test_printversion.log', 'w');
		expect(dkcobj.printVersion(fd)).to.be.a('boolean').to.be.true;
		fs.closeSync(fd);

		dkcobj.clean();
		done();
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
