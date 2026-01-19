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
 * CREATE:   Thu 15 Jan 2026
 * REVISION:
 */

//
// lazy-load the built CJS bundle and provide compatible exports
//
// [NOTE]
// 
// This file intentionally does NOT require('./build/cjs/index.js') at top-level.
//

import { createRequire } from 'module';

const require = createRequire(import.meta.url);

//
// Load CJS
//
let	_loaded_cjs = null;
function loadCjs()
{
	if(null !== _loaded_cjs){
		return _loaded_cjs;
	}
	_loaded_cjs = require('./build/cjs/index.js');
	return _loaded_cjs;
}

//
// Factory that delegates to CJS impl on first use
//
function k2hdkcFactory(...args)
{
	const cjs	= loadCjs();
	const impl	= (typeof cjs === 'function') ? cjs : (cjs && cjs.default) ? cjs.default : cjs;
	if(typeof impl === 'function'){
		return impl(...args);
	}
	return impl;
}

//
// Provide callable/constructible proxies for named exports
//
// This mirrors createLazyProxy from src/index.ts but scoped to ESM file.
//
function createLazyExport(name)
{
	let _cached = undefined;

	function loadActual()
	{
		if(_cached !== undefined){
			return _cached;
		}
		const _cjs		= loadCjs();
		const _actual	= (_cjs && _cjs[name]) ? _cjs[name] : (_cjs && _cjs.default && _cjs.default[name]) ? _cjs.default[name] : undefined;
		if (!_actual){
			throw new Error("Native export " + JSON.stringify(name) + " is not available from ./build/cjs/index.js");
		}
		_cached = _actual;
		return _cached;
	}

	const target = function(...args)
	{
		const _actual = loadActual();
		return _actual.apply(this, args);
	};

	const handler = {
		apply(_t, thisArg, args) {
			const _actual = loadActual();
			return _actual.apply(thisArg, args);
		},
		construct(_t, args, newTarget) {
			const _actual = loadActual();
			return Reflect.construct(_actual, args, newTarget);
		},
		get(_t, prop, receiver) {
			const _actual = loadActual();
			return Reflect.get(_actual, prop, receiver);
		},
		set(_t, prop, value, receiver) {
			const _actual = loadActual();
			return Reflect.set(_actual, prop, value, receiver);
		},
		has(_t, prop) {
			const _actual = loadActual();
			return prop in _actual;
		},
		ownKeys(_t) {
			const _actual = loadActual();
			return Reflect.ownKeys(_actual);
		},
		getOwnPropertyDescriptor(_t, prop) {
			const _actual = loadActual();
			return Object.getOwnPropertyDescriptor(_actual, prop) || undefined;
		},
		getPrototypeOf(_t) {
			const _actual = loadActual();
			return Object.getPrototypeOf(_actual);
		},
		setPrototypeOf(_t, proto) {
			const _actual = loadActual();
			return Object.setPrototypeOf(_actual, proto);
		},
		defineProperty(_t, prop, descriptor) {
			const _actual = loadActual();
			return Reflect.defineProperty(_actual, prop, descriptor);
		},
		deleteProperty(_t, prop) {
			const _actual = loadActual();
			return Reflect.deleteProperty(_actual, prop);
		}
	};

	return new Proxy(target, handler);
}

//
// Export named lazy-callable proxies
//
export const K2hdkcNode = createLazyExport('K2hdkcNode');

export default k2hdkcFactory;

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
