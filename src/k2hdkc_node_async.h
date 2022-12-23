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
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class InitWorker : public Nan::AsyncWorker
{
	public:
		InitWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin)
		{}
		~InitWorker() {}

		void Execute()
		{
			if(!pslaveobj){
				this->SetErrorMessage("No object is associated to async worker");
				return;
			}
			if(!is_set_conf){
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}

			// build permanent connection object
			if(pslaveobj->Initialize(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin)){
				if(!pslaveobj->Open(no_giveup_rejoin)){
					// set error
					this->SetErrorMessage("Failed to open(join to) chmpx slave.");
				}
			}else{
				// set error
				this->SetErrorMessage("Failed to initialize k2hdkcslave object.");
			}
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;
};

//---------------------------------------------------------
// GetValueWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, const char* psubkey, bool is_check_attr, const char* ppass)
// Callback function:	function(string error[, string value])
//
//---------------------------------------------------------
class GetValueWorker : public Nan::AsyncWorker
{
	public:
		GetValueWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, const char* psubkey, bool is_check_attr, const char* ppass) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), is_subkey_set(NULL != psubkey), strsubkey(psubkey ? psubkey : ""), attrchk(is_check_attr), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), is_result_set(false), strresult("")
		{}
		~GetValueWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// work
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(is_subkey_set){
				// subkey is specified, thus need to check the key has it.
				K2hdkcComGetSubkeys*	pSubComObj;
				if(!pslaveobj){
					pSubComObj = GetOtSlaveK2hdkcComGetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
				}else{
					pSubComObj = GetPmSlaveK2hdkcComGetSubkeys(pslaveobj);
				}
				if(!pSubComObj){
					this->SetErrorMessage("Internal error: Could not create command object.");
					return;
				}
				// get subkey list in key
				K2HSubKeys*	pSubKeys= NULL;
				if(!pSubComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, attrchk, &pSubKeys, &rescode) || !pSubKeys){
					// key does not have any subkey
					DKC_DELETE(pSubComObj);
					is_result_set = false;
					return;
				}

				// convert subkey binary data to string array
				strarr_t	strarr;
				pSubKeys->StringArray(strarr);
				DKC_DELETE(pSubKeys);
				DKC_DELETE(pSubComObj);

				// check subkey
				bool	found = false;
				for(strarr_t::const_iterator iter = strarr.begin(); iter != strarr.end(); ++iter){
					if(0 == strcmp(iter->c_str(), strsubkey.c_str())){
						found = true;
						break;
					}
				}
				if(!found){
					// not found subkey in key
					is_result_set = false;
					return;
				}
				// switch key to subkey
				strkey = strsubkey;
			}

			// get value
			K2hdkcComGet*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComGet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComGet(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}
			const unsigned char*	pvaltmp		= NULL;
			size_t					valtmplen	= 0L;
			bool					result		= pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, attrchk, (is_pass_set ? strpass.c_str() : NULL), &pvaltmp, &valtmplen, &rescode);
			if(result && (pvaltmp && 0 < valtmplen)){
				strresult		= std::string(reinterpret_cast<const char*>(pvaltmp), valtmplen);
				is_result_set	= true;
			}else{
				is_result_set	= false;
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 2;

			if(callback){
				if(is_result_set){
					v8::Local<v8::Value>	argv[argc] = { Nan::Null(), Nan::New<v8::String>(strresult.c_str()).ToLocalChecked() };
					callback->Call(argc, argv);
				}else{
					v8::Local<v8::Value>	argv[argc] = { Nan::Null(), Nan::Null() };
					callback->Call(argc, argv);
				}
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_key_set;
		std::string		strkey;
		bool			is_subkey_set;
		std::string		strsubkey;
		bool			attrchk;
		bool			is_pass_set;
		std::string		strpass;

		bool			is_result_set;
		std::string		strresult;
};

//---------------------------------------------------------
// GetSubkeysWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, bool is_check_attr)
// Callback function:	function(string error, array subkeys)
//
//---------------------------------------------------------
class GetSubkeysWorker : public Nan::AsyncWorker
{
	public:
		GetSubkeysWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, bool is_check_attr) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin), is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), attrchk(is_check_attr)
		{}
		~GetSubkeysWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// get command object
			K2hdkcComGetSubkeys*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComGetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComGetSubkeys(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			// get subkey list in key
			dkcres_type_t	rescode = DKC_NORESTYPE;
			K2HSubKeys*		pSubKeys= NULL;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, attrchk, &pSubKeys, &rescode) || !pSubKeys){
				// key does not have any subkey
				this->SetErrorMessage("Failed to get subkey because the key does not have any subkey.");
			}else{
				// convert subkey binary data to string array
				subkey_array.clear();
				pSubKeys->StringArray(subkey_array);
				DKC_DELETE(pSubKeys);

				if(0 == subkey_array.size()){
					this->SetErrorMessage("Failed to get subkey because the key does not have any subkey.");
				}
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			v8::Local<v8::Array>	retarr		= Nan::New<v8::Array>();
			int						pos			= 0 ;
			for(strarr_t::const_iterator iter = subkey_array.begin(); iter != subkey_array.end(); ++iter, ++pos){
				Nan::Set(retarr, pos, Nan::New<v8::String>(iter->c_str()).ToLocalChecked());
			}
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), retarr };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked(), Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;
		bool			is_key_set;

		std::string		strkey;
		bool			attrchk;

		strarr_t		subkey_array;
};

//---------------------------------------------------------
// GetAttrsWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey)
// Callback function:	function(string error, array attribute_keys)
//
//---------------------------------------------------------
class GetAttrsWorker : public Nan::AsyncWorker
{
	public:
		GetAttrsWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin), is_key_set(NULL != pkey), strkey(pkey ? pkey : "")
		{}
		~GetAttrsWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// get command object
			K2hdkcComGetAttrs*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComGetAttrs(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComGetAttrs(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			// get attribute list in key
			dkcres_type_t	rescode = DKC_NORESTYPE;
			K2HAttrs*		pAttrs	= NULL;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, &pAttrs, &rescode) || !pAttrs){
				// key does not have any attribute key
				this->SetErrorMessage("Failed to get attribute keys because the key does not have any attribute.");
			}else{
				// convert attribute key binary data to string array
				attrs_array.clear();
				pAttrs->KeyStringArray(attrs_array);
				DKC_DELETE(pAttrs);

				if(0 == attrs_array.size()){
					this->SetErrorMessage("Failed to get attribute keys because the key does not have any attribute.");
				}
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			v8::Local<v8::Array>	retarr		= Nan::New<v8::Array>();
			int						pos			= 0 ;
			for(strarr_t::const_iterator iter = attrs_array.begin(); iter != attrs_array.end(); ++iter, ++pos){
				Nan::Set(retarr, pos, Nan::New<v8::String>(iter->c_str()).ToLocalChecked());
			}
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), retarr };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked(), Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;
		bool			is_key_set;

		std::string		strkey;

		strarr_t		attrs_array;
};

//---------------------------------------------------------
// SetValueWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, const char* pval, const char* psubkey, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class SetValueWorker : public Nan::AsyncWorker
{
	public:
		SetValueWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, const char* pval, const char* psubkey, const char* ppass, const time_t* pexpire) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), is_val_set(NULL != pval), strval(pval ? pval : ""), is_subkey_set(NULL != psubkey), strsubkey(psubkey ? psubkey : ""), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), expire(pexpire ? *pexpire : 0)
		{}
		~SetValueWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// work
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(is_subkey_set){
				// subkey is specified, set value into subkey
				K2hdkcComAddSubkey*	pComObj;
				if(!pslaveobj){
					pComObj = GetOtSlaveK2hdkcComAddSubkey(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
				}else{
					pComObj = GetPmSlaveK2hdkcComAddSubkey(pslaveobj);
				}
				if(!pComObj){
					this->SetErrorMessage("Internal error: Could not create command object.");
					return;
				}
				if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(strsubkey.c_str()), strsubkey.length() + 1, (is_val_set ? reinterpret_cast<const unsigned char*>(strval.c_str()) : NULL), (is_val_set ? strval.length() + 1 : 0), true, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode)){
					this->SetErrorMessage("Failed to set value into subkey/key.");
				}
				DKC_DELETE(pComObj);
			}else{
				// set value to key
				K2hdkcComSet*	pComObj;
				if(!pslaveobj){
					pComObj = GetOtSlaveK2hdkcComSet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
				}else{
					pComObj = GetPmSlaveK2hdkcComSet(pslaveobj);
				}
				if(!pComObj){
					this->SetErrorMessage("Internal error: Could not create command object.");
					return;
				}
				if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, (is_val_set ? reinterpret_cast<const unsigned char*>(strval.c_str()) : NULL), (is_val_set ? strval.length() + 1 : 0), false, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode)){
					this->SetErrorMessage("Failed to set value into key.");
				}
				DKC_DELETE(pComObj);
			}
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_key_set;
		std::string		strkey;
		bool			is_val_set;
		std::string		strval;
		bool			is_subkey_set;
		std::string		strsubkey;
		bool			is_pass_set;
		std::string		strpass;
		time_t			expire;
};

//---------------------------------------------------------
// SetSubkeysWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, unsigned char* psubkeys, size_t subkeylength)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class SetSubkeysWorker : public Nan::AsyncWorker
{
	public:
		SetSubkeysWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, unsigned char* psubkeys, size_t subkeylength) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), bysubkeys(NULL), skeylen(0)
		{
			if(psubkeys && 0UL < subkeylength){
				bysubkeys	= k2hbindup(psubkeys, subkeylength);
				skeylen		= subkeylength;
			}
		}

		~SetSubkeysWorker()
		{
			K2H_Free(bysubkeys);
		}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComSetSubkeys*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComSetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComSetSubkeys(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			bool			result	= false;
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(bysubkeys && 0UL < skeylen){
				// set subkeys to key
				result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, bysubkeys, skeylen, &rescode);
			}else{
				// clear subkeys
				result = pComObj->ClearSubkeysCommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, &rescode);
			}
			DKC_DELETE(pComObj);

			if(!result){
				if(bysubkeys && 0UL < skeylen){
					this->SetErrorMessage("Failed to set subkeys into key.");
				}else{
					this->SetErrorMessage("Failed to clear subkeys in key.");
				}
			}
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_key_set;
		std::string		strkey;
		unsigned char*	bysubkeys;
		size_t			skeylen;
};

//---------------------------------------------------------
// SetAllWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, const char* pval, unsigned char* psubkeys, size_t subkeylength, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class SetAllWorker : public Nan::AsyncWorker
{
	public:
		SetAllWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, const char* pval, unsigned char* psubkeys, size_t subkeylength, const char* ppass, const time_t* pexpire) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), is_val_set(NULL != pval), strval(pval ? pval : ""), bysubkeys(NULL), skeylen(0), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), expire(pexpire ? *pexpire : 0)
		{
			if(psubkeys && 0UL < subkeylength){
				bysubkeys	= k2hbindup(psubkeys, subkeylength);
				skeylen		= subkeylength;
			}
		}

		~SetAllWorker()
		{
			K2H_Free(bysubkeys);
		}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// work
			bool			result	= false;
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(is_pass_set && 0 < expire){
				// set value with passphrase and expire, then the operation is separated.
				K2hdkcComSet*	pComObj;
				if(!pslaveobj){
					pComObj = GetOtSlaveK2hdkcComSet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
				}else{
					pComObj = GetPmSlaveK2hdkcComSet(pslaveobj);
				}
				if(!pComObj){
					this->SetErrorMessage("Internal error: Could not create command object.");
					return;
				}

				// set value to key
				result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, (is_val_set ? reinterpret_cast<const unsigned char*>(strval.c_str()) : NULL), (is_val_set ? strval.length() + 1 : 0), false, (is_pass_set ? strpass.c_str() : NULL), &expire, &rescode);
				DKC_DELETE(pComObj);

				// set subkeys
				if(result && bysubkeys && 0UL < skeylen){

					K2hdkcComSetSubkeys*	pSubComObj;
					if(!pslaveobj){
						pSubComObj = GetOtSlaveK2hdkcComSetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
					}else{
						pSubComObj = GetPmSlaveK2hdkcComSetSubkeys(pslaveobj);
					}
					if(!pSubComObj){
						this->SetErrorMessage("Internal error: Could not create command object.");
						return;
					}

					// set subkeys to key
					result = pSubComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, bysubkeys, skeylen, &rescode);
					DKC_DELETE(pSubComObj);
				}

			}else{
				// no passphrase and no expire, then one action
				K2hdkcComSetAll*	pComObj;
				if(!pslaveobj){
					pComObj = GetOtSlaveK2hdkcComSetAll(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
				}else{
					pComObj = GetPmSlaveK2hdkcComSetAll(pslaveobj);
				}
				if(!pComObj){
					this->SetErrorMessage("Internal error: Could not create command object.");
					return;
				}
				// set value and subkeys
				result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, (is_val_set ? reinterpret_cast<const unsigned char*>(strval.c_str()) : NULL), (is_val_set ? strval.length() + 1 : 0), bysubkeys, skeylen, NULL, 0UL, &rescode);
				DKC_DELETE(pComObj);
			}
			if(!result){
				this->SetErrorMessage("Failed to set value/subkeys into key.");
			}
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_key_set;
		std::string		strkey;
		bool			is_val_set;
		std::string		strval;
		unsigned char*	bysubkeys;
		size_t			skeylen;
		bool			is_pass_set;
		std::string		strpass;
		time_t			expire;
};

//---------------------------------------------------------
// RemoveWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, bool is_del_subkeys)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class RemoveWorker : public Nan::AsyncWorker
{
	public:
		RemoveWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, bool is_del_subkeys) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), is_subkeys(is_del_subkeys)
		{}

		~RemoveWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComDel*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComDel(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComDel(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			// remove key
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, is_subkeys, true, &rescode)){
				this->SetErrorMessage("Failed to remove key.");
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_key_set;
		std::string		strkey;
		bool			is_subkeys;
};

//---------------------------------------------------------
// RenameWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* poldkey, const char* pnewkey, const char* pparentkey, bool is_check_attr, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class RenameWorker : public Nan::AsyncWorker
{
	public:
		RenameWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* poldkey, const char* pnewkey, const char* pparentkey, bool is_check_attr, const char* ppass, const time_t* pexpire) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_old_set(NULL != poldkey), strold(poldkey ? poldkey : ""), is_new_set(NULL != pnewkey), strnew(pnewkey ? pnewkey : ""), is_parent_set(NULL != pparentkey), strparent(pparentkey ? pparentkey : ""), attrchk(is_check_attr), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), expire(pexpire ? *pexpire : 0)
		{}

		~RenameWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_old_set || !is_new_set){
				this->SetErrorMessage("Specified source(old) key or destination(new) key name is empty(null)");
				return;
			}

			// work
			K2hdkcComRen*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComRen(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComRen(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			// rename key
			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strold.c_str()), strold.length() + 1, reinterpret_cast<const unsigned char*>(strnew.c_str()), strnew.length() + 1, (is_parent_set ? reinterpret_cast<const unsigned char*>(strparent.c_str()) : NULL), (is_parent_set ? strparent.length() + 1 : 0), attrchk, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode)){
				this->SetErrorMessage("Failed to rename key.");
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_old_set;
		std::string		strold;
		bool			is_new_set;
		std::string		strnew;
		bool			is_parent_set;
		std::string		strparent;
		bool			attrchk;
		bool			is_pass_set;
		std::string		strpass;
		time_t			expire;
};

//---------------------------------------------------------
// QueuePushWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pprefix, const char* pkey, const char* pval, bool is_fifo_type, bool is_check_attr, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class QueuePushWorker : public Nan::AsyncWorker
{
	public:
		QueuePushWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pprefix, const char* pkey, const char* pval, bool is_fifo_type, bool is_check_attr, const char* ppass, const time_t* pexpire) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_prefix_set(NULL != pprefix), strprefix(pprefix ? pprefix : ""), is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), is_val_set(NULL != pval), strval(pval ? pval : ""), is_fifo(is_fifo_type), attrchk(is_check_attr), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), expire(pexpire ? *pexpire : 0)
		{}

		~QueuePushWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_prefix_set || !is_val_set){
				this->SetErrorMessage("Specified prefix or value is empty(null)");
				return;
			}

			// work
			K2hdkcComQPush*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComQPush(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComQPush(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			bool			result	= false;
			if(is_key_set){
				// key queue
				result = pComObj->KeyQueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(strval.c_str()), strval.length() + 1, is_fifo, NULL, 0UL, attrchk, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode);
			}else{
				// queue
				result = pComObj->QueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, reinterpret_cast<const unsigned char*>(strval.c_str()), strval.length() + 1, is_fifo, NULL, 0UL, attrchk, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode);
			}
			if(!result){
				this->SetErrorMessage("Failed to push queue.");
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_prefix_set;
		std::string		strprefix;
		bool			is_key_set;
		std::string		strkey;
		bool			is_val_set;
		std::string		strval;
		bool			is_fifo;
		bool			attrchk;
		bool			is_pass_set;
		std::string		strpass;
		time_t			expire;
};

//---------------------------------------------------------
// QueuePopWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pprefix, bool is_fifo_type, bool is_key_queue_type, const char* ppass)
// Callback function:	function(string error[[, string key], string value])
//
//---------------------------------------------------------
class QueuePopWorker : public Nan::AsyncWorker
{
	public:
		QueuePopWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pprefix, bool is_fifo_type, bool is_key_queue_type, const char* ppass) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_prefix_set(NULL != pprefix), strprefix(pprefix ? pprefix : ""), is_fifo(is_fifo_type), is_key_queue(is_key_queue_type), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""),
			is_res_key_set(false), strreskey(""), is_res_val_set(false), strresval("")
		{}

		~QueuePopWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_prefix_set){
				this->SetErrorMessage("Specified prefix is empty(null)");
				return;
			}

			// work
			K2hdkcComQPop*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComQPop(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComQPop(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			bool			result	= false;
			if(is_key_queue){
				// key queue
				const unsigned char*	pkeytmp		= NULL;
				size_t					keytmplen	= 0L;
				const unsigned char*	pvaltmp		= NULL;
				size_t					valtmplen	= 0L;
				result = pComObj->KeyQueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, is_fifo, true, (is_pass_set ? strpass.c_str() : NULL), &pkeytmp, &keytmplen, &pvaltmp, &valtmplen, &rescode);
				if(result && (pkeytmp && 0 < keytmplen)){
					is_res_key_set		= true;
					strreskey			= std::string(reinterpret_cast<const char*>(pkeytmp), keytmplen);
					if(pvaltmp && 0 < valtmplen){
						is_res_val_set	= true;
						strresval		= std::string(reinterpret_cast<const char*>(pvaltmp), valtmplen);
					}
				}
			}else{
				// queue
				const unsigned char*	pvaltmp		= NULL;
				size_t					valtmplen	= 0L;
				result = pComObj->QueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, is_fifo, true, (is_pass_set ? strpass.c_str() : NULL), &pvaltmp, &valtmplen, &rescode);

				if(result && (pvaltmp && 0 < valtmplen)){
					is_res_val_set	= true;
					strresval		= std::string(reinterpret_cast<const char*>(pvaltmp), valtmplen);
				}
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;

			if(!callback){
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
			if(is_key_queue){
				const int						argc		= 3;
				if(is_res_key_set){
					if(is_res_val_set){
						v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New<v8::String>(strreskey.c_str()).ToLocalChecked(), Nan::New<v8::String>(strresval.c_str()).ToLocalChecked() };
						callback->Call(argc, argv);
					}else{
						v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New<v8::String>(strreskey.c_str()).ToLocalChecked(), Nan::Null() };
						callback->Call(argc, argv);
					}
				}else{
					v8::Local<v8::Value>		argv[argc]	= { Nan::Null(), Nan::Null(), Nan::Null() };
					callback->Call(argc, argv);
				}
			}else{
				const int						argc		= 2;
				if(is_res_val_set){
					v8::Local<v8::Value>		argv[argc]	= { Nan::Null(), Nan::New<v8::String>(strresval.c_str()).ToLocalChecked() };
					callback->Call(argc, argv);
				}else{
					v8::Local<v8::Value>		argv[argc]	= { Nan::Null(), Nan::Null() };
					callback->Call(argc, argv);
				}
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_prefix_set;
		std::string		strprefix;
		bool			is_fifo;
		bool			is_key_queue;
		bool			is_pass_set;
		std::string		strpass;

		bool			is_res_key_set;
		std::string		strreskey;
		bool			is_res_val_set;
		std::string		strresval;
};

//---------------------------------------------------------
// QueueRemoveWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pprefix, int remove_count, bool is_fifo_type, bool is_key_queue_type, const char* ppass)
// Callback function:	function(string error[[, string key], string value])
//
//---------------------------------------------------------
class QueueRemoveWorker : public Nan::AsyncWorker
{
	public:
		QueueRemoveWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pprefix, int remove_count, bool is_fifo_type, bool is_key_queue_type, const char* ppass) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_prefix_set(NULL != pprefix), strprefix(pprefix ? pprefix : ""), count(remove_count), is_fifo(is_fifo_type), is_key_queue(is_key_queue_type), is_pass_set(NULL != ppass), strpass(ppass ? ppass : "")
		{}

		~QueueRemoveWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_prefix_set){
				this->SetErrorMessage("Specified prefix is empty(null)");
				return;
			}

			// work
			K2hdkcComQDel*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComQDel(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComQDel(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			bool			result	= false;
			if(is_key_queue){
				// key queue
				result = pComObj->KeyQueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, count, is_fifo, true, (is_pass_set ? strpass.c_str() : NULL), &rescode);
			}else{
				// queue
				result = pComObj->QueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, count, is_fifo, true, (is_pass_set ? strpass.c_str() : NULL), &rescode);
			}
			if(!result){
				this->SetErrorMessage("Failed to remove queue.");
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_prefix_set;
		std::string		strprefix;
		int				count;
		bool			is_fifo;
		bool			is_key_queue;
		bool			is_pass_set;
		std::string		strpass;
};

//---------------------------------------------------------
// CasInitWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, uint32_t val, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class CasInitWorker : public Nan::AsyncWorker
{
	public:
		CasInitWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, uint32_t val, const char* ppass, const time_t* pexpire) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), value(val), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), expire(pexpire ? *pexpire : 0)
		{}
		~CasInitWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComCasInit*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComCasInit(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComCasInit(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(&value), sizeof(uint32_t), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode)){
				this->SetErrorMessage("Failed to initialize CAS key and value.");
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_key_set;
		std::string		strkey;
		uint32_t		value;
		bool			is_pass_set;
		std::string		strpass;
		time_t			expire;
};

//---------------------------------------------------------
// CasGetWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, const char* ppass)
// Callback function:	function(string error[, int value])
//
//---------------------------------------------------------
class CasGetWorker : public Nan::AsyncWorker
{
	public:
		CasGetWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, const char* ppass) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), resvalue(0)
		{}
		~CasGetWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComCasGet*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComCasGet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComCasGet(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t			rescode		= DKC_NORESTYPE;
			const unsigned char*	pvaltmp		= NULL;
			size_t					valtmplen	= 0L;
			if(pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, true, (is_pass_set ? strpass.c_str() : NULL), &pvaltmp, &valtmplen, &rescode) && (valtmplen == sizeof(uint32_t))){
				memcpy(reinterpret_cast<unsigned char*>(&resvalue), pvaltmp, valtmplen);
			}else{
				this->SetErrorMessage("Failed to get CAS value.");
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New(resvalue) };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_key_set;
		std::string		strkey;
		bool			is_pass_set;
		std::string		strpass;

		uint32_t		resvalue;
};

//---------------------------------------------------------
// CasSetWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, uint32_t oldvalue, uint32_t newvalue, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class CasSetWorker : public Nan::AsyncWorker
{
	public:
		CasSetWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, uint32_t oldvalue, uint32_t newvalue, const char* ppass, const time_t* pexpire) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), oldval(oldvalue), newval(newvalue), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), expire(pexpire ? *pexpire : 0)
		{}
		~CasSetWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComCasSet*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComCasSet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComCasSet(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(&oldval), sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&newval), sizeof(uint32_t), true, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode)){
				this->SetErrorMessage("Failed to set CAS value.");
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_key_set;
		std::string		strkey;
		uint32_t		oldval;
		uint32_t		newval;
		bool			is_pass_set;
		std::string		strpass;
		time_t			expire;
};

//---------------------------------------------------------
// CasIncDecWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, bool is_increment_type, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class CasIncDecWorker : public Nan::AsyncWorker
{
	public:
		CasIncDecWorker(Nan::Callback* callback, K2hdkcSlave* pobj, const char* configuration, int control_port, const char* inputcuk, bool is_auto_rejoin, bool is_nogiveup_rejoin, const char* pkey, bool is_increment_type, const char* ppass, const time_t* pexpire) :
			Nan::AsyncWorker(callback), pslaveobj(pobj),
			is_set_conf(NULL != configuration), conf(configuration ? configuration : ""), ctlport(control_port), cuk(inputcuk ? inputcuk : ""), auto_rejoin(is_auto_rejoin), no_giveup_rejoin(is_nogiveup_rejoin),
			is_key_set(NULL != pkey), strkey(pkey ? pkey : ""), is_increment(is_increment_type), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), expire(pexpire ? *pexpire : 0)
		{}
		~CasIncDecWorker() {}

		void Execute()
		{
			if(!pslaveobj && !is_set_conf){
				// onetime connection mode needs configuration
				this->SetErrorMessage("No configuration is associated to async worker");
				return;
			}
			if(!is_key_set){
				this->SetErrorMessage("Specified key is empty(null)");
				return;
			}

			// work
			K2hdkcComCasIncDec*	pComObj;
			if(!pslaveobj){
				pComObj = GetOtSlaveK2hdkcComCasIncDec(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComCasIncDec(pslaveobj);
			}
			if(!pComObj){
				this->SetErrorMessage("Internal error: Could not create command object.");
				return;
			}

			dkcres_type_t	rescode = DKC_NORESTYPE;
			if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, is_increment, true, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode)){
				if(is_increment){
					this->SetErrorMessage("Failed to increment CAS value.");
				}else{
					this->SetErrorMessage("Failed to decrement CAS value.");
				}
			}
			DKC_DELETE(pComObj);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2hdkcSlave*	pslaveobj;

		bool			is_set_conf;
		std::string		conf;
		int16_t			ctlport;
		std::string		cuk;
		bool			auto_rejoin;
		bool			no_giveup_rejoin;

		bool			is_key_set;
		std::string		strkey;
		bool			is_increment;
		bool			is_pass_set;
		std::string		strpass;
		time_t			expire;
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
