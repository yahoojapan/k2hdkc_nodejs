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

//-------------------------------------------------------------
// This uses 'export' to be compatible with CommonJS consumers:
//	const k2hdkc = require('k2hdkc');
//
// And works for ESM consumers with default import:
//	import k2hdkc from 'k2hdkc';
//-------------------------------------------------------------

//
// callable / constructible main factory
//
declare function k2hdkc(): k2hdkc.K2hdkcNode;

declare namespace k2hdkc
{
	//---------------------------------------------------------
	// Callback types for K2hdkcNode
	//---------------------------------------------------------
	// [NOTE]
	// We allow Error in case it might be changed to an Error object
	// in the future, but currently we do not return Error.
	//
	export type K2hdkcInitCallback = (err?: Error | string | null) => void;
	export type K2hdkcGetValueCallback = (err?: Error | string | null, value?: string | null) => void;
	export type K2hdkcGetSubkeysCallback = (err?: Error | string | null, subkeys?: string[]) => void;
	export type K2hdkcGetAttrsCallback = (err?: Error | string | null, attribute_keys?: string[]) => void;
	export type K2hdkcSetValueCallback = (err?: Error | string | null) => void;
	export type K2hdkcSetSubkeysCallback = (err?: Error | string | null) => void;
	export type K2hdkcSetAllCallback = (err?: Error | string | null) => void;
	export type K2hdkcRemoveCallback = (err?: Error | string | null) => void;
	export type K2hdkcRenameCallback = (err?: Error | string | null) => void;
	export type K2hdkcQueuePushCallback = (err?: Error | string | null) => void;
	export type K2hdkcQueuePopCallback = (err?: Error | string | null, data1?: string | null, data2?: string | null) => void;
	export type K2hdkcQueueRemoveCallback = (err?: Error | string | null) => void;
	export type K2hdkcCasInitCallback = (err?: Error | string | null) => void;
	export type K2hdkcCasGetCallback = (err?: Error | string | null, value?: number) => void;
	export type K2hdkcCasSetCallback = (err?: Error | string | null) => void;
	export type K2hdkcCasIncDecCallback = (err?: Error | string | null) => void;

	//---------------------------------------------------------
	// Emitter callback types for K2hdkcNode
	//---------------------------------------------------------
	export type OnK2hdkcEmitterCallback = (err?: string | null, ...args: any[]) => void;
	export type OnK2hdkcInitEmitterCallback = (err?: Error | string | null) => void;
	export type OnK2hdkcGetValueEmitterCallback = (err?: Error | string | null, value?: string | null) => void;
	export type OnK2hdkcGetSubkeysEmitterCallback = (err?: Error | string | null, subkeys?: string[]) => void;
	export type OnK2hdkcGetAttrsEmitterCallback = (err?: Error | string | null, attribute_keys?: string[]) => void;
	export type OnK2hdkcSetValueEmitterCallback = (err?: Error | string | null) => void;
	export type OnK2hdkcSetSubkeysEmitterCallback = (err?: Error | string | null) => void;
	export type OnK2hdkcSetAllEmitterCallback = (err?: Error | string | null) => void;
	export type OnK2hdkcRemoveEmitterCallback = (err?: Error | string | null) => void;
	export type OnK2hdkcRenameEmitterCallback = (err?: Error | string | null) => void;
	export type OnK2hdkcQueuePushEmitterCallback = (err?: Error | string | null) => void;
	export type OnK2hdkcQueuePopEmitterCallback = (err?: Error | string | null, data1?: string | null, data2?: string | null) => void;
	export type OnK2hdkcQueueRemoveEmitterCallback = (err?: Error | string | null) => void;
	export type OnK2hdkcCasInitEmitterCallback = (err?: Error | string | null) => void;
	export type OnK2hdkcCasGetEmitterCallback = (err?: Error | string | null, value?: number) => void;
	export type OnK2hdkcCasSetEmitterCallback = (err?: Error | string | null) => void;
	export type OnK2hdkcCasIncDecEmitterCallback = (err?: Error | string | null) => void;

	//---------------------------------------------------------
	// K2hdkcNode Class
	//---------------------------------------------------------
	export class K2hdkcNode
	{
		// Constructor
		constructor();	// always no arguments

		//-----------------------------------------------------
		// Methods (Callback can be called)
		//-----------------------------------------------------
		// initialize on server
		init(conf: string, cb?: K2hdkcInitCallback): boolean;
		init(conf: string, port: number, cb?: K2hdkcInitCallback): boolean;
		init(conf: string, port: number, cuk: string | null, cb?: K2hdkcInitCallback): boolean;
		init(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, cb?: K2hdkcInitCallback): boolean;
		init(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, cb?: K2hdkcInitCallback): boolean;

		// get
		getValue(key: string, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, key: string, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, key: string, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcGetValueCallback): boolean;

		getValue(key: string, subkey: string | null, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, key: string, subkey: string | null, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, key: string, subkey: string | null, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, key: string, subkey: string | null, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, subkey: string | null, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, subkey: string | null, cb: K2hdkcGetValueCallback): boolean;

		getValue(key: string, subkey: string | null, attrcheck: boolean, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, key: string, subkey: string | null, attrcheck: boolean, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, key: string, subkey: string | null, attrcheck: boolean, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, key: string, subkey: string | null, attrcheck: boolean, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, subkey: string | null, attrcheck: boolean, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, subkey: string | null, attrcheck: boolean, cb: K2hdkcGetValueCallback): boolean;

		getValue(key: string, subkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, key: string, subkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, key: string, subkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, key: string, subkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, subkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcGetValueCallback): boolean;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, subkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcGetValueCallback): boolean;

		// get subkeys
		getSubkeys(key: string, cb: K2hdkcGetSubkeysCallback): boolean;
		getSubkeys(conf: string, key: string, cb: K2hdkcGetSubkeysCallback): boolean;
		getSubkeys(conf: string, port: number, key: string, cb: K2hdkcGetSubkeysCallback): boolean;
		getSubkeys(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcGetSubkeysCallback): boolean;
		getSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcGetSubkeysCallback): boolean;
		getSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcGetSubkeysCallback): boolean;

		getSubkeys(key: string, attrcheck: boolean, cb: K2hdkcGetSubkeysCallback): boolean;
		getSubkeys(conf: string, key: string, attrcheck: boolean, cb: K2hdkcGetSubkeysCallback): boolean;
		getSubkeys(conf: string, port: number, key: string, attrcheck: boolean, cb: K2hdkcGetSubkeysCallback): boolean;
		getSubkeys(conf: string, port: number, cuk: string | null, key: string, attrcheck: boolean, cb: K2hdkcGetSubkeysCallback): boolean;
		getSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, attrcheck: boolean, cb: K2hdkcGetSubkeysCallback): boolean;
		getSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, attrcheck: boolean, cb: K2hdkcGetSubkeysCallback): boolean;

		// get attrs
		getAttrs(key: string, cb: K2hdkcGetAttrsCallback): boolean;
		getAttrs(conf: string, key: string, cb: K2hdkcGetAttrsCallback): boolean;
		getAttrs(conf: string, port: number, key: string, cb: K2hdkcGetAttrsCallback): boolean;
		getAttrs(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcGetAttrsCallback): boolean;
		getAttrs(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcGetAttrsCallback): boolean;
		getAttrs(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcGetAttrsCallback): boolean;

		// set value
		setValue(key: string, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, key: string, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, key: string, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcSetValueCallback): boolean;

		setValue(key: string, value: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, key: string, value: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, key: string, value: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, key: string, value: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, cb: K2hdkcSetValueCallback): boolean;

		setValue(key: string, value: string | null, subkey: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, key: string, value: string | null, subkey: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, key: string, value: string | null, subkey: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string | null, cb: K2hdkcSetValueCallback): boolean;

		setValue(key: string, value: string | null, subkey: string | null, pass: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, key: string, value: string | null, subkey: string | null, pass: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, key: string, value: string | null, subkey: string | null, pass: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string | null, pass: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string | null, pass: string | null, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string | null, pass: string | null, cb: K2hdkcSetValueCallback): boolean;

		setValue(key: string, value: string | null, subkey: string | null, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, key: string, value: string | null, subkey: string | null, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, key: string, value: string | null, subkey: string | null, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string | null, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string | null, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string | null, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;

		// set subkeys
		setSubkeys(key: string, cb: K2hdkcSetSubkeysCallback): boolean;
		setSubkeys(conf: string, key: string, cb: K2hdkcSetSubkeysCallback): boolean;
		setSubkeys(conf: string, port: number, key: string, cb: K2hdkcSetSubkeysCallback): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcSetSubkeysCallback): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcSetSubkeysCallback): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcSetSubkeysCallback): boolean;

		setSubkeys(key: string, subkeys: string[] | null, cb: K2hdkcSetSubkeysCallback): boolean;
		setSubkeys(conf: string, key: string, subkeys: string[] | null, cb: K2hdkcSetSubkeysCallback): boolean;
		setSubkeys(conf: string, port: number, key: string, subkeys: string[] | null, cb: K2hdkcSetSubkeysCallback): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, key: string, subkeys: string[] | null, cb: K2hdkcSetSubkeysCallback): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, subkeys: string[] | null, cb: K2hdkcSetSubkeysCallback): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, subkeys: string[] | null, cb: K2hdkcSetSubkeysCallback): boolean;

		// add subkey (NOTE: callback function type is K2hdkcSetValueCallback)
		addSubkey(key: string, value: string | null, subkey: string, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, key: string, value: string | null, subkey: string, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, key: string, value: string | null, subkey: string, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string, cb: K2hdkcSetValueCallback): boolean;

		addSubkey(key: string, value: string | null, subkey: string, pass: string | null, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, key: string, value: string | null, subkey: string, pass: string | null, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, key: string, value: string | null, subkey: string, pass: string | null, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string, pass: string | null, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string, pass: string | null, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string, pass: string | null, cb: K2hdkcSetValueCallback): boolean;

		addSubkey(key: string, value: string | null, subkey: string, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, key: string, value: string | null, subkey: string, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, key: string, value: string | null, subkey: string, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string, pass: string | null, expire: number, cb: K2hdkcSetValueCallback): boolean;

		// clear subkeys (NOTE: callback function type is K2hdkcSetSubkeysCallback)
		clearSubkeys(key: string, cb: K2hdkcSetSubkeysCallback): boolean;
		clearSubkeys(conf: string, key: string, cb: K2hdkcSetSubkeysCallback): boolean;
		clearSubkeys(conf: string, port: number, key: string, cb: K2hdkcSetSubkeysCallback): boolean;
		clearSubkeys(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcSetSubkeysCallback): boolean;
		clearSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcSetSubkeysCallback): boolean;
		clearSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcSetSubkeysCallback): boolean;

		// set all
		setAll(key: string, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, key: string, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, key: string, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcSetAllCallback): boolean;

		setAll(key: string, value: string | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, key: string, value: string | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, key: string, value: string | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, key: string, value: string | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, cb: K2hdkcSetAllCallback): boolean;

		setAll(key: string, value: string | null, subkeys: string[] | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, key: string, value: string | null, subkeys: string[] | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, key: string, value: string | null, subkeys: string[] | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkeys: string[] | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null, cb: K2hdkcSetAllCallback): boolean;

		setAll(key: string, value: string | null, subkeys: string[] | null, pass: string | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, key: string, value: string | null, subkeys: string[] | null, pass: string | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, key: string, value: string | null, subkeys: string[] | null, pass: string | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkeys: string[] | null, pass: string | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null, pass: string | null, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null, pass: string | null, cb: K2hdkcSetAllCallback): boolean;

		setAll(key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number, cb: K2hdkcSetAllCallback): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number, cb: K2hdkcSetAllCallback): boolean;

		// remove
		remove(key: string, cb: K2hdkcRemoveCallback): boolean;
		remove(conf: string, key: string, cb: K2hdkcRemoveCallback): boolean;
		remove(conf: string, port: number, key: string, cb: K2hdkcRemoveCallback): boolean;
		remove(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcRemoveCallback): boolean;
		remove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcRemoveCallback): boolean;
		remove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcRemoveCallback): boolean;

		remove(key: string, is_del_subkeys: boolean, cb: K2hdkcRemoveCallback): boolean;
		remove(conf: string, key: string, is_del_subkeys: boolean, cb: K2hdkcRemoveCallback): boolean;
		remove(conf: string, port: number, key: string, is_del_subkeys: boolean, cb: K2hdkcRemoveCallback): boolean;
		remove(conf: string, port: number, cuk: string | null, key: string, is_del_subkeys: boolean, cb: K2hdkcRemoveCallback): boolean;
		remove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, is_del_subkeys: boolean, cb: K2hdkcRemoveCallback): boolean;
		remove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, is_del_subkeys: boolean, cb: K2hdkcRemoveCallback): boolean;

		// rename
		rename(oldkey: string, newkey: string, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, oldkey: string, newkey: string, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, oldkey: string, newkey: string, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, oldkey: string, newkey: string, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, oldkey: string, newkey: string, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, oldkey: string, newkey: string, cb: K2hdkcRenameCallback): boolean;

		rename(oldkey: string, newkey: string, parentkey: string | null, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, oldkey: string, newkey: string, parentkey: string | null, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, oldkey: string, newkey: string, parentkey: string | null, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, oldkey: string, newkey: string, parentkey: string | null, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, cb: K2hdkcRenameCallback): boolean;

		rename(oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, cb: K2hdkcRenameCallback): boolean;

		rename(oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, cb: K2hdkcRenameCallback): boolean;

		rename(oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcRenameCallback): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcRenameCallback): boolean;

		// queue push
		queuePush(prefix: string, key: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, key: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, key: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, key: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, key: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, key: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, key: null, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, key: null, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, key: null, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: null, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: null, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: null, val: string | null, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, key: null, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, key: null, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, key: null, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: null, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, val: string | null, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, cb: K2hdkcQueuePushCallback): boolean;

		queuePush(prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number, cb: K2hdkcQueuePushCallback): boolean;

		// queue pop
		queuePop(prefix: string, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, prefix: string, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, prefix: string, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, prefix: string, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, cb: K2hdkcQueuePopCallback): boolean;

		queuePop(prefix: string, is_fifo: boolean, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, prefix: string, is_fifo: boolean, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, prefix: string, is_fifo: boolean, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, prefix: string, is_fifo: boolean, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, is_fifo: boolean, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, is_fifo: boolean, cb: K2hdkcQueuePopCallback): boolean;

		queuePop(prefix: string, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, prefix: string, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, prefix: string, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, prefix: string, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueuePopCallback): boolean;

		queuePop(prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueuePopCallback): boolean;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueuePopCallback): boolean;

		// queue remove
		queueRemove(prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;

		queueRemove(prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;

		queueRemove(prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;

		queueRemove(prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;

		queueRemove(prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;

		// queue del (NOTE: callback function type is K2hdkcQueueRemoveCallback)
		queueDel(prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, cb: K2hdkcQueueRemoveCallback): boolean;

		queueDel(prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, cb: K2hdkcQueueRemoveCallback): boolean;

		queueDel(prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, cb: K2hdkcQueueRemoveCallback): boolean;

		queueDel(prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, cb: K2hdkcQueueRemoveCallback): boolean;

		queueDel(prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null, cb: K2hdkcQueueRemoveCallback): boolean;

		// cas init
		casInit(key: string, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, key: string, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, key: string, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcCasInitCallback): boolean;

		casInit(key: string, value: number, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, key: string, value: number, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, key: string, value: number, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, key: string, value: number, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: number, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: number, cb: K2hdkcCasInitCallback): boolean;

		casInit(key: string, value: number, pass: string | null, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, key: string, value: number, pass: string | null, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, key: string, value: number, pass: string | null, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, key: string, value: number, pass: string | null, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: number, pass: string | null, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: number, pass: string | null, cb: K2hdkcCasInitCallback): boolean;

		casInit(key: string, value: number, pass: string | null, expire: number, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, key: string, value: number, pass: string | null, expire: number, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, key: string, value: number, pass: string | null, expire: number, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, key: string, value: number, pass: string | null, expire: number, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: number, pass: string | null, expire: number, cb: K2hdkcCasInitCallback): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: number, pass: string | null, expire: number, cb: K2hdkcCasInitCallback): boolean;

		// cas get
		casGet(key: string, cb: K2hdkcCasGetCallback): boolean;
		casGet(conf: string, key: string, cb: K2hdkcCasGetCallback): boolean;
		casGet(conf: string, port: number, key: string, cb: K2hdkcCasGetCallback): boolean;
		casGet(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcCasGetCallback): boolean;
		casGet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcCasGetCallback): boolean;
		casGet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcCasGetCallback): boolean;

		casGet(key: string, pass: string | null, cb: K2hdkcCasGetCallback): boolean;
		casGet(conf: string, key: string, pass: string | null, cb: K2hdkcCasGetCallback): boolean;
		casGet(conf: string, port: number, key: string, pass: string | null, cb: K2hdkcCasGetCallback): boolean;
		casGet(conf: string, port: number, cuk: string | null, key: string, pass: string | null, cb: K2hdkcCasGetCallback): boolean;
		casGet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, pass: string | null, cb: K2hdkcCasGetCallback): boolean;
		casGet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, pass: string | null, cb: K2hdkcCasGetCallback): boolean;

		// cas set
		casSet(key: string, oldval: number, newval: number, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, key: string, oldval: number, newval: number, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, key: string, oldval: number, newval: number, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, cuk: string | null, key: string, oldval: number, newval: number, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, oldval: number, newval: number, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, oldval: number, newval: number, cb: K2hdkcCasSetCallback): boolean;

		casSet(key: string, oldval: number, newval: number, pass: string | null, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, key: string, oldval: number, newval: number, pass: string | null, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, key: string, oldval: number, newval: number, pass: string | null, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, cuk: string | null, key: string, oldval: number, newval: number, pass: string | null, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, oldval: number, newval: number, pass: string | null, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, oldval: number, newval: number, pass: string | null, cb: K2hdkcCasSetCallback): boolean;

		casSet(key: string, oldval: number, newval: number, pass: string | null, expire: number, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, key: string, oldval: number, newval: number, pass: string | null, expire: number, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, key: string, oldval: number, newval: number, pass: string | null, expire: number, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, cuk: string | null, key: string, oldval: number, newval: number, pass: string | null, expire: number, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, oldval: number, newval: number, pass: string | null, expire: number, cb: K2hdkcCasSetCallback): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, oldval: number, newval: number, pass: string | null, expire: number, cb: K2hdkcCasSetCallback): boolean;

		// cas inc/dec
		casIncDec(key: string, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, key: string, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, key: string, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, key: string, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, cb: K2hdkcCasIncDecCallback): boolean;

		casIncDec(key: string, is_increment: boolean, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, key: string, is_increment: boolean, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, key: string, is_increment: boolean, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, key: string, is_increment: boolean, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, is_increment: boolean, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, is_increment: boolean, cb: K2hdkcCasIncDecCallback): boolean;

		casIncDec(key: string, is_increment: boolean, pass: string | null, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, key: string, is_increment: boolean, pass: string | null, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, key: string, is_increment: boolean, pass: string | null, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, key: string, is_increment: boolean, pass: string | null, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, is_increment: boolean, pass: string | null, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, is_increment: boolean, pass: string | null, cb: K2hdkcCasIncDecCallback): boolean;

		casIncDec(key: string, is_increment: boolean, pass: string | null, expire: number, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, key: string, is_increment: boolean, pass: string | null, expire: number, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, key: string, is_increment: boolean, pass: string | null, expire: number, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, key: string, is_increment: boolean, pass: string | null, expire: number, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, is_increment: boolean, pass: string | null, expire: number, cb: K2hdkcCasIncDecCallback): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, is_increment: boolean, pass: string | null, expire: number, cb: K2hdkcCasIncDecCallback): boolean;

		//-----------------------------------------------------
		// Methods (no callback)
		//-----------------------------------------------------
		// clean/clear
		clean(): boolean;
		clear(): boolean;

		// check
		isPermanent(): boolean;

		// get
		getValue(key: string): string | null;
		getValue(conf: string, key: string): string | null;
		getValue(conf: string, port: number, key: string): string | null;
		getValue(conf: string, port: number, cuk: string | null, key: string): string | null;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string): string | null;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string): string | null;

		getValue(key: string, subkey: string | null): string | null;
		getValue(conf: string, key: string, subkey: string | null): string | null;
		getValue(conf: string, port: number, key: string, subkey: string | null): string | null;
		getValue(conf: string, port: number, cuk: string | null, key: string, subkey: string | null): string | null;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, subkey: string | null): string | null;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, subkey: string | null): string | null;

		getValue(key: string, subkey: string | null, attrcheck: boolean): string | null;
		getValue(conf: string, key: string, subkey: string | null, attrcheck: boolean): string | null;
		getValue(conf: string, port: number, key: string, subkey: string | null, attrcheck: boolean): string | null;
		getValue(conf: string, port: number, cuk: string | null, key: string, subkey: string | null, attrcheck: boolean): string | null;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, subkey: string | null, attrcheck: boolean): string | null;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, subkey: string | null, attrcheck: boolean): string | null;

		getValue(key: string, subkey: string | null, attrcheck: boolean, pass: string | null): string | null;
		getValue(conf: string, key: string, subkey: string | null, attrcheck: boolean, pass: string | null): string | null;
		getValue(conf: string, port: number, key: string, subkey: string | null, attrcheck: boolean, pass: string | null): string | null;
		getValue(conf: string, port: number, cuk: string | null, key: string, subkey: string | null, attrcheck: boolean, pass: string | null): string | null;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, subkey: string | null, attrcheck: boolean, pass: string | null): string | null;
		getValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, subkey: string | null, attrcheck: boolean, pass: string | null): string | null;

		// get subkeys
		getSubkeys(key: string): string[] | null;
		getSubkeys(conf: string, key: string): string[] | null;
		getSubkeys(conf: string, port: number, key: string): string[] | null;
		getSubkeys(conf: string, port: number, cuk: string | null, key: string): string[] | null;
		getSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string): string[] | null;
		getSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string): string[] | null;

		getSubkeys(key: string, attrcheck: boolean): string[] | null;
		getSubkeys(conf: string, key: string, attrcheck: boolean): string[] | null;
		getSubkeys(conf: string, port: number, key: string, attrcheck: boolean): string[] | null;
		getSubkeys(conf: string, port: number, cuk: string | null, key: string, attrcheck: boolean): string[] | null;
		getSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, attrcheck: boolean): string[] | null;
		getSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, attrcheck: boolean): string[] | null;

		// get attrs
		getAttrs(key: string): string[] | null;
		getAttrs(conf: string, key: string): string[] | null;
		getAttrs(conf: string, port: number, key: string): string[] | null;
		getAttrs(conf: string, port: number, cuk: string | null, key: string): string[] | null;
		getAttrs(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string): string[] | null;
		getAttrs(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string): string[] | null;

		// set value
		setValue(key: string): boolean;
		setValue(conf: string, key: string): boolean;
		setValue(conf: string, port: number, key: string): boolean;
		setValue(conf: string, port: number, cuk: string | null, key: string): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string): boolean;

		setValue(key: string, value: string | null): boolean;
		setValue(conf: string, key: string, value: string | null): boolean;
		setValue(conf: string, port: number, key: string, value: string | null): boolean;
		setValue(conf: string, port: number, cuk: string | null, key: string, value: string | null): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null): boolean;

		setValue(key: string, value: string | null, subkey: string | null): boolean;
		setValue(conf: string, key: string, value: string | null, subkey: string | null): boolean;
		setValue(conf: string, port: number, key: string, value: string | null, subkey: string | null): boolean;
		setValue(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string | null): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string | null): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string | null): boolean;

		setValue(key: string, value: string | null, subkey: string | null, pass: string | null): boolean;
		setValue(conf: string, key: string, value: string | null, subkey: string | null, pass: string | null): boolean;
		setValue(conf: string, port: number, key: string, value: string | null, subkey: string | null, pass: string | null): boolean;
		setValue(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string | null, pass: string | null): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string | null, pass: string | null): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string | null, pass: string | null): boolean;

		setValue(key: string, value: string | null, subkey: string | null, pass: string | null, expire: number): boolean;
		setValue(conf: string, key: string, value: string | null, subkey: string | null, pass: string | null, expire: number): boolean;
		setValue(conf: string, port: number, key: string, value: string | null, subkey: string | null, pass: string | null, expire: number): boolean;
		setValue(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string | null, pass: string | null, expire: number): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string | null, pass: string | null, expire: number): boolean;
		setValue(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string | null, pass: string | null, expire: number): boolean;

		// set subkeys
		setSubkeys(key: string): boolean;
		setSubkeys(conf: string, key: string): boolean;
		setSubkeys(conf: string, port: number, key: string): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, key: string): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string): boolean;

		setSubkeys(key: string, subkeys: string[] | null): boolean;
		setSubkeys(conf: string, key: string, subkeys: string[] | null): boolean;
		setSubkeys(conf: string, port: number, key: string, subkeys: string[] | null): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, key: string, subkeys: string[] | null): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, subkeys: string[] | null): boolean;
		setSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, subkeys: string[] | null): boolean;

		// add subkey
		addSubkey(key: string, value: string | null, subkey: string): boolean;
		addSubkey(conf: string, key: string, value: string | null, subkey: string): boolean;
		addSubkey(conf: string, port: number, key: string, value: string | null, subkey: string): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string): boolean;

		addSubkey(key: string, value: string | null, subkey: string, pass: string | null): boolean;
		addSubkey(conf: string, key: string, value: string | null, subkey: string, pass: string | null): boolean;
		addSubkey(conf: string, port: number, key: string, value: string | null, subkey: string, pass: string | null): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string, pass: string | null): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string, pass: string | null): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string, pass: string | null): boolean;

		addSubkey(key: string, value: string | null, subkey: string, pass: string | null, expire: number): boolean;
		addSubkey(conf: string, key: string, value: string | null, subkey: string, pass: string | null, expire: number): boolean;
		addSubkey(conf: string, port: number, key: string, value: string | null, subkey: string, pass: string | null, expire: number): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkey: string, pass: string | null, expire: number): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkey: string, pass: string | null, expire: number): boolean;
		addSubkey(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkey: string, pass: string | null, expire: number): boolean;

		// clear subkeys
		clearSubkeys(key: string, subkeys: string[] | null): boolean;
		clearSubkeys(conf: string, key: string, subkeys: string[] | null): boolean;
		clearSubkeys(conf: string, port: number, key: string, subkeys: string[] | null): boolean;
		clearSubkeys(conf: string, port: number, cuk: string | null, key: string, subkeys: string[] | null): boolean;
		clearSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, subkeys: string[] | null): boolean;
		clearSubkeys(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, subkeys: string[] | null): boolean;

		// set all
		setAll(key: string): boolean;
		setAll(conf: string, key: string): boolean;
		setAll(conf: string, port: number, key: string): boolean;
		setAll(conf: string, port: number, cuk: string | null, key: string): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string): boolean;

		setAll(key: string, value: string | null): boolean;
		setAll(conf: string, key: string, value: string | null): boolean;
		setAll(conf: string, port: number, key: string, value: string | null): boolean;
		setAll(conf: string, port: number, cuk: string | null, key: string, value: string | null): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null): boolean;

		setAll(key: string, value: string | null, subkeys: string[] | null): boolean;
		setAll(conf: string, key: string, value: string | null, subkeys: string[] | null): boolean;
		setAll(conf: string, port: number, key: string, value: string | null, subkeys: string[] | null): boolean;
		setAll(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkeys: string[] | null): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null): boolean;

		setAll(key: string, value: string | null, subkeys: string[] | null, pass: string | null): boolean;
		setAll(conf: string, key: string, value: string | null, subkeys: string[] | null, pass: string | null): boolean;
		setAll(conf: string, port: number, key: string, value: string | null, subkeys: string[] | null, pass: string | null): boolean;
		setAll(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkeys: string[] | null, pass: string | null): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null, pass: string | null): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null, pass: string | null): boolean;

		setAll(key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number): boolean;
		setAll(conf: string, key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number): boolean;
		setAll(conf: string, port: number, key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number): boolean;
		setAll(conf: string, port: number, cuk: string | null, key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number): boolean;
		setAll(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: string | null, subkeys: string[] | null, pass: string | null, expire: number): boolean;

		// remove
		remove(key: string): boolean;
		remove(conf: string, key: string): boolean;
		remove(conf: string, port: number, key: string): boolean;
		remove(conf: string, port: number, cuk: string | null, key: string): boolean;
		remove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string): boolean;
		remove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string): boolean;

		remove(key: string, is_del_subkeys: boolean): boolean;
		remove(conf: string, key: string, is_del_subkeys: boolean): boolean;
		remove(conf: string, port: number, key: string, is_del_subkeys: boolean): boolean;
		remove(conf: string, port: number, cuk: string | null, key: string, is_del_subkeys: boolean): boolean;
		remove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, is_del_subkeys: boolean): boolean;
		remove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, is_del_subkeys: boolean): boolean;

		// rename
		rename(oldkey: string, newkey: string): boolean;
		rename(conf: string, oldkey: string, newkey: string): boolean;
		rename(conf: string, port: number, oldkey: string, newkey: string): boolean;
		rename(conf: string, port: number, cuk: string | null, oldkey: string, newkey: string): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, oldkey: string, newkey: string): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, oldkey: string, newkey: string): boolean;

		rename(oldkey: string, newkey: string, parentkey: string | null): boolean;
		rename(conf: string, oldkey: string, newkey: string, parentkey: string | null): boolean;
		rename(conf: string, port: number, oldkey: string, newkey: string, parentkey: string | null): boolean;
		rename(conf: string, port: number, cuk: string | null, oldkey: string, newkey: string, parentkey: string | null): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null): boolean;

		rename(oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean): boolean;
		rename(conf: string, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean): boolean;
		rename(conf: string, port: number, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean): boolean;
		rename(conf: string, port: number, cuk: string | null, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean): boolean;

		rename(oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null): boolean;
		rename(conf: string, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null): boolean;
		rename(conf: string, port: number, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null): boolean;
		rename(conf: string, port: number, cuk: string | null, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null): boolean;

		rename(oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number): boolean;
		rename(conf: string, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number): boolean;
		rename(conf: string, port: number, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number): boolean;
		rename(conf: string, port: number, cuk: string | null, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number): boolean;
		rename(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, oldkey: string, newkey: string, parentkey: string | null, attrcheck: boolean, pass: string | null, expire: number): boolean;

		// queue push
		queuePush(prefix: string, key: string, val: string | null): boolean;
		queuePush(conf: string, prefix: string, key: string, val: string | null): boolean;
		queuePush(conf: string, port: number, prefix: string, key: string, val: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: string, val: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: string, val: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: string, val: string | null): boolean;

		queuePush(prefix: string, key: string, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, prefix: string, key: string, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, prefix: string, key: string, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: string, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean): boolean;

		queuePush(prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;

		queuePush(prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;

		queuePush(prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;

		queuePush(prefix: string, key: null, val: string | null): boolean;
		queuePush(conf: string, prefix: string, key: null, val: string | null): boolean;
		queuePush(conf: string, port: number, prefix: string, key: null, val: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: null, val: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: null, val: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: null, val: string | null): boolean;

		queuePush(prefix: string, key: null, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, prefix: string, key: null, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, prefix: string, key: null, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: null, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean): boolean;

		queuePush(prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;

		queuePush(prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;

		queuePush(prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, key: null, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;

		queuePush(prefix: string, val: string | null): boolean;
		queuePush(conf: string, prefix: string, val: string | null): boolean;
		queuePush(conf: string, port: number, prefix: string, val: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, val: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, val: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, val: string | null): boolean;

		queuePush(prefix: string, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, prefix: string, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, prefix: string, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean): boolean;

		queuePush(prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean): boolean;

		queuePush(prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null): boolean;

		queuePush(prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, cuk: string | null, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;
		queuePush(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, val: string | null, is_fifo: boolean, attrcheck: boolean, pass: string | null, expire: number): boolean;

		// queue pop
		queuePop(prefix: string): [string, string | null] | string | null;
		queuePop(conf: string, prefix: string): [string, string | null] | string | null;
		queuePop(conf: string, port: number, prefix: string): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, prefix: string): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string): [string, string | null] | string | null;

		queuePop(prefix: string, is_fifo: boolean): [string, string | null] | string | null;
		queuePop(conf: string, prefix: string, is_fifo: boolean): [string, string | null] | string | null;
		queuePop(conf: string, port: number, prefix: string, is_fifo: boolean): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, prefix: string, is_fifo: boolean): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, is_fifo: boolean): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, is_fifo: boolean): [string, string | null] | string | null;

		queuePop(prefix: string, is_fifo: boolean, is_key_queue: boolean): [string, string | null] | string | null;
		queuePop(conf: string, prefix: string, is_fifo: boolean, is_key_queue: boolean): [string, string | null] | string | null;
		queuePop(conf: string, port: number, prefix: string, is_fifo: boolean, is_key_queue: boolean): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, prefix: string, is_fifo: boolean, is_key_queue: boolean): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, is_fifo: boolean, is_key_queue: boolean): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, is_fifo: boolean, is_key_queue: boolean): [string, string | null] | string | null;

		queuePop(prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null): [string, string | null] | string | null;
		queuePop(conf: string, prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null): [string, string | null] | string | null;
		queuePop(conf: string, port: number, prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null): [string, string | null] | string | null;
		queuePop(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, is_fifo: boolean, is_key_queue: boolean, pass: string | null): [string, string | null] | string | null;

		// queue remove
		queueRemove(prefix: string): boolean;
		queueRemove(conf: string, prefix: string): boolean;
		queueRemove(conf: string, port: number, prefix: string): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, prefix: string): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string): boolean;

		queueRemove(prefix: string, count: number): boolean;
		queueRemove(conf: string, prefix: string, count: number): boolean;
		queueRemove(conf: string, port: number, prefix: string, count: number): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, prefix: string, count: number): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number): boolean;

		queueRemove(prefix: string, count: number, is_fifo: boolean): boolean;
		queueRemove(conf: string, prefix: string, count: number, is_fifo: boolean): boolean;
		queueRemove(conf: string, port: number, prefix: string, count: number, is_fifo: boolean): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean): boolean;

		queueRemove(prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;
		queueRemove(conf: string, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;
		queueRemove(conf: string, port: number, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;

		queueRemove(prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;
		queueRemove(conf: string, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;
		queueRemove(conf: string, port: number, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;
		queueRemove(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;

		// queue del
		queueDel(prefix: string): boolean;
		queueDel(conf: string, prefix: string): boolean;
		queueDel(conf: string, port: number, prefix: string): boolean;
		queueDel(conf: string, port: number, cuk: string | null, prefix: string): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string): boolean;

		queueDel(prefix: string, count: number): boolean;
		queueDel(conf: string, prefix: string, count: number): boolean;
		queueDel(conf: string, port: number, prefix: string, count: number): boolean;
		queueDel(conf: string, port: number, cuk: string | null, prefix: string, count: number): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number): boolean;

		queueDel(prefix: string, count: number, is_fifo: boolean): boolean;
		queueDel(conf: string, prefix: string, count: number, is_fifo: boolean): boolean;
		queueDel(conf: string, port: number, prefix: string, count: number, is_fifo: boolean): boolean;
		queueDel(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean): boolean;

		queueDel(prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;
		queueDel(conf: string, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;
		queueDel(conf: string, port: number, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;
		queueDel(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean): boolean;

		queueDel(prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;
		queueDel(conf: string, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;
		queueDel(conf: string, port: number, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;
		queueDel(conf: string, port: number, cuk: string | null, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;
		queueDel(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, prefix: string, count: number, is_fifo: boolean, is_key_queue: boolean, pass: string | null): boolean;

		// cas init
		casInit(key: string): boolean;
		casInit(conf: string, key: string): boolean;
		casInit(conf: string, port: number, key: string): boolean;
		casInit(conf: string, port: number, cuk: string | null, key: string): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string): boolean;

		casInit(key: string, value: number): boolean;
		casInit(conf: string, key: string, value: number): boolean;
		casInit(conf: string, port: number, key: string, value: number): boolean;
		casInit(conf: string, port: number, cuk: string | null, key: string, value: number): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: number): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: number): boolean;

		casInit(key: string, value: number, pass: string | null): boolean;
		casInit(conf: string, key: string, value: number, pass: string | null): boolean;
		casInit(conf: string, port: number, key: string, value: number, pass: string | null): boolean;
		casInit(conf: string, port: number, cuk: string | null, key: string, value: number, pass: string | null): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: number, pass: string | null): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: number, pass: string | null): boolean;

		casInit(key: string, value: number, pass: string | null, expire: number): boolean;
		casInit(conf: string, key: string, value: number, pass: string | null, expire: number): boolean;
		casInit(conf: string, port: number, key: string, value: number, pass: string | null, expire: number): boolean;
		casInit(conf: string, port: number, cuk: string | null, key: string, value: number, pass: string | null, expire: number): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, value: number, pass: string | null, expire: number): boolean;
		casInit(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, value: number, pass: string | null, expire: number): boolean;

		// cas get
		casGet(key: string): number | undefined;
		casGet(conf: string, key: string): number | undefined;
		casGet(conf: string, port: number, key: string): number | undefined;
		casGet(conf: string, port: number, cuk: string | null, key: string): number | undefined;
		casGet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string): number | undefined;
		casGet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string): number | undefined;

		casGet(key: string, pass: string | null): number | undefined;
		casGet(conf: string, key: string, pass: string | null): number | undefined;
		casGet(conf: string, port: number, key: string, pass: string | null): number | undefined;
		casGet(conf: string, port: number, cuk: string | null, key: string, pass: string | null): number | undefined;
		casGet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, pass: string | null): number | undefined;
		casGet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, pass: string | null): number | undefined;

		// cas set
		casSet(key: string, oldval: number, newval: number): boolean;
		casSet(conf: string, key: string, oldval: number, newval: number): boolean;
		casSet(conf: string, port: number, key: string, oldval: number, newval: number): boolean;
		casSet(conf: string, port: number, cuk: string | null, key: string, oldval: number, newval: number): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, oldval: number, newval: number): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, oldval: number, newval: number): boolean;

		casSet(key: string, oldval: number, newval: number, pass: string | null): boolean;
		casSet(conf: string, key: string, oldval: number, newval: number, pass: string | null): boolean;
		casSet(conf: string, port: number, key: string, oldval: number, newval: number, pass: string | null): boolean;
		casSet(conf: string, port: number, cuk: string | null, key: string, oldval: number, newval: number, pass: string | null): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, oldval: number, newval: number, pass: string | null): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, oldval: number, newval: number, pass: string | null): boolean;

		casSet(key: string, oldval: number, newval: number, pass: string | null, expire: number): boolean;
		casSet(conf: string, key: string, oldval: number, newval: number, pass: string | null, expire: number): boolean;
		casSet(conf: string, port: number, key: string, oldval: number, newval: number, pass: string | null, expire: number): boolean;
		casSet(conf: string, port: number, cuk: string | null, key: string, oldval: number, newval: number, pass: string | null, expire: number): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, oldval: number, newval: number, pass: string | null, expire: number): boolean;
		casSet(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, oldval: number, newval: number, pass: string | null, expire: number): boolean;

		// cas inc/dec
		casIncDec(key: string): boolean;
		casIncDec(conf: string, key: string): boolean;
		casIncDec(conf: string, port: number, key: string): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, key: string): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string): boolean;

		casIncDec(key: string, is_increment: boolean): boolean;
		casIncDec(conf: string, key: string, is_increment: boolean): boolean;
		casIncDec(conf: string, port: number, key: string, is_increment: boolean): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, key: string, is_increment: boolean): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, is_increment: boolean): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, is_increment: boolean): boolean;

		casIncDec(key: string, is_increment: boolean, pass: string | null): boolean;
		casIncDec(conf: string, key: string, is_increment: boolean, pass: string | null): boolean;
		casIncDec(conf: string, port: number, key: string, is_increment: boolean, pass: string | null): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, key: string, is_increment: boolean, pass: string | null): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, is_increment: boolean, pass: string | null): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, is_increment: boolean, pass: string | null): boolean;

		casIncDec(key: string, is_increment: boolean, pass: string | null, expire: number): boolean;
		casIncDec(conf: string, key: string, is_increment: boolean, pass: string | null, expire: number): boolean;
		casIncDec(conf: string, port: number, key: string, is_increment: boolean, pass: string | null, expire: number): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, key: string, is_increment: boolean, pass: string | null, expire: number): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, key: string, is_increment: boolean, pass: string | null, expire: number): boolean;
		casIncDec(conf: string, port: number, cuk: string | null, auto_rejoin: boolean, no_giveup_rejoin: boolean, key: string, is_increment: boolean, pass: string | null, expire: number): boolean;

		// version
		printVersion(fd?: number): boolean;

		//-----------------------------------------------------
		// Emitter registration/unregistration
		//-----------------------------------------------------
		on(emitter: string, cb: OnK2hdkcEmitterCallback): boolean;
		onInit(cb: OnK2hdkcInitEmitterCallback): boolean;
		onGet(cb: OnK2hdkcGetValueEmitterCallback): boolean;
		onGetSubkeys(cb: OnK2hdkcGetSubkeysEmitterCallback): boolean;
		onGetAttrs(cb: OnK2hdkcGetAttrsEmitterCallback): boolean;
		onSet(cb: OnK2hdkcSetValueEmitterCallback): boolean;
		onSetSubkeys(cb: OnK2hdkcSetSubkeysEmitterCallback): boolean;
		onSetAll(cb: OnK2hdkcSetAllEmitterCallback): boolean;
		onRemove(cb: OnK2hdkcRemoveEmitterCallback): boolean;
		onRename(cb: OnK2hdkcRenameEmitterCallback): boolean;
		onQueuePush(cb: OnK2hdkcQueuePushEmitterCallback): boolean;
		onQueuePop(cb: OnK2hdkcQueuePopEmitterCallback): boolean;
		onQueueRemove(cb: OnK2hdkcQueueRemoveEmitterCallback): boolean;
		onQueueDel(cb: OnK2hdkcQueueRemoveEmitterCallback): boolean;		// NOTE: callback is OnK2hdkcQueueRemoveEmitterCallback
		onCasInit(cb: OnK2hdkcCasInitEmitterCallback): boolean;
		onCasGet(cb: OnK2hdkcCasGetEmitterCallback): boolean;
		onCasSet(cb: OnK2hdkcCasSetEmitterCallback): boolean;
		onCasIncDec(cb: OnK2hdkcCasIncDecEmitterCallback): boolean;

		off(emitter: string): boolean;
		offInit(): boolean;
		offGet(): boolean;
		offGetSubkeys(): boolean;
		offGetAttrs(): boolean;
		offSet(): boolean;
		offSetSubkeys(): boolean;
		offSetAll(): boolean;
		offRemove(): boolean;
		offRename(): boolean;
		offQueuePush(): boolean;
		offQueuePop(): boolean;
		offQueueRemove(): boolean;
		offQueueDel(): boolean;
		offCasInit(): boolean;
		offCasGet(): boolean;
		offCasSet(): boolean;
		offCasIncDec(): boolean;

		//-----------------------------------------------------
		// Promise APIs(Currently no imprelemnts)
		//-----------------------------------------------------
		// [NOTE]
		// Although it is not currently implemented, here is an example definition:
		//
		// ex.	getValue(conf: string, key: string): Promise<boolean>;
		//
	}

	//---------------------------------------------------------
	// K2hdkcFactoryType
	//---------------------------------------------------------
	export type K2hdkcFactoryType = {
		():			K2hdkcNode;
		new():		K2hdkcNode;
		K2hdkcNode:	typeof K2hdkcNode;
	};
} // end namespace k2hdkc

//-------------------------------------------------------------
// Exports
//-------------------------------------------------------------
//
// UMD global name
//
export as namespace k2hdkc;

//
// CommonJS default export
//
export = k2hdkc;

//
// Ambient module
//
// For "import", "import type" users(type-only export).
// This provides named type exports(type-only).
//
declare module 'k2hdkc'
{
	//
	// Default(value-level default import with esModuleInterop)
	//
	const _default: typeof k2hdkc;
	export default _default;

	//
	// Type named exports
	//
	// [NOTE]
	// ex. "import type { K2hdkcNode } from 'k2hdkc'"
	//
	export type K2hdkcNode			= k2hdkc.K2hdkcNode;
	export type K2hdkcFactoryType	= k2hdkc.K2hdkcFactoryType;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
