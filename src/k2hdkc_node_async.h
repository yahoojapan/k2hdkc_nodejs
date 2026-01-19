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

#ifndef K2HDKC_NODE_AYNC_H
#define K2HDKC_NODE_AYNC_H

#include "k2hdkc_common.h"

//
// AsyncWorker classes for using K2hdkcNode
//

//---------------------------------------------------------
// InitWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class InitWorker : public Napi::AsyncWorker
{
	public:
		InitWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin)
		{
			_callbackRef.Ref();
		}

		~InitWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(_conf.empty()){
				SetError("No configuration is associated to async worker");
				return;
			}

			// build permanent connection object
			if(_slaveobj.Initialize(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin)){
				if(!_slaveobj.Open(_no_giveup_rejoin)){
					// set error
					SetError("Failed to open(join to) chmpx slave.");
				}
			}else{
				// set error
				SetError("Failed to initialize k2hdkcslave object.");
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;
};

//---------------------------------------------------------
// GetValueWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, const std::string& subkey, bool is_check_attr, const std::string& pass)
// Callback function:	function(string error[, string value])
//
//---------------------------------------------------------
class GetValueWorker : public Napi::AsyncWorker
{
	public:
		GetValueWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, const std::string& subkey, bool is_check_attr, const std::string& pass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key), _strsubkey(subkey), _attrchk(is_check_attr), _strpass(pass), _is_result_set(false), _strresult("")
		{
			_callbackRef.Ref();
		}

		~GetValueWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// work
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!_strsubkey.empty()){
				// subkey is specified, thus need to check the key has it.
				K2hdkcComGetSubkeys*	pSubComObj;
				if(!_slaveobj.GetChmCntrlObject()){
					pSubComObj = GetOtSlaveK2hdkcComGetSubkeys(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
				}else{
					pSubComObj = GetPmSlaveK2hdkcComGetSubkeys(&_slaveobj);
				}
				if(!pSubComObj){
					SetError("Internal error: Could not create command object.");
					return;
				}
				// get subkey list in key
				K2HSubKeys*	pSubKeys= NULL;
				if(!pSubComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, _attrchk, &pSubKeys, &rescode) || !pSubKeys){
					// key does not have any subkey
					DKC_DELETE(pSubComObj);
					return;
				}
				DKC_DELETE(pSubComObj);

				// convert subkey binary data to string array
				strarr_t	strarr;
				pSubKeys->StringArray(strarr);
				DKC_DELETE(pSubKeys);

				// check subkey
				bool	found = false;
				for(strarr_t::const_iterator iter = strarr.begin(); iter != strarr.end(); ++iter){
					if(0 == strcmp(iter->c_str(), _strsubkey.c_str())){
						found = true;
						break;
					}
				}
				if(!found){
					// not found subkey in key
					return;
				}
				// switch key to subkey
				_strkey = _strsubkey;
			}

			// get value
			K2hdkcComGet*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComGet(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComGet(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			const unsigned char*	pvaltmp		= NULL;
			size_t					valtmplen	= 0L;
			bool					result		= pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, _attrchk, (_strpass.empty() ? NULL : _strpass.c_str()), &pvaltmp, &valtmplen, &rescode);
			if(result && (pvaltmp && 0 < valtmplen)){
				_strresult		= std::string(reinterpret_cast<const char*>(pvaltmp), ('\0' == pvaltmp[valtmplen - 1] ? valtmplen - 1 : valtmplen));
				_is_result_set	= true;
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				if(_is_result_set){
					Napi::String	valBuf = Napi::String::New(env, _strresult.c_str(), _strresult.length());
					_callbackRef.Value().Call({ env.Null(), valBuf });
				}else{
					_callbackRef.Value().Call({ env.Null(), env.Null() });
				}
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strkey;
		std::string				_strsubkey;
		bool					_attrchk;
		std::string				_strpass;

		bool					_is_result_set;
		std::string				_strresult;
};

//---------------------------------------------------------
// GetSubkeysWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, bool is_check_attr)
// Callback function:	function(string error, array subkeys)
//
//---------------------------------------------------------
class GetSubkeysWorker : public Napi::AsyncWorker
{
	public:
		GetSubkeysWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, bool is_check_attr) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key), _attrchk(is_check_attr)
		{
			_callbackRef.Ref();
		}

		~GetSubkeysWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// get command object
			K2hdkcComGetSubkeys*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComGetSubkeys(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComGetSubkeys(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			// get subkey list in key
			dkcres_type_t	rescode = DKC_NORESTYPE;
			K2HSubKeys*		pSubKeys= NULL;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, _attrchk, &pSubKeys, &rescode) || !pSubKeys){
				// key does not have any subkey
				SetError("Failed to get subkey because the key does not have any subkey.");
			}else{
				// convert subkey binary data to string array
				subkey_array.clear();
				pSubKeys->StringArray(subkey_array);
				DKC_DELETE(pSubKeys);

				if(0 == subkey_array.size()){
					SetError("Failed to get subkey because the key does not have any subkey.");
				}
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			Napi::Array retarr	= Napi::Array::New(env, subkey_array.size());
			uint32_t	pos		= 0;
			for(const auto &str: subkey_array){
				std::string	strtmp(str);
				while(!strtmp.empty()){
					unsigned char tmpch = static_cast<unsigned char>(strtmp.back());
					if('\0' != tmpch && !std::isspace(tmpch)){
						break;
					}
					strtmp.pop_back();
				}
				retarr.Set(pos++, Napi::String::New(env, strtmp));
			}

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null(), retarr });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strkey;
		bool					_attrchk;

		strarr_t				subkey_array;
};

//---------------------------------------------------------
// GetAttrsWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key)
// Callback function:	function(string error, array attribute_keys)
//
//---------------------------------------------------------
class GetAttrsWorker : public Napi::AsyncWorker
{
	public:
		GetAttrsWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key)
		{
			_callbackRef.Ref();
		}

		~GetAttrsWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// get command object
			K2hdkcComGetAttrs*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComGetAttrs(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComGetAttrs(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			// get attribute list in key
			dkcres_type_t	rescode = DKC_NORESTYPE;
			K2HAttrs*		pAttrs	= NULL;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, &pAttrs, &rescode) || !pAttrs){
				// key does not have any attribute key
				SetError("Failed to get attribute keys because the key does not have any attribute.");
			}else{
				// convert attribute key binary data to string array
				attrs_array.clear();
				pAttrs->KeyStringArray(attrs_array);
				DKC_DELETE(pAttrs);

				if(0 == attrs_array.size()){
					SetError("Failed to get attribute keys because the key does not have any attribute.");
				}
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			Napi::Array retarr	= Napi::Array::New(env, attrs_array.size());
			uint32_t	pos		= 0;
			for(const auto &str: attrs_array){
				std::string	strtmp(str);
				while(!strtmp.empty()){
					unsigned char tmpch = static_cast<unsigned char>(strtmp.back());
					if('\0' != tmpch && !std::isspace(tmpch)){
						break;
					}
					strtmp.pop_back();
				}
				retarr.Set(pos++, Napi::String::New(env, strtmp));
			}

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null(), retarr });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;
		std::string				_strkey;

		strarr_t				attrs_array;
};

//---------------------------------------------------------
// SetValueWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, const std::string& val, const std::string& subkey, const std::string& pass, const time_t input_expire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class SetValueWorker : public Napi::AsyncWorker
{
	public:
		SetValueWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, const std::string& val, const std::string& subkey, const std::string& pass, const time_t input_expire) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key), _strval(val), _strsubkey(subkey), _strpass(pass), _expire(input_expire)
		{
			_callbackRef.Ref();
		}

		~SetValueWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// work
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!_strsubkey.empty()){
				// subkey is specified, set value into subkey
				K2hdkcComAddSubkey*	pComObj;
				if(!_slaveobj.GetChmCntrlObject()){
					pComObj = GetOtSlaveK2hdkcComAddSubkey(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
				}else{
					pComObj = GetPmSlaveK2hdkcComAddSubkey(&_slaveobj);
				}
				if(!pComObj){
					SetError("Internal error: Could not create command object.");
					return;
				}
				if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, reinterpret_cast<const unsigned char*>(_strsubkey.c_str()), _strsubkey.length() + 1, (_strval.empty() ? NULL : reinterpret_cast<const unsigned char*>(_strval.c_str())), (_strval.empty() ? 0 : _strval.length() + 1), true, (_strpass.empty() ? NULL : _strpass.c_str()), (_expire > 0 ? &_expire : NULL), &rescode)){
					SetError("Failed to set value into subkey/key.");
				}
				DKC_DELETE(pComObj);
			}else{
				// set value to key
				K2hdkcComSet*	pComObj;
				if(!_slaveobj.GetChmCntrlObject()){
					pComObj = GetOtSlaveK2hdkcComSet(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
				}else{
					pComObj = GetPmSlaveK2hdkcComSet(&_slaveobj);
				}
				if(!pComObj){
					SetError("Internal error: Could not create command object.");
					return;
				}
				if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, (_strval.empty() ? NULL : reinterpret_cast<const unsigned char*>(_strval.c_str())), (_strval.empty() ? 0 : _strval.length() + 1), false, (_strpass.empty() ? NULL : _strpass.c_str()), (_expire > 0 ? &_expire : NULL), &rescode)){
					SetError("Failed to set value into key.");
				}
				DKC_DELETE(pComObj);
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strkey;
		std::string				_strval;
		std::string				_strsubkey;
		std::string				_strpass;
		time_t					_expire;
};

//---------------------------------------------------------
// SetSubkeysWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, unsigned char* psubkeys, size_t subkeylength)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class SetSubkeysWorker : public Napi::AsyncWorker
{
	public:
		SetSubkeysWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, unsigned char* psubkeys, size_t subkeylength) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key), _bysubkeys(NULL), _skeylen(0)
		{
			_callbackRef.Ref();

			if(psubkeys && 0UL < subkeylength){
				_bysubkeys	= k2hbindup(psubkeys, subkeylength);
				_skeylen		= subkeylength;
			}
		}

		~SetSubkeysWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
			K2H_Free(_bysubkeys);
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComSetSubkeys*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComSetSubkeys(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComSetSubkeys(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			bool			result	= false;
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(_bysubkeys && 0UL < _skeylen){
				// set subkeys to key
				result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, _bysubkeys, _skeylen, &rescode);
			}else{
				// clear subkeys
				result = pComObj->ClearSubkeysCommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, &rescode);
			}
			DKC_DELETE(pComObj);

			if(!result){
				if(_bysubkeys && 0UL < _skeylen){
					SetError("Failed to set subkeys into key.");
				}else{
					SetError("Failed to clear subkeys in key.");
				}
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strkey;
		unsigned char*			_bysubkeys;
		size_t					_skeylen;
};

//---------------------------------------------------------
// SetAllWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, const std::string& val, unsigned char* psubkeys, size_t subkeylength, const std::string& pass, const time_t input_expire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class SetAllWorker : public Napi::AsyncWorker
{
	public:
		SetAllWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, const std::string& val, unsigned char* psubkeys, size_t subkeylength, const std::string& pass, const time_t input_expire) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key), _strval(val), _bysubkeys(NULL), _skeylen(0), _strpass(pass), _expire(input_expire)
		{
			_callbackRef.Ref();

			if(psubkeys && 0UL < subkeylength){
				_bysubkeys	= k2hbindup(psubkeys, subkeylength);
				_skeylen		= subkeylength;
			}
		}

		~SetAllWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
			K2H_Free(_bysubkeys);
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// work
			bool			result	= false;
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!_strpass.empty() && 0 < _expire){
				// set value with passphrase and expire, then the operation is separated.
				K2hdkcComSet*	pComObj;
				if(!_slaveobj.GetChmCntrlObject()){
					pComObj = GetOtSlaveK2hdkcComSet(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
				}else{
					pComObj = GetPmSlaveK2hdkcComSet(&_slaveobj);
				}
				if(!pComObj){
					SetError("Internal error: Could not create command object.");
					return;
				}

				// set value to key
				result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, (_strval.empty() ? NULL : reinterpret_cast<const unsigned char*>(_strval.c_str())), (_strval.empty() ? 0 : _strval.length() + 1), false, _strpass.c_str(), &_expire, &rescode);
				DKC_DELETE(pComObj);

				// set subkeys
				if(result && _bysubkeys && 0UL < _skeylen){
					K2hdkcComSetSubkeys*	pSubComObj;
					if(!_slaveobj.GetChmCntrlObject()){
						pSubComObj = GetOtSlaveK2hdkcComSetSubkeys(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
					}else{
						pSubComObj = GetPmSlaveK2hdkcComSetSubkeys(&_slaveobj);
					}
					if(!pSubComObj){
						SetError("Internal error: Could not create command object.");
						return;
					}

					// set subkeys to key
					result = pSubComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, _bysubkeys, _skeylen, &rescode);
					DKC_DELETE(pSubComObj);
				}

			}else{
				// no passphrase and no expire, then one action
				K2hdkcComSetAll*	pComObj;
				if(!_slaveobj.GetChmCntrlObject()){
					pComObj = GetOtSlaveK2hdkcComSetAll(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
				}else{
					pComObj = GetPmSlaveK2hdkcComSetAll(&_slaveobj);
				}
				if(!pComObj){
					SetError("Internal error: Could not create command object.");
					return;
				}
				// set value and subkeys
				result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, (_strval.empty() ? NULL : reinterpret_cast<const unsigned char*>(_strval.c_str())), (_strval.empty() ? 0 : _strval.length() + 1), _bysubkeys, _skeylen, NULL, 0UL, &rescode);
				DKC_DELETE(pComObj);
			}
			if(!result){
				SetError("Failed to set value/subkeys into key.");
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strkey;
		std::string				_strval;
		unsigned char*			_bysubkeys;
		size_t					_skeylen;
		std::string				_strpass;
		time_t					_expire;
};

//---------------------------------------------------------
// RemoveWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, bool is_del_subkeys)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class RemoveWorker : public Napi::AsyncWorker
{
	public:
		RemoveWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, bool is_del_subkeys) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key), _is_subkeys(is_del_subkeys)
		{
			_callbackRef.Ref();
		}

		~RemoveWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComDel*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComDel(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComDel(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			// remove key
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, _is_subkeys, true, &rescode)){
				SetError("Failed to remove key.");
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strkey;
		bool					_is_subkeys;
};

//---------------------------------------------------------
// RenameWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& oldkey, const std::string& newkey, const std::string& parentkey, bool is_check_attr, const std::string& pass, const time_t input_expire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class RenameWorker : public Napi::AsyncWorker
{
	public:
		RenameWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& oldkey, const std::string& newkey, const std::string& parentkey, bool is_check_attr, const std::string& pass, const time_t input_expire) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strold(oldkey), _strnew(newkey), _strparent(parentkey), _attrchk(is_check_attr), _strpass(pass), _expire(input_expire)
		{
			_callbackRef.Ref();
		}

		~RenameWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strold.empty() || _strnew.empty()){
				SetError("Specified source(old) key or destination(new) key name is empty(null)");
				return;
			}

			// work
			K2hdkcComRen*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComRen(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComRen(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			// rename key
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strold.c_str()), _strold.length() + 1, reinterpret_cast<const unsigned char*>(_strnew.c_str()), _strnew.length() + 1, (_strparent.empty() ? NULL : reinterpret_cast<const unsigned char*>(_strparent.c_str())), (_strparent.empty() ? 0 : _strparent.length() + 1), _attrchk, (_strpass.empty() ? NULL : _strpass.c_str()), (_expire > 0 ? &_expire : NULL), &rescode)){
				SetError("Failed to rename key.");
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strold;
		std::string				_strnew;
		std::string				_strparent;
		bool					_attrchk;
		std::string				_strpass;
		time_t					_expire;
};

//---------------------------------------------------------
// QueuePushWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& prefix, const std::string& key, const std::string& val, bool is_fifo_type, bool is_check_attr, const std::string& pass, const time_t input_expire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class QueuePushWorker : public Napi::AsyncWorker
{
	public:
		QueuePushWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& prefix, const std::string& key, const std::string& val, bool is_fifo_type, bool is_check_attr, const std::string& pass, const time_t input_expire) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strprefix(prefix), _strkey(key), _strval(val), _is_fifo(is_fifo_type), _attrchk(is_check_attr), _strpass(pass), _expire(input_expire)
		{
			_callbackRef.Ref();
		}

		~QueuePushWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strprefix.empty() || _strval.empty()){
				SetError("Specified prefix or value is empty(null)");
				return;
			}

			// work
			K2hdkcComQPush*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComQPush(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComQPush(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			bool			result	= false;
			if(!_strkey.empty()){
				// key queue
				result = pComObj->KeyQueueCommandSend(reinterpret_cast<const unsigned char*>(_strprefix.c_str()), _strprefix.length() + 1, reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, reinterpret_cast<const unsigned char*>(_strval.c_str()), _strval.length() + 1, _is_fifo, NULL, 0UL, _attrchk, (_strpass.empty() ? NULL : _strpass.c_str()), (_expire > 0 ? &_expire : NULL), &rescode);
			}else{
				// queue
				result = pComObj->QueueCommandSend(reinterpret_cast<const unsigned char*>(_strprefix.c_str()), _strprefix.length() + 1, reinterpret_cast<const unsigned char*>(_strval.c_str()), _strval.length() + 1, _is_fifo, NULL, 0UL, _attrchk, (_strpass.empty() ? NULL : _strpass.c_str()), (_expire > 0 ? &_expire : NULL), &rescode);
			}
			if(!result){
				SetError("Failed to push queue.");
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strprefix;
		std::string				_strkey;
		std::string				_strval;
		bool					_is_fifo;
		bool					_attrchk;
		std::string				_strpass;
		time_t					_expire;
};

//---------------------------------------------------------
// QueuePopWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& prefix, bool is_fifo_type, bool is_key_queue_type, const std::string& pass)
// Callback function:	function(string error[[, string key], string value])
//
//---------------------------------------------------------
class QueuePopWorker : public Napi::AsyncWorker
{
	public:
		QueuePopWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& prefix, bool is_fifo_type, bool is_key_queue_type, const std::string& pass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strprefix(prefix), _is_fifo(is_fifo_type), _is_key_queue(is_key_queue_type), _strpass(pass), _is_res_key_set(false), _strreskey(""), _is_res_val_set(false), _strresval("")
		{
			_callbackRef.Ref();
		}

		~QueuePopWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strprefix.empty()){
				SetError("Specified prefix is empty(null)");
				return;
			}

			// work
			K2hdkcComQPop*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComQPop(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComQPop(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			bool			result	= false;
			if(_is_key_queue){
				// key queue
				const unsigned char*	pkeytmp		= NULL;
				size_t					keytmplen	= 0L;
				const unsigned char*	pvaltmp		= NULL;
				size_t					valtmplen	= 0L;
				result = pComObj->KeyQueueCommandSend(reinterpret_cast<const unsigned char*>(_strprefix.c_str()), _strprefix.length() + 1, _is_fifo, true, (_strpass.empty() ? NULL : _strpass.c_str()), &pkeytmp, &keytmplen, &pvaltmp, &valtmplen, &rescode);
				if(result && (pkeytmp && 0 < keytmplen)){
					_is_res_key_set		= true;
					_strreskey			= std::string(reinterpret_cast<const char*>(pkeytmp), ('\0' == pkeytmp[keytmplen - 1] ? keytmplen - 1 : keytmplen));
					if(pvaltmp && 0 < valtmplen){
						_is_res_val_set	= true;
						_strresval		= std::string(reinterpret_cast<const char*>(pvaltmp), ('\0' == pvaltmp[valtmplen - 1] ? valtmplen - 1 : valtmplen));
					}
				}
			}else{
				// queue
				const unsigned char*	pvaltmp		= NULL;
				size_t					valtmplen	= 0L;
				result = pComObj->QueueCommandSend(reinterpret_cast<const unsigned char*>(_strprefix.c_str()), _strprefix.length() + 1, _is_fifo, true, (_strpass.empty() ? NULL : _strpass.c_str()), &pvaltmp, &valtmplen, &rescode);

				if(result && (pvaltmp && 0 < valtmplen)){
					_is_res_val_set	= true;
					_strresval		= std::string(reinterpret_cast<const char*>(pvaltmp), ('\0' == pvaltmp[valtmplen - 1] ? valtmplen - 1 : valtmplen));
				}
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				if(_is_key_queue){
					if(_is_res_key_set){
						if(_is_res_val_set){
							Napi::String	res_skey = Napi::String::New(env, _strreskey.c_str(), _strreskey.length());
							Napi::String	res_sval = Napi::String::New(env, _strresval.c_str(), _strresval.length());
							_callbackRef.Value().Call({ env.Null(), res_skey, res_sval });
						}else{

							Napi::String	res_skey = Napi::String::New(env, _strreskey.c_str(), _strreskey.length());
							_callbackRef.Value().Call({ env.Null(), res_skey });
						}
					}else{
						_callbackRef.Value().Call({ env.Null(), env.Null(), env.Null() });
					}
				}else{
					if(_is_res_val_set){
						Napi::String	res_sval = Napi::String::New(env, _strresval.c_str(), _strresval.length());
						_callbackRef.Value().Call({ env.Null(), res_sval });
					}else{
						_callbackRef.Value().Call({ env.Null(), env.Null() });
					}
				}
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strprefix;
		bool					_is_fifo;
		bool					_is_key_queue;
		std::string				_strpass;

		bool					_is_res_key_set;
		std::string				_strreskey;
		bool					_is_res_val_set;
		std::string				_strresval;
};

//---------------------------------------------------------
// QueueRemoveWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& prefix, int remove_count, bool is_fifo_type, bool is_key_queue_type, const std::string& pass)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class QueueRemoveWorker : public Napi::AsyncWorker
{
	public:
		QueueRemoveWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& prefix, int remove_count, bool is_fifo_type, bool is_key_queue_type, const std::string& pass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strprefix(prefix), _rmcount(remove_count), _is_fifo(is_fifo_type), _is_key_queue(is_key_queue_type), _strpass(pass)
		{
			_callbackRef.Ref();
		}

		~QueueRemoveWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strprefix.empty()){
				SetError("Specified prefix is empty(null)");
				return;
			}

			// work
			K2hdkcComQDel*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComQDel(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComQDel(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			bool			result	= false;
			if(_is_key_queue){
				// key queue
				result = pComObj->KeyQueueCommandSend(reinterpret_cast<const unsigned char*>(_strprefix.c_str()), _strprefix.length() + 1, _rmcount, _is_fifo, true, (_strpass.empty() ? NULL : _strpass.c_str()), &rescode);
			}else{
				// queue
				result = pComObj->QueueCommandSend(reinterpret_cast<const unsigned char*>(_strprefix.c_str()), _strprefix.length() + 1, _rmcount, _is_fifo, true, (_strpass.empty() ? NULL : _strpass.c_str()), &rescode);
			}
			if(!result){
				SetError("Failed to remove queue.");
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strprefix;
		int						_rmcount;
		bool					_is_fifo;
		bool					_is_key_queue;
		std::string				_strpass;
};

//---------------------------------------------------------
// CasInitWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, uint32_t val, const std::string& pass, const time_t input_expire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class CasInitWorker : public Napi::AsyncWorker
{
	public:
		CasInitWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, uint32_t val, const std::string& pass, const time_t input_expire) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key), _value(val), _strpass(pass), _expire(input_expire)
		{
			_callbackRef.Ref();
		}

		~CasInitWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComCasInit*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComCasInit(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComCasInit(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, reinterpret_cast<const unsigned char*>(&_value), sizeof(uint32_t), (_strpass.empty() ? NULL : _strpass.c_str()), (_expire > 0 ? &_expire : NULL), &rescode)){
				SetError("Failed to initialize CAS key and value.");
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strkey;
		uint32_t				_value;
		std::string				_strpass;
		time_t					_expire;
};

//---------------------------------------------------------
// CasGetWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, const std::string& pass)
// Callback function:	function(string error[, int value])
//
//---------------------------------------------------------
class CasGetWorker : public Napi::AsyncWorker
{
	public:
		CasGetWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, const std::string& pass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key), _strpass(pass), _resvalue(0)
		{
			_callbackRef.Ref();
		}

		~CasGetWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComCasGet*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComCasGet(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComCasGet(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t			rescode		= DKC_NORESTYPE;
			const unsigned char*	pvaltmp		= NULL;
			size_t					valtmplen	= 0L;
			if(pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, true, (_strpass.empty() ? NULL : _strpass.c_str()), &pvaltmp, &valtmplen, &rescode) && (valtmplen == sizeof(uint32_t))){
				memcpy(reinterpret_cast<unsigned char*>(&_resvalue), pvaltmp, valtmplen);
			}else{
				SetError("Failed to get CAS value.");
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				Napi::Number	res_val = Napi::Number::New(env, _resvalue);
				_callbackRef.Value().Call({ env.Null(), res_val });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;
		std::string				_strkey;
		std::string				_strpass;

		uint32_t				_resvalue;
};

//---------------------------------------------------------
// CasSetWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, uint32_t oldvalue, uint32_t newvalue, const std::string& pass, const time_t input_expire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class CasSetWorker : public Napi::AsyncWorker
{
	public:
		CasSetWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, uint32_t oldvalue, uint32_t newvalue, const std::string& pass, const time_t input_expire) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key), _oldval(oldvalue), _newval(newvalue), _strpass(pass), _expire(input_expire)
		{
			_callbackRef.Ref();
		}

		~CasSetWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComCasSet*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComCasSet(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComCasSet(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, reinterpret_cast<const unsigned char*>(&_oldval), sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&_newval), sizeof(uint32_t), true, (_strpass.empty() ? NULL : _strpass.c_str()), (_expire > 0 ? &_expire : NULL), &rescode)){
				SetError("Failed to set CAS value.");
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strkey;
		uint32_t				_oldval;
		uint32_t				_newval;
		std::string				_strpass;
		time_t					_expire;
};

//---------------------------------------------------------
// CasIncDecWorker class
//
// Constructor:			constructor(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, bool is_increment_type, const std::string& pass, const time_t input_expire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class CasIncDecWorker : public Napi::AsyncWorker
{
	public:
		CasIncDecWorker(const Napi::Function& callback, K2hdkcSlave& slaveobj, const std::string& configuration, int control_port, const std::string& inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const std::string& key, bool is_increment_type, const std::string& pass, const time_t input_expire) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)),
			_slaveobj(slaveobj), _conf(configuration), _ctlport(control_port), _cuk(inputcuk), _auto_rejoin(is_auto_rejoin), _no_giveup_rejoin(is_nogiveup_rejoin), _strkey(key), _is_increment(is_increment_type), _strpass(pass), _expire(input_expire)
		{
			_callbackRef.Ref();
		}

		~CasIncDecWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_slaveobj.GetChmCntrlObject() && _conf.empty()){
				// onetime connection mode needs configuration
				SetError("No configuration is associated to async worker");
				return;
			}
			if(_strkey.empty()){
				SetError("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComCasIncDec*	pComObj;
			if(!_slaveobj.GetChmCntrlObject()){
				pComObj = GetOtSlaveK2hdkcComCasIncDec(_conf.c_str(), _ctlport, (_cuk.empty() ? NULL : _cuk.c_str()), _auto_rejoin, _no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComCasIncDec(&_slaveobj);
			}
			if(!pComObj){
				SetError("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, _is_increment, true, (_strpass.empty() ? NULL : _strpass.c_str()), (_expire > 0 ? &_expire : NULL), &rescode)){
				if(_is_increment){
					SetError("Failed to increment CAS value.");
				}else{
					SetError("Failed to decrement CAS value.");
				}
			}
			DKC_DELETE(pComObj);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker");
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, err.Value().ToString().Utf8Value()) });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2hdkcSlave&			_slaveobj;
		std::string				_conf;
		int16_t					_ctlport;
		std::string				_cuk;
		bool					_auto_rejoin;
		bool					_no_giveup_rejoin;

		std::string				_strkey;
		bool					_is_increment;
		std::string				_strpass;
		time_t					_expire;
};

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
