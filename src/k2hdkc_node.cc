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

#include "k2hdkc_node.h"
#include "k2hdkc_node_async.h"

using namespace std;

//---------------------------------------------------------
// Emitter
//---------------------------------------------------------
#define	EMITTER_POS_INIT			(0)
#define	EMITTER_POS_GET				(EMITTER_POS_INIT		+ 1)
#define	EMITTER_POS_GETSUBKEYS		(EMITTER_POS_GET		+ 1)
#define	EMITTER_POS_GETATTRS		(EMITTER_POS_GETSUBKEYS	+ 1)
#define	EMITTER_POS_SET				(EMITTER_POS_GETATTRS	+ 1)
#define	EMITTER_POS_SETSUBKEYS		(EMITTER_POS_SET		+ 1)
#define	EMITTER_POS_SETALL			(EMITTER_POS_SETSUBKEYS	+ 1)
#define	EMITTER_POS_REMOVE			(EMITTER_POS_SETALL		+ 1)
#define	EMITTER_POS_RENAME			(EMITTER_POS_REMOVE		+ 1)
#define	EMITTER_POS_QUEUEPUSH		(EMITTER_POS_RENAME		+ 1)
#define	EMITTER_POS_QUEUEPOP		(EMITTER_POS_QUEUEPUSH	+ 1)
#define	EMITTER_POS_QUEUEREMOVE		(EMITTER_POS_QUEUEPOP	+ 1)
#define	EMITTER_POS_CASINIT			(EMITTER_POS_QUEUEREMOVE+ 1)
#define	EMITTER_POS_CASGET			(EMITTER_POS_CASINIT	+ 1)
#define	EMITTER_POS_CASSET			(EMITTER_POS_CASGET		+ 1)
#define	EMITTER_POS_CASINCDEC		(EMITTER_POS_CASSET		+ 1)

const char*	stc_emitters[] = {
	"init",
	"get",
	"getSubkeys",
	"getAttrs",
	"set",
	"setSubkeys",
	"setAll",
	"remove",
	"rename",
	"queuePush",
	"queuePop",
	"queueRemove",
	"casInit",
	"casGet",
	"casSet",
	"casIncDec",
	NULL
};

inline const char* GetNormalizationEmitter(const char* emitter)
{
	if(!emitter){
		return nullptr;
	}
	for(const char** ptmp = &stc_emitters[0]; ptmp && *ptmp; ++ptmp){
		if(0 == strcasecmp(*ptmp, emitter)){
			return *ptmp;
		}
	}
	return nullptr;
}

//---------------------------------------------------------
// Utility (using StackEmitCB Class)
//---------------------------------------------------------
static Napi::Value SetK2hdkcNodeCallback(const Napi::CallbackInfo& info, size_t pos, const char* pemitter)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode* obj = Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// check parameter
	if(info.Length() <= pos){
		Napi::TypeError::New(env, "No callback is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[pos].IsFunction()){
		Napi::TypeError::New(env, "The parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	Napi::Function cb = info[pos].As<Napi::Function>();

	// set
	bool result = obj->_cbs.Set(std::string(pemitter), cb);
	return Napi::Boolean::New(env, result);
}

static Napi::Value UnsetK2hdkcNodeCallback(const Napi::CallbackInfo& info, const char* pemitter)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode* obj = Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// unset
	bool result = obj->_cbs.Unset(std::string(pemitter));
	return Napi::Boolean::New(env, result);
}

//---------------------------------------------------------
// K2hdkcNode Class
//---------------------------------------------------------
Napi::FunctionReference	K2hdkcNode::constructor;

//---------------------------------------------------------
// Class method
//---------------------------------------------------------
//
// Parser for onetime connection
//
// info Arguments are following:
//	String	conf				configuration(must)
//	int		port				control port number for chmpx(optional, default CHM_INVALID_PORT)
//	String	cuk					configuration(optional, default empty)
//	bool	auto_rejoin			automatic rejoin flag value(optional, default false)
//	bool	no_giveup_rejoin	never giveup rejoin flag value(optional, default false)
//
// This macro gets argpos argument, which means next position for info value array.
//
bool K2hdkcNode::ParseArgumentsForOnetime(const Napi::CallbackInfo& info, K2hdkcNode* obj, size_t& argpos, std::string& conf, int16_t& ctlport, std::string& cuk, bool& auto_rejoin, bool& no_giveup_rejoin)
{
	if(obj && obj->IsInitialize()){
		return true;
	}

	Napi::Env env = info.Env();

	if(info.Length() < 2){
		Napi::TypeError::New(env, "There is no enough parameters for onetime connection.").ThrowAsJavaScriptException();
		return false;
	}

	// initialize
	conf.clear();
	ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	cuk.clear();
	auto_rejoin		= false;
	no_giveup_rejoin= false;

	// first param must be string
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "First parameter is not string").ThrowAsJavaScriptException();
		return false;
	}
	conf = info[argpos++].As<Napi::String>().Utf8Value();

	// after first
	if(argpos < info.Length()){
		if(info[argpos].IsNumber()){
			// 2'nd argument is control port
			ctlport	= static_cast<int16_t>(info[argpos++].As<Napi::Number>().Int32Value());

			if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
				// 3'rd argument is cuk
				if(info[argpos].IsString()){
					cuk = info[argpos++].As<Napi::String>().Utf8Value();
				}else{
					// keep cuk empty
					++argpos;
				}
				if(argpos < info.Length() && info[argpos].IsBoolean()){
					// 4'th argument is auto_rejoin
					auto_rejoin = info[argpos++].As<Napi::Boolean>().Value();

					if(argpos < info.Length() && info[argpos].IsBoolean()){
						// 5'th argument is no_giveup_rejoin
						no_giveup_rejoin = info[argpos++].As<Napi::Boolean>().Value();
					}
				}
			}else if(argpos < info.Length() && info[argpos].IsBoolean()){
				// 3'rd argument is auto_rejoin
				auto_rejoin = info[argpos++].As<Napi::Boolean>().Value();

				if(argpos < info.Length() && info[argpos].IsBoolean()){
					// 4'th argument is no_giveup_rejoin
					no_giveup_rejoin = info[argpos++].As<Napi::Boolean>().Value();
				}

			// cppcheck-suppress unmatchedSuppression
			// cppcheck-suppress multiCondition
			}else if(argpos < info.Length() && info[argpos].IsBoolean()){
				// 3'rd argument is no_giveup_rejoin
				no_giveup_rejoin = info[argpos++].As<Napi::Boolean>().Value();
			}
		}else if(info[argpos].IsString() || info[argpos].IsNull()){
			// 2'nd argument is cuk
			if(info[argpos].IsString()){
				cuk = info[argpos++].As<Napi::String>().Utf8Value();
			}else{
				// keep cuk empty
				++argpos;
			}
			if(argpos < info.Length() && info[argpos].IsBoolean()){
				// 3'rd argument is auto_rejoin
				auto_rejoin = info[argpos++].As<Napi::Boolean>().Value();

				if(argpos < info.Length() && info[argpos].IsBoolean()){
					// 4'th argument is no_giveup_rejoin
					no_giveup_rejoin = info[argpos++].As<Napi::Boolean>().Value();
				}
			}
		}else if(argpos < info.Length() && info[argpos].IsBoolean()){
			// 2'nd argument is auto_rejoin
			auto_rejoin = info[argpos++].As<Napi::Boolean>().Value();

			if(argpos < info.Length() && info[argpos].IsBoolean()){
				// 3'rd argument is no_giveup_rejoin
				no_giveup_rejoin = info[argpos++].As<Napi::Boolean>().Value();
			}

		// cppcheck-suppress unmatchedSuppression
		// cppcheck-suppress multiCondition
		}else if(argpos < info.Length() && info[argpos].IsBoolean()){
			// 2'nd argument is no_giveup_rejoin
			no_giveup_rejoin = info[argpos++].As<Napi::Boolean>().Value();
		}
	}
	return true;
}

//---------------------------------------------------------
// K2hdkcNode Methods
//---------------------------------------------------------
K2hdkcNode::K2hdkcNode(const Napi::CallbackInfo& info) : Napi::ObjectWrap<K2hdkcNode>(info), _cbs(), _k2hdkcslave()
{
	// [NOTE]
	// Perhaps due to an initialization order issue, these
	// chmpx debug environment variable settings don't work.
	// So, load the environment variables and set the debug
	// mode/file settings here.
	//
	const char* k2hdkcdbgmode = std::getenv("DKCDBGMODE");
	const char* k2hdkcdbgfile = std::getenv("DKCDBGFILE");
	if(k2hdkcdbgmode && k2hdkcdbgfile){
		if(0 == strcasecmp(k2hdkcdbgmode, "SLT") || 0 == strcasecmp(k2hdkcdbgmode, "SILENT")){
			k2hdkc_set_debug_level_silent();
		}else if(0 == strcasecmp(k2hdkcdbgmode, "ERR") || 0 == strcasecmp(k2hdkcdbgmode, "ERROR")){
			k2hdkc_set_debug_level_error();
		}else if(0 == strcasecmp(k2hdkcdbgmode, "WARNING") || 0 == strcasecmp(k2hdkcdbgmode, "WARN") || 0 == strcasecmp(k2hdkcdbgmode, "WAN")){
			k2hdkc_set_debug_level_warning();
		}else if(0 == strcasecmp(k2hdkcdbgmode, "INFO") || 0 == strcasecmp(k2hdkcdbgmode, "INF") || 0 == strcasecmp(k2hdkcdbgmode, "MSG")){
			k2hdkc_set_debug_level_message();
		}else if(0 == strcasecmp(k2hdkcdbgmode, "DUMP") || 0 == strcasecmp(k2hdkcdbgmode, "DMP")){
			k2hdkc_set_debug_level_dump();
		}
		k2hdkc_set_debug_file(k2hdkcdbgfile);		// Ignore any errors that occur.
	}
}

K2hdkcNode::~K2hdkcNode()
{
	Clean();
}

void K2hdkcNode::Clean()
{
	_k2hdkcslave.Clean(true);
}

bool K2hdkcNode::IsInitialize()
{
	// [NOTE]
	// Should use K2hdkcSlave::IsInitialize(), but it's protected,
	// so we use GetChmCntrlObject() instead.
	//
	return (NULL != _k2hdkcslave.GetChmCntrlObject());
}

void K2hdkcNode::Init(Napi::Env env, Napi::Object exports)
{
	Napi::Function funcs = DefineClass(env, "K2hdkcNode", {
		// DefineClass normally handles the constructor internally. Therefore, there is no need
		// to include a static wrapper New() in the class prototype, which works the same way as
		// when using NAN.
		// For reference, the following example shows how to declare New as a static method.
		// (Registration is not normally required.)
		//
		//	K2hdkcNode::InstanceMethod("new", 			&K2hdkcNode::New),

		K2hdkcNode::InstanceMethod("on",				&K2hdkcNode::On),
		K2hdkcNode::InstanceMethod("onInit",			&K2hdkcNode::OnInit),
		K2hdkcNode::InstanceMethod("onGet",				&K2hdkcNode::OnGet),
		K2hdkcNode::InstanceMethod("onGetSubkeys",		&K2hdkcNode::OnGetSubkeys),
		K2hdkcNode::InstanceMethod("onGetAttrs",		&K2hdkcNode::OnGetAttrs),
		K2hdkcNode::InstanceMethod("onSet",				&K2hdkcNode::OnSet),
		K2hdkcNode::InstanceMethod("onSetSubkeys",		&K2hdkcNode::OnSetSubkeys),
		K2hdkcNode::InstanceMethod("onSetAll",			&K2hdkcNode::OnSetAll),
		K2hdkcNode::InstanceMethod("onRemove",			&K2hdkcNode::OnRemove),
		K2hdkcNode::InstanceMethod("onRename",			&K2hdkcNode::OnRename),
		K2hdkcNode::InstanceMethod("onQueuePush",		&K2hdkcNode::OnQueuePush),
		K2hdkcNode::InstanceMethod("onQueuePop",		&K2hdkcNode::OnQueuePop),
		K2hdkcNode::InstanceMethod("onQueueRemove",		&K2hdkcNode::OnQueueRemove),
		K2hdkcNode::InstanceMethod("onQueueDel",		&K2hdkcNode::OnQueueRemove),
		K2hdkcNode::InstanceMethod("onCasInit",			&K2hdkcNode::OnCasInit),
		K2hdkcNode::InstanceMethod("onCasGet",			&K2hdkcNode::OnCasGet),
		K2hdkcNode::InstanceMethod("onCasSet",			&K2hdkcNode::OnCasSet),
		K2hdkcNode::InstanceMethod("onCasIncDec",		&K2hdkcNode::OnCasIncDec),
		K2hdkcNode::InstanceMethod("off",				&K2hdkcNode::Off),
		K2hdkcNode::InstanceMethod("offInit",			&K2hdkcNode::OffInit),
		K2hdkcNode::InstanceMethod("offGet",			&K2hdkcNode::OffGet),
		K2hdkcNode::InstanceMethod("offGetSubkeys",		&K2hdkcNode::OffGetSubkeys),
		K2hdkcNode::InstanceMethod("offGetAttrs",		&K2hdkcNode::OffGetAttrs),
		K2hdkcNode::InstanceMethod("offSet",			&K2hdkcNode::OffSet),
		K2hdkcNode::InstanceMethod("offSetSubkeys",		&K2hdkcNode::OffSetSubkeys),
		K2hdkcNode::InstanceMethod("offSetAll",			&K2hdkcNode::OffSetAll),
		K2hdkcNode::InstanceMethod("offRemove",			&K2hdkcNode::OffRemove),
		K2hdkcNode::InstanceMethod("offRename",			&K2hdkcNode::OffRename),
		K2hdkcNode::InstanceMethod("offQueuePush",		&K2hdkcNode::OffQueuePush),
		K2hdkcNode::InstanceMethod("offQueuePop",		&K2hdkcNode::OffQueuePop),
		K2hdkcNode::InstanceMethod("offQueueRemove",	&K2hdkcNode::OffQueueRemove),
		K2hdkcNode::InstanceMethod("offQueueDel",		&K2hdkcNode::OffQueueRemove),
		K2hdkcNode::InstanceMethod("offCasInit",		&K2hdkcNode::OffCasInit),
		K2hdkcNode::InstanceMethod("offCasGet",			&K2hdkcNode::OffCasGet),
		K2hdkcNode::InstanceMethod("offCasSet",			&K2hdkcNode::OffCasSet),
		K2hdkcNode::InstanceMethod("offCasIncDec",		&K2hdkcNode::OffCasIncDec),

		K2hdkcNode::InstanceMethod("init",				&K2hdkcNode::Init),
		K2hdkcNode::InstanceMethod("clean",				&K2hdkcNode::Clean),
		K2hdkcNode::InstanceMethod("clear",				&K2hdkcNode::Clean),
		K2hdkcNode::InstanceMethod("isPermanent",		&K2hdkcNode::IsPermanent),
		K2hdkcNode::InstanceMethod("getValue",			&K2hdkcNode::GetValue),
		K2hdkcNode::InstanceMethod("getSubkeys",		&K2hdkcNode::GetSubkeys),
		K2hdkcNode::InstanceMethod("getAttrs",			&K2hdkcNode::GetAttrs),
		K2hdkcNode::InstanceMethod("setValue",			&K2hdkcNode::SetValue),
		K2hdkcNode::InstanceMethod("setSubkeys",		&K2hdkcNode::SetSubkeys),
		K2hdkcNode::InstanceMethod("addSubkey",			&K2hdkcNode::SetValue),
		K2hdkcNode::InstanceMethod("clearSubkeys",		&K2hdkcNode::SetSubkeys),
		K2hdkcNode::InstanceMethod("setAll",			&K2hdkcNode::SetAll),
		K2hdkcNode::InstanceMethod("remove",			&K2hdkcNode::Remove),
		K2hdkcNode::InstanceMethod("rename",			&K2hdkcNode::Rename),
		K2hdkcNode::InstanceMethod("queuePush",			&K2hdkcNode::QueuePush),
		K2hdkcNode::InstanceMethod("queuePop",			&K2hdkcNode::QueuePop),
		K2hdkcNode::InstanceMethod("queueRemove",		&K2hdkcNode::QueueRemove),
		K2hdkcNode::InstanceMethod("queueDel",			&K2hdkcNode::QueueRemove),
		K2hdkcNode::InstanceMethod("casInit",			&K2hdkcNode::CasInit),
		K2hdkcNode::InstanceMethod("casGet",			&K2hdkcNode::CasGet),
		K2hdkcNode::InstanceMethod("casSet",			&K2hdkcNode::CasSet),
		K2hdkcNode::InstanceMethod("casIncDec",			&K2hdkcNode::CasIncDec),
		K2hdkcNode::InstanceMethod("printVersion",		&K2hdkcNode::PrintVersion)
	});

	constructor = Napi::Persistent(funcs);
	constructor.SuppressDestruct();

	// [NOTE]
	// do NOT do exports.Set("K2hdkcNode", func) here if InitAll will return createFn.
	//
}

Napi::Object K2hdkcNode::NewInstance(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::EscapableHandleScope scope(env);

    Napi::Object	obj			= constructor.Value().New({}).As<Napi::Object>();
	K2hdkcNode*		pk2hdkcobj	= K2hdkcNode::Unwrap(obj.As<Napi::Object>());

	if(0 < info.Length()){
		// called with arguments
		//
		// [NOTE] this logic as same as Init() except callback function parameter
		//
		size_t		argpos			= 0;
		std::string	conf;
		int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
		std::string	cuk;
		bool		auto_rejoin		= false;
		bool		no_giveup_rejoin= false;

		// parse arguments
		if(!K2hdkcNode::ParseArgumentsForOnetime(info, pk2hdkcobj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		    return scope.Escape(obj).As<Napi::Object>();
		}
		// check over arguments
		if(argpos < info.Length()){
			Napi::TypeError::New(env, "Too many parameter.").ThrowAsJavaScriptException();
		    return scope.Escape(obj).As<Napi::Object>();
		}

		// build permanent connection object
		if(!pk2hdkcobj->_k2hdkcslave.Initialize(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin)){
			pk2hdkcobj->Clean();
			Napi::Error::New(env, "Could not initialize k2hdkc slave object").ThrowAsJavaScriptException();
		    return scope.Escape(obj).As<Napi::Object>();
		}
		if(!pk2hdkcobj->_k2hdkcslave.Open(no_giveup_rejoin)){
			pk2hdkcobj->Clean();
			Napi::Error::New(env, "Could not open msgid by k2hdkc slave object").ThrowAsJavaScriptException();
		    return scope.Escape(obj).As<Napi::Object>();
		}
	}
    return scope.Escape(obj).As<Napi::Object>();
}

Napi::Object K2hdkcNode::GetInstance(const Napi::CallbackInfo& info)
{
	if(0 < info.Length()){
		return K2hdkcNode::constructor.New({info[0]});
	}else{
		return K2hdkcNode::constructor.New({});
	}
}

/**
 * @mainpage k2hdkc_nodejs
 */

/// \defgroup nodejs_methods	the methods for using from node.js
//@{

/**
 * @memberof K2hdkcNode
 * @fn void\
 * On(\
 * 	String	emitter\
 * 	, Callback cbfunc\
 * )
 * @brief	set callback handling
 *
 * @param[in] emitter			Specify emitter name
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::On(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No handle emitter name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else if(info.Length() < 2){
		Napi::TypeError::New(env, "No callback is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// check emitter name
	std::string emitter  = info[0].ToString().Utf8Value();
	const char* pemitter = GetNormalizationEmitter(emitter.c_str());
	if(!pemitter){
		std::string	msg	= "Unknown ";
		msg				+= emitter;
		msg				+= " emitter";
		Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// add callback
	return SetK2hdkcNodeCallback(info, 1, pemitter);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnInit(void)
 *
 * @brief	set callback handling for initializing k2hdkc object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnInit(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_INIT]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnGet(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnGet(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_GET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnGetSubkeys(void)
 *
 * @brief	set callback handling for getting value
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnGetSubkeys(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_GETSUBKEYS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnGetAttrs(void)
 *
 * @brief	set callback handling for getting attributes
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnGetAttrs(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_GETATTRS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnSet(void)
 *
 * @brief	set callback handling for setting value
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnSet(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_SET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnSetSubkeys(void)
 *
 * @brief	set callback handling for setting subkeys
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnSetSubkeys(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_SETSUBKEYS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnSetAll(void)
 *
 * @brief	set callback handling for setting all value
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnSetAll(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_SETALL]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnRemove(void)
 *
 * @brief	set callback handling for removing
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnRemove(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_REMOVE]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnRename(void)
 *
 * @brief	set callback handling for renaming
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnRename(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_RENAME]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnQueuePush(void)
 *
 * @brief	set callback handling for pushing queue
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnQueuePush(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_QUEUEPUSH]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnQueuePop(void)
 *
 * @brief	set callback handling for popping queue
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnQueuePop(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_QUEUEPOP]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnQueueRemove(void)
 *
 * @brief	set callback handling for removing queue
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnQueueRemove(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_QUEUEREMOVE]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnCasInit(void)
 *
 * @brief	set callback handling for initializing CAS
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnCasInit(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_CASINIT]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnCasGet(void)
 *
 * @brief	set callback handling for getting CAS
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnCasGet(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_CASGET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnCasSet(void)
 *
 * @brief	set callback handling for setting CAS
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnCasSet(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_CASSET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnCasIncDec(void)
 *
 * @brief	set callback handling for increment/decrement CAS
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OnCasIncDec(const Napi::CallbackInfo& info)
{
	return SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_CASINCDEC]);
}

/**
 * @memberof K2hdkcNode
 * @fn void\
 * Off(\
 * 	String	emitter\
 * )
 *
 * @brief	set callback handling
 *
 * @param[in] emitter			Specify emitter name
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::Off(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No handle emitter name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// check emitter name
	std::string	emitter  = info[0].ToString().Utf8Value();
	const char*	pemitter = GetNormalizationEmitter(emitter.c_str());
	if (nullptr == pemitter) {
		std::string msg	= "Unknown ";
		msg				+= emitter;
		msg				+= " emitter";
		Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// unset callback
	return UnsetK2hdkcNodeCallback(info, pemitter);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffInit(void)
 *
 * @brief	unset callback handling for initializing k2hdkc object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffInit(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_INIT]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffGet(void)
 *
 * @brief	unset callback handling for getting value
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffGet(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_GET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffGetSubkeys(void)
 *
 * @brief	unset callback handling for getting subkeys
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffGetSubkeys(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_GETSUBKEYS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffGetAttrs(void)
 *
 * @brief	unset callback handling for getting attributes
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffGetAttrs(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_GETATTRS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffSet(void)
 *
 * @brief	unset callback handling for setting value
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffSet(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_SET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffSetSubkeys(void)
 *
 * @brief	unset callback handling for setting subkeys
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffSetSubkeys(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_SETSUBKEYS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffSetAll(void)
 *
 * @brief	unset callback handling for setting all values
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffSetAll(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_SETALL]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffRemove(void)
 *
 * @brief	unset callback handling for removing
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffRemove(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_REMOVE]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffRename(void)
 *
 * @brief	unset callback handling for renaming
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffRename(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_RENAME]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffQueuePush(void)
 *
 * @brief	unset callback handling for pushing queue
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffQueuePush(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_QUEUEPUSH]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffQueuePop(void)
 *
 * @brief	unset callback handling for popping queue
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffQueuePop(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_QUEUEPOP]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffQueueRemove(void)
 *
 * @brief	unset callback handling for removing queue
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffQueueRemove(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_QUEUEREMOVE]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffCasInit(void)
 *
 * @brief	unset callback handling for initializing CAS
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffCasInit(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_CASINIT]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffCasGet(void)
 *
 * @brief	unset callback handling for getting CAS
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffCasGet(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_CASGET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffCasSet(void)
 *
 * @brief	unset callback handling for setting CAS
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffCasSet(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_CASSET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffCasIncDec(void)
 *
 * @brief	unset callback handling for increment/decrement CAS
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hdkcNode::OffCasIncDec(const Napi::CallbackInfo& info)
{
	return UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_CASINCDEC]);
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool Init(\
 * void\
 * )
 *
 * @brief	Initialize parameters for K2hdkcNode object for onetime connection.
 *
 * @return	returns true for success, false for failure
 *
 * @fn bool Init(\
 * String	conf=null\
 * , int	port=CHM_INVALID_PORT\
 * , bool	auto_rejoin=false\
 * , bool	no_giveup_rejoin=false\
 * , Callback cbfunc=null\
 * )
 *
 * @brief	Initialize parameters for K2hdkcNode object for permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				If permanent connection mode, specify configuration
 * @param[in] port				If permanent connection mode, specify control port number for chmpx(optional)
 * @param[in] cuk				If permanent connection mode, specify cuk for chmpx(optional)
 * @param[in] auto_rejoin		If permanent connection mode, specify automatic rejoin flag value(optional)
 * @param[in] no_giveup_rejoin	If permanent connection mode, specify never giveup for rejoin flag value(optional)
 * @param[in] cbfunc			callback function.
 *
 * @return	returns true for success, false for failure
 *			If a callback is set, always return true.
 *
 */

Napi::Value K2hdkcNode::Init(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*	obj	= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_INIT]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// reinitialize at first
	obj->Clean();

	// default values
	std::string		conf("");
	int16_t			ctlport = static_cast<int16_t>(CHM_INVALID_PORT);
	std::string		cuk("");

	if(0 == info.Length()){
		if(hasCallback){
			// [NOTE]
			// If callback is set, it calls worker. But it returns error,
			// because no configuration is specified.
			//
			InitWorker*	worker = new InitWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, false, false);
			worker->Queue();
		}
		return Napi::Boolean::New(env, true);

	}else{
		// permanent connection type
		size_t	argpos			= 0;
		bool	auto_rejoin		= false;
		bool	no_giveup_rejoin= false;

		// parse arguments
		if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
			return env.Undefined();
		}
		if(argpos < info.Length() && info[argpos].IsFunction()){
			// last argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
		// check over arguments
		if(argpos < info.Length()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		// work
		if(hasCallback){
			InitWorker*	worker = new InitWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin);
			worker->Queue();
		}else{
			// build permanent connection object
			if(!obj->_k2hdkcslave.Initialize(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin)){
				obj->Clean();
				return Napi::Boolean::New(env, false);
			}
			if(!obj->_k2hdkcslave.Open(no_giveup_rejoin)){
				obj->Clean();
				return Napi::Boolean::New(env, false);
			}
		}
		return Napi::Boolean::New(env, true);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool Clean(\
 * void\
 * )
 *
 * @brief	Deinitialize object for K2hdkcNode.
 *			If it has K2hdkcSlave object and open connection, close and destruct it.
 *
 * @return	returns always true.
 *
 */

Napi::Value K2hdkcNode::Clean(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*	obj	= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	obj->Clean();
	return Napi::Boolean::New(env, true);
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool IsPermanent(\
 * void\
 * )
 *
 * @brief	Check K2hdkcNode object has permanent connection.
 *
 * @return	returns true or failure.
 *
 */

Napi::Value K2hdkcNode::IsPermanent(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*	obj	= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	bool result = obj->IsInitialize();
	return Napi::Boolean::New(env, result);
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool GetValue(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 * 	, String	subkey=""\
 * 	, bool		attrcheck=true\
 * 	, String	pass=""\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Get value for onetime connection.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name.
 * @param[in] subkey			Specify the subkey name when you want to get value for it.
 * @param[in] attrcheck			If this parameter is true, enable to check attributes which is presented by builtin.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] cbfunc			callback function.
 *
 * @return	Returns the value. If there is no key(subkey), returns null.
 *			Returns always true when the callback function is specified.
 *
 * @fn bool GetValue(\
 *  String		key\
 * 	, String	subkey=""\
 * 	, bool		attrcheck=true\
 * 	, String	pass=""\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Get value for permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key			Specify the key name.
 * @param[in] subkey		Specify the subkey name when you want to get value for it.
 * @param[in] attrcheck		If this parameter is true, enable to check attributes which is presented by builtin.
 * @param[in] pass			Specify optional passphrase if encrypting the key
 * @param[in] cbfunc		callback function.
 *
 * @return	Returns the value. If there is no key(subkey), returns null.
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::GetValue(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_GET]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strkey;
	std::string	strsubkey;
	bool		attrchk			= true;
	std::string	strpass;

	// target arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Napi::TypeError::New(env, "There is no key name parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(info[argpos].IsString()){
		strkey = info[argpos].As<Napi::String>().Utf8Value();
	}
	++argpos;

	// other arguments for target
	if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
		// target 2'nd parameter is subkey(string or null)
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			strsubkey		= info[argpos++].As<Napi::String>().Utf8Value();
		}

		if(argpos < info.Length() && info[argpos].IsBoolean()){
			// target 3'rd parameter is attrchk
			attrchk = info[argpos++].As<Napi::Boolean>().Value();

			if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
				// target 4'th parameter is pass
				if(info[argpos].IsNull()){
					argpos++;
				}else{
					strpass		= info[argpos++].As<Napi::String>().Utf8Value();
				}

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// target 5'th parameter is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}

			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 4'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}

		}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// target 3'rd parameter is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass		= info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 4'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}

		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 3'rd parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}

	}else if(argpos < info.Length() && info[argpos].IsBoolean()){
		// target 2'nd parameter is attrchk
		attrchk = info[argpos++].As<Napi::Boolean>().Value();

		if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// target 3'rd parameter is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass		= info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 4'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}

		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 3'rd parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}

	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// target 2'nd parameter is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		GetValueWorker*	worker = new GetValueWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey, strsubkey, attrchk, strpass);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		dkcres_type_t	rescode = DKC_NORESTYPE;
		if(!strsubkey.empty()){
			// subkey is specified, thus need to check the key has it.
			K2hdkcComGetSubkeys*	pSubComObj;
			if(!obj->IsInitialize()){
				pSubComObj = GetOtSlaveK2hdkcComGetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pSubComObj = GetPmSlaveK2hdkcComGetSubkeys(&(obj->_k2hdkcslave));
			}
			if(!pSubComObj){
				Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
				return env.Undefined();
			}

			// get subkey list in key
			K2HSubKeys*	pSubKeys= NULL;
			if(!pSubComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, attrchk, &pSubKeys, &rescode) || !pSubKeys){
				// key does not have any subkey
				DKC_DELETE(pSubComObj);
				return env.Null();
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
				return env.Null();
			}
			// switch key to subkey
			strkey = strsubkey;
		}

		// get value
		K2hdkcComGet*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComGet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComGet(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		const unsigned char*	pvaltmp		= NULL;
		size_t					valtmplen	= 0L;
		bool					result		= pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, attrchk, (strpass.empty() ? NULL : strpass.c_str()), &pvaltmp, &valtmplen, &rescode);
		if(result && (pvaltmp && 0 < valtmplen)){
			std::string	strresult(reinterpret_cast<const char*>(pvaltmp), ('\0' == pvaltmp[valtmplen - 1] ? valtmplen - 1 : valtmplen));
			DKC_DELETE(pComObj);
			return Napi::String::New(env, strresult);
		}else{
			DKC_DELETE(pComObj);
			return env.Null();
		}
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool GetSubkeys(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 * 	, bool		attrcheck=true\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Get subkeys for onetime connection.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name.
 * @param[in] attrcheck			If this parameter is true, enable to check attributes which is presented by builtin.
 * @param[in] cbfunc			callback function.
 *
 * @return	Returns the array. If there is no key(subkey), returns null.
 *			Returns always true when the callback function is specified.
 *
 * @fn bool GetSubkeys(\
 *  String		key\
 * 	, bool		attrcheck=true\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Get subkeys for permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key			Specify the key name.
 * @param[in] attrcheck		If this parameter is true, enable to check attributes which is presented by builtin.
 * @param[in] cbfunc		callback function.
 *
 * @return	Returns the array. If there is no key(subkey), returns null.
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::GetSubkeys(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_GETSUBKEYS]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strkey;
	bool		attrchk	= true;

	// target arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Napi::TypeError::New(env, "There is no key name parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strkey = info[argpos++].As<Napi::String>().Utf8Value();

	// other arguments for target
	if(argpos < info.Length() && info[argpos].IsBoolean()){
		// target 2'nd parameter is attrchk
		attrchk = info[argpos++].As<Napi::Boolean>().Value();

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 3'rd parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}

	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// target 2'nd parameter is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}
	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		GetSubkeysWorker*	worker = new GetSubkeysWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey, attrchk);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComGetSubkeys*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComGetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComGetSubkeys(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		// get subkey list in key
		dkcres_type_t	rescode = DKC_NORESTYPE;
		K2HSubKeys*		pSubKeys= NULL;
		if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, attrchk, &pSubKeys, &rescode) || !pSubKeys){
			// key does not have any subkey
			DKC_DELETE(pComObj);
			return env.Null();
		}else{
			// convert subkey binary data to string array
			strarr_t	strarr;
			pSubKeys->StringArray(strarr);
			DKC_DELETE(pSubKeys);

			Napi::Array retarr	= Napi::Array::New(env, strarr.size());
			uint32_t	pos		= 0;
			for(const auto& str: strarr){
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

			DKC_DELETE(pComObj);
			return retarr;
		}
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool GetAttrs(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Get attribute keys for onetime connection.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name.
 * @param[in] cbfunc			callback function.
 *
 * @return	If there are attributes, returns attribute names list. If not, returns null.
 *			Returns always true when the callback function is specified.
 *
 * @fn bool GetAttrs(\
 *  String		key\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Get attribute keys permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key			Specify the key name.
 * @param[in] cbfunc		callback function.
 *
 * @return	If there are attributes, returns attribute names list. If not, returns null.
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::GetAttrs(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_GETATTRS]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strkey;

	// target arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Napi::TypeError::New(env, "There is no key name parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strkey = info[argpos++].As<Napi::String>().Utf8Value();

	// other arguments for target
	if(argpos < info.Length() && info[argpos].IsFunction()){
		// target 2'nd parameter is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}
	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		GetAttrsWorker*	worker = new GetAttrsWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComGetAttrs*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComGetAttrs(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComGetAttrs(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		// get attribute list in key
		dkcres_type_t	rescode = DKC_NORESTYPE;
		K2HAttrs*		pAttrs	= NULL;
		if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, &pAttrs, &rescode) || !pAttrs){
			// key does not have any attribute key
			DKC_DELETE(pComObj);
			return env.Null();
		}else{
			// convert attribute key binary data to string array
			strarr_t	strarr;
			pAttrs->KeyStringArray(strarr);
			DKC_DELETE(pAttrs);

			Napi::Array retarr	= Napi::Array::New(env, strarr.size());
			uint32_t	pos		= 0;
			for(const auto& str: strarr){
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

			DKC_DELETE(pComObj);
			return retarr;
		}
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool SetValue(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 *  , String	value=""\
 * 	, String	subkey=""\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Set value into key or subkey for onetime connection.
 *			If the key is existed, the key's value is over wrote by new value.
 *			Then if the key has subkeys, those subkeys is not changed.
 *			If the subkey parameter is specified, new subkey is set in the key and
 *			the value is set to new subkey. Then the subkey is existed, the subkey's
 *			value is over write.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name
 * @param[in] value				Specify the value
 * @param[in] subkey			Specify the subkey name, if you want to set the value to subkey.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 * @fn bool SetValue(\
 *  String		key\
 *  , String	value=""\
 * 	, String	subkey=""\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Set value into key or subkey for permanent connection.
 *			If the key is existed, the key's value is over wrote by new value.
 *			Then if the key has subkeys, those subkeys is not changed.
 *			If the subkey parameter is specified, new subkey is set in the key and
 *			the value is set to new subkey. Then the subkey is existed, the subkey's
 *			value is over write.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key				Specify the key name
 * @param[in] value				Specify the value
 * @param[in] subkey			Specify the subkey name, if you want to set the value to subkey.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::SetValue(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_SET]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strkey;
	std::string	strval;
	std::string	strsubkey;
	std::string	strpass;
	time_t		expire = 0;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= (argpos + 1)){
		Napi::TypeError::New(env, "There is no key name nor value parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strkey = info[argpos++].As<Napi::String>().Utf8Value();

	// target value arguments(for both ontime and permanent connection)
	if(info[argpos].IsString()){
		strval = info[argpos++].As<Napi::String>().Utf8Value();
	}else{
		argpos++;
	}

	// other arguments for target
	if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
		// target 2'nd parameter is subkey(string or null)
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			strsubkey = info[argpos++].As<Napi::String>().Utf8Value();
		}

		if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// target 3'rd parameter is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsNumber()){
				// target 4'th parameter is expire
				int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// target 5'th parameter is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 4'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsNumber()){
			// target 3'rd parameter is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 4'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 3'rd parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsNumber()){
		// target 2'nd parameter is expire
		int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
		expire			= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 3'rd parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// target 2'nd parameter is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		SetValueWorker*	worker = new SetValueWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey, strval, strsubkey, strpass, expire);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		dkcres_type_t	rescode = DKC_NORESTYPE;
		bool			result	= false;
		if(!strsubkey.empty()){
			// subkey is specified, set value into subkey
			K2hdkcComAddSubkey*	pComObj;
			if(!obj->IsInitialize()){
				pComObj = GetOtSlaveK2hdkcComAddSubkey(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComAddSubkey(&(obj->_k2hdkcslave));
			}
			if(!pComObj){
				Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(strsubkey.c_str()), strsubkey.length() + 1, (strval.empty() ? NULL : reinterpret_cast<const unsigned char*>(strval.c_str())), (strval.empty() ? 0 : strval.length() + 1), true, (strpass.empty() ? NULL : strpass.c_str()), (expire > 0 ? &expire : NULL), &rescode);
			DKC_DELETE(pComObj);
		}else{
			// set value to key
			K2hdkcComSet*	pComObj;
			if(!obj->IsInitialize()){
				pComObj = GetOtSlaveK2hdkcComSet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComSet(&(obj->_k2hdkcslave));
			}
			if(!pComObj){
				Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, (strval.empty() ? NULL : reinterpret_cast<const unsigned char*>(strval.c_str())), (strval.empty() ? 0 : strval.length() + 1), false, (strpass.empty() ? NULL : strpass.c_str()), (expire > 0 ? &expire : NULL), &rescode);
			DKC_DELETE(pComObj);
		}
		return Napi::Boolean::New(env, result);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool SetSubkeys(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 *  , Array		subkeys=""\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Set/Clear subkey list for onetime connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name
 * @param[in] subkeys			Specify the subkeys array. if null is set to subkeys, subkeys is cleared
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 * @fn bool SetSubkeys(\
 *  String		key\
 *  , Array		subkeys=""\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Set/Clear subkey list for permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key				Specify the key name
 * @param[in] subkeys			Specify the subkeys array. if null is set to subkeys, subkeys is cleared
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::SetSubkeys(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_SETSUBKEYS]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string		strkey;
	unsigned char*	bysubkeys	= NULL;
	size_t			skeylen		= 0UL;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Napi::TypeError::New(env, "There is no key name parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strkey = info[argpos++].As<Napi::String>().Utf8Value();

	// other arguments for target
	if(argpos < info.Length() && (info[argpos].IsArray() || info[argpos].IsNull())){
		// target 2'nd parameter is subkeys(array or null)
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			// Array to binary
			Napi::Array	inSubkeys = info[argpos++].As<Napi::Array>();
			K2HSubKeys	Subkeys;
			for(uint32_t pos = 0; pos < static_cast<uint32_t>(inSubkeys.Length()); ++pos){
				Napi::Value	oneVal = inSubkeys.Get(pos);
				if(!oneVal.IsString()){
					Napi::TypeError::New(env, "subkey element must be string").ThrowAsJavaScriptException();
					return env.Undefined();
				}
				std::string	tmpkey = oneVal.As<Napi::String>().Utf8Value();

				if(Subkeys.end() == Subkeys.insert(tmpkey.c_str())){
					// failed to set subkey
					return Napi::Boolean::New(env, false);
				}
			}

			// serialize
			if(0UL < Subkeys.size()){
				if(!Subkeys.Serialize(&bysubkeys, skeylen)){
					// failed to set subkey
					return Napi::Boolean::New(env, false);
				}
			}
		}

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 3'rd parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}

	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// target 2'nd parameter is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		SetSubkeysWorker*	worker = new SetSubkeysWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey, bysubkeys, skeylen);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComSetSubkeys*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComSetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComSetSubkeys(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
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
		return Napi::Boolean::New(env, result);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool SetAll(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 *  , String	value=""\
 *  , Array		subkeys=""\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Set value/subkey list for onetime connection.
 *			If the key is existed, the key's value is over wrote by new value.
 *			Then if the key has subkeys, those subkeys is not changed.
 *			If the subkey parameter is specified, new subkey is set in the key and
 *			the value is set to new subkey. Then the subkey is existed, the subkey's
 *			value is over write.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name
 * @param[in] value				Specify the value
 * @param[in] subkeys			Specify the subkeys array, if you want to set subkeys.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 * @fn bool SetAll(\
 *  String		key\
 *  , String	value\
 *  , Array		subkeys=""\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Set value/subkey list for permanent connection.
 *			If the key is existed, the key's value is over wrote by new value.
 *			Then if the key has subkeys, those subkeys is not changed.
 *			If the subkey parameter is specified, new subkey is set in the key and
 *			the value is set to new subkey. Then the subkey is existed, the subkey's
 *			value is over write.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key				Specify the key name
 * @param[in] value				Specify the value
 * @param[in] subkeys			Specify the subkeys array, if you want to set subkeys.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::SetAll(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_SETALL]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string		strkey;
	std::string		strval;
	unsigned char*	bysubkeys	= NULL;
	size_t			skeylen		= 0UL;
	std::string		strpass;
	time_t			expire		= 0;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= (argpos + 1)){
		Napi::TypeError::New(env, "There is no key name nor value parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strkey = info[argpos++].As<Napi::String>().Utf8Value();

	// target value arguments(for both ontime and permanent connection)
	if(info[argpos].IsString()){
		strval = info[argpos++].As<Napi::String>().Utf8Value();
	}else if(info[argpos].IsNull()){
		argpos++;
	}

	// other arguments for target
	if(argpos < info.Length() && (info[argpos].IsArray() || info[argpos].IsNull())){
		// target 2'nd parameter is subkeys(array or null)
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			// Array to binary
			Napi::Array	inSubkeys = info[argpos++].As<Napi::Array>();
			K2HSubKeys	Subkeys;
			for(uint32_t pos = 0; pos < static_cast<uint32_t>(inSubkeys.Length()); ++pos){
				Napi::Value	oneVal = inSubkeys.Get(pos);
				if(!oneVal.IsString()){
					Napi::TypeError::New(env, "subkey element must be string").ThrowAsJavaScriptException();
					return env.Undefined();
				}
				std::string	tmpkey = oneVal.As<Napi::String>().Utf8Value();

				if(Subkeys.end() == Subkeys.insert(tmpkey.c_str())){
					// failed to set subkey
					return Napi::Boolean::New(env, false);
				}
			}

			// serialize
			if(0UL < Subkeys.size()){
				if(!Subkeys.Serialize(&bysubkeys, skeylen)){
					// failed to set subkey
					return Napi::Boolean::New(env, false);
				}
			}
		}

		if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// target 3'rd parameter is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsNumber()){
				// target 4'th parameter is expire
				int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// target 5'th parameter is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}

			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 4'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}

		}else if(argpos < info.Length() && info[argpos].IsNumber()){
			// target 3'rd parameter is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 4'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}

		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 3'rd parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}

	}else if(argpos < info.Length() && info[argpos].IsNumber()){
		// target 2'nd parameter is expire
		int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
		expire			= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 3'rd parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}

	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// target 2'nd parameter is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		SetAllWorker*	worker = new SetAllWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey, strval, bysubkeys, skeylen, strpass, expire);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool			result	= false;
		dkcres_type_t	rescode = DKC_NORESTYPE;
		if(!strpass.empty() && 0 < expire){
			// set value with passphrase and expire, then the operation is separated.
			K2hdkcComSet*	pComObj;
			if(!obj->IsInitialize()){
				pComObj = GetOtSlaveK2hdkcComSet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComSet(&(obj->_k2hdkcslave));
			}
			if(!pComObj){
				Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
				return env.Undefined();
			}

			// set value to key
			result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, (strval.empty() ? NULL : reinterpret_cast<const unsigned char*>(strval.c_str())), (strval.empty() ? 0 : strval.length() + 1), false, strpass.c_str(), &expire, &rescode);
			DKC_DELETE(pComObj);

			// set subkeys
			if(result && bysubkeys && 0UL < skeylen){
				K2hdkcComSetSubkeys*	pSubComObj;
				if(!obj->IsInitialize()){
					pSubComObj = GetOtSlaveK2hdkcComSetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
				}else{
					pSubComObj = GetPmSlaveK2hdkcComSetSubkeys(&(obj->_k2hdkcslave));
				}
				if(!pSubComObj){
					Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
					return env.Undefined();
				}

				// set subkeys to key
				result = pSubComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, bysubkeys, skeylen, &rescode);
				DKC_DELETE(pSubComObj);
			}

		}else{
			// no passphrase and no expire, then one action
			K2hdkcComSetAll*	pComObj;
			if(!obj->IsInitialize()){
				pComObj = GetOtSlaveK2hdkcComSetAll(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComSetAll(&(obj->_k2hdkcslave));
			}
			if(!pComObj){
				Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			// set value and subkeys
			result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, (strval.empty() ? NULL : reinterpret_cast<const unsigned char*>(strval.c_str())), (strval.empty() ? 0 : strval.length() + 1), bysubkeys, skeylen, NULL, 0UL, &rescode);
			DKC_DELETE(pComObj);
		}
		return Napi::Boolean::New(env, result);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool Remove(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 * 	, bool		is_del_subkeys=false\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Remove key for onetime connection.
 *			If the flag of subkey is true, removes all subkey.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name
 * @param[in] is_del_subkeys	Specify optional flag for removing subkey
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 * @fn bool Remove(\
 *  String		key\
 * 	, bool		is_del_subkeys=false\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Remove key for permanent connection.
 *			If the flag of subkey is true, removes all subkey.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key				Specify the key name
 * @param[in] is_del_subkeys	Specify optional flag for removing subkey
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::Remove(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_REMOVE]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strkey;
	bool		is_subkeys = false;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Napi::TypeError::New(env, "There is no key name parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strkey = info[argpos++].As<Napi::String>().Utf8Value();

	// other arguments for target
	if(argpos < info.Length() && info[argpos].IsBoolean()){
		// target 2'nd parameter is subkey flag
		is_subkeys = info[argpos++].As<Napi::Boolean>().Value();

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 3'rd parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// target 2'nd parameter is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		RemoveWorker*	worker = new RemoveWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey, is_subkeys);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComDel*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComDel(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComDel(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		// remove key
		dkcres_type_t	rescode = DKC_NORESTYPE;
		bool			result	= pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, is_subkeys, true, &rescode);

		DKC_DELETE(pComObj);
		return Napi::Boolean::New(env, result);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool Rename(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	oldkey\
 *  , String	newkey\
 *  , String	parentkey=""\
 * 	, bool		attrcheck=true\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Rename key name for onetime connection.
 *			If parent key is specified, changes parent's subkey list.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] oldkey			Specify the source(old) key name
 * @param[in] newkey			Specify the destination(new) key name
 * @param[in] parentkey			Specify optional destination(new) parent key name
 * @param[in] attrcheck			If this parameter is true, enable to check attributes which is presented by builtin.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 * @fn bool Rename(\
 *  String	oldkey\
 *  , String	newkey\
 *  , String	parentkey=""\
 * 	, bool		attrcheck=true\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Rename key name for permanent connection.
 *			If parent key is specified, changes parent's subkey list.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] oldkey			Specify the source(old) key name
 * @param[in] newkey			Specify the destination(new) key name
 * @param[in] parentkey			Specify optional destination(new) parent key name
 * @param[in] attrcheck			If this parameter is true, enable to check attributes which is presented by builtin.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::Rename(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_RENAME]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strold;
	std::string	strnew;
	std::string	strparent;
	std::string	strpass;
	bool		attrchk	= true;
	time_t		expire	= 0;

	// target old key arguments(for both ontime and permanent connection)
	if(info.Length() <= (argpos + 1)){
		Napi::TypeError::New(env, "There is no key name nor value parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strold = info[argpos++].As<Napi::String>().Utf8Value();

	// target new key arguments(for both ontime and permanent connection)
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strnew = info[argpos++].As<Napi::String>().Utf8Value();

	// other arguments for target
	if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
		// target 3'rd parameter is parent key
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			strparent = info[argpos++].As<Napi::String>().Utf8Value();
		}

		if(argpos < info.Length() && info[argpos].IsBoolean()){
			// target 4'th parameter is attrchk
			attrchk = info[argpos++].As<Napi::Boolean>().Value();

			if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
				// target 5'th parameter is pass
				if(info[argpos].IsNull()){
					argpos++;
				}else{
					strpass = info[argpos++].As<Napi::String>().Utf8Value();
				}

				if(argpos < info.Length() && info[argpos].IsNumber()){
					// target 6'th parameter is expire
					int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
					expire			= static_cast<time_t>(nexpire);

					if(argpos < info.Length() && info[argpos].IsFunction()){
						// target 7'th parameter is callback
						maybeCallback	= info[argpos++].As<Napi::Function>();
						hasCallback		= true;
					}
				}else if(argpos < info.Length() && info[argpos].IsFunction()){
					// target 6'th parameter is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsNumber()){
				// target 5'th parameter is expire
				int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// target 6'th parameter is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 5'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// target 4'th parameter is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsNumber()){
				// target 5'th parameter is expire
				int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// target 6'th parameter is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 5'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsNumber()){
			// target 4'th parameter is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 5'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 4'th parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsBoolean()){
		// target 3'rd parameter is attrchk
		attrchk = info[argpos++].As<Napi::Boolean>().Value();

		if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// target 4'th parameter is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsNumber()){
				// target 5'th parameter is expire
				int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// target 6'th parameter is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 5'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsNumber()){
			// target 4'th parameter is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// target 5'th parameter is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 4'th parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsNumber()){
		// target 3'rd parameter is expire
		int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
		expire			= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// target 4'th parameter is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// target 3'rd parameter is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		RenameWorker*	worker = new RenameWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strold, strnew, strparent, attrchk, strpass, expire);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComRen*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComRen(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComRen(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		dkcres_type_t	rescode = DKC_NORESTYPE;
		bool			result	= pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strold.c_str()), strold.length() + 1, reinterpret_cast<const unsigned char*>(strnew.c_str()), strnew.length() + 1, (strparent.empty() ? NULL : reinterpret_cast<const unsigned char*>(strparent.c_str())), (strparent.empty() ? 0 : strparent.length() + 1), attrchk, (strpass.empty() ? NULL : strpass.c_str()), (expire > 0 ? &expire : NULL), &rescode);

		DKC_DELETE(pComObj);
		return Napi::Boolean::New(env, result);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool QueuePush(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	prefix\
 *  , String	key=""\
 *  , String	val\
 * 	, bool		is_fifo=true\
 * 	, bool		attrcheck=true\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Push key and value(or only value) to queue for onetime connection.
 *			If key is empty, pushes only value. Otherwise pushes key and value.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] prefix			Specify prefix name for queue
 * @param[in] key				Specify optional key name which is pushed to queue.
 * @param[in] val				Specify value which is pushed to queue.
 * @param[in] is_fifo			Specify optional queue mode fifo or lifo.
 * @param[in] attrcheck			If this parameter is true, enable to check attributes which is presented by builtin.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 * @fn bool QueuePush(\
 *  String		prefix\
 *  , String	key=""\
 *  , String	val\
 * 	, bool		is_fifo=true\
 * 	, bool		attrcheck=true\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Push key and value(or only value) to queue for permanent connection.
 *			If key is empty, pushes only value. Otherwise pushes key and value.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] prefix			Specify prefix name for queue
 * @param[in] key				Specify optional key name which is pushed to queue.
 * @param[in] val				Specify value which is pushed to queue.
 * @param[in] is_fifo			Specify optional queue mode fifo or lifo.
 * @param[in] attrcheck			If this parameter is true, enable to check attributes which is presented by builtin.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::QueuePush(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_QUEUEPUSH]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strprefix;
	std::string	strkey;
	std::string	strval;
	bool		is_fifo	= true;
	bool		attrchk	= true;
	std::string	strpass;
	time_t		expire	= 0;

	// target prefix
	if(info.Length() <= (argpos + 1)){
		Napi::TypeError::New(env, "There is no prefix nor value parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The prefix name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strprefix = info[argpos++].As<Napi::String>().Utf8Value();

	// target (key and val) or val
	if(info[argpos].IsString()){
		if((argpos + 1) < info.Length() && (info[argpos + 1].IsString() || info[argpos + 1].IsNull())){
			// 2'nd argument is key, 3'rd argument is value.
			strkey = info[argpos++].As<Napi::String>().Utf8Value();
			strval = info[argpos++].As<Napi::String>().Utf8Value();
		}else{
			// 2'nd argument is value(key is not set).
			strval = info[argpos++].As<Napi::String>().Utf8Value();
		}
	}else if(info[argpos].IsNull()){
		if((argpos + 1) < info.Length() && (info[argpos + 1].IsString() || info[argpos + 1].IsNull())){
			// 2'nd argument is key( = null), 3'rd argument is value.
			argpos++;
			strval = info[argpos++].As<Napi::String>().Utf8Value();
		}else{
			Napi::TypeError::New(env, "The key name or value must be string.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
	}else{
		Napi::TypeError::New(env, "The key name or value must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// other arguments for target
	if(argpos < info.Length() && info[argpos].IsBoolean()){
		// 3'rd argument is fifo/lifo mode
		is_fifo = info[argpos++].As<Napi::Boolean>().Value();

		if(argpos < info.Length() && info[argpos].IsBoolean()){
			// 4'th argument is attrchk
			attrchk = info[argpos++].As<Napi::Boolean>().Value();

			if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
				// 5'th argument is pass
				if(info[argpos].IsNull()){
					argpos++;
				}else{
					strpass = info[argpos++].As<Napi::String>().Utf8Value();
				}

				if(argpos < info.Length() && info[argpos].IsNumber()){
					// 6'th argument is expire
					int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
					expire			= static_cast<time_t>(nexpire);

					if(argpos < info.Length() && info[argpos].IsFunction()){
						// 7'th argument is callback
						maybeCallback	= info[argpos++].As<Napi::Function>();
						hasCallback		= true;
					}
				}else if(argpos < info.Length() && info[argpos].IsFunction()){
					// 6'th argument is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsNumber()){
				// 5'th argument is expire
				int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// 6'th argument is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// 5'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// 4'th argument is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsNumber()){
				// 5'th argument is expire
				int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// 6'th argument is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// 5'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsNumber()){
			// 4'th argument is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 5'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 4'th argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}

	// cppcheck-suppress unmatchedSuppression
	// cppcheck-suppress multiCondition
	}else if(argpos < info.Length() && info[argpos].IsBoolean()){
		// 3'rd argument is attrchk
		attrchk = info[argpos++].As<Napi::Boolean>().Value();

		if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// 4'th argument is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsNumber()){
				// 5'th argument is expire
				int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// 6'th argument is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// 5'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsNumber()){
			// 4'th argument is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 5'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 4'th argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
		// 3'rd argument is pass
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			strpass = info[argpos++].As<Napi::String>().Utf8Value();
		}

		if(argpos < info.Length() && info[argpos].IsNumber()){
			// 4'th argument is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 5'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 5'th argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsNumber()){
		// 3'rd argument is expire
		int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
		expire			= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// 4'th argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// 3'rd argument is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		QueuePushWorker*	worker = new QueuePushWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strprefix, strkey, strval, is_fifo, attrchk, strpass, expire);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComQPush*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComQPush(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComQPush(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		dkcres_type_t	rescode = DKC_NORESTYPE;
		bool			result	= false;
		if(!strkey.empty()){
			// key queue
			result = pComObj->KeyQueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(strval.c_str()), strval.length() + 1, is_fifo, NULL, 0UL, attrchk, (strpass.empty() ? NULL : strpass.c_str()), (expire > 0 ? &expire : NULL), &rescode);
		}else{
			// queue
			result = pComObj->QueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, reinterpret_cast<const unsigned char*>(strval.c_str()), strval.length() + 1, is_fifo, NULL, 0UL, attrchk, (strpass.empty() ? NULL : strpass.c_str()), (expire > 0 ? &expire : NULL), &rescode);
		}
		DKC_DELETE(pComObj);
		return Napi::Boolean::New(env, result);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool QueuePop(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	prefix\
 * 	, bool		is_fifo=true\
 *  , bool		is_key_queue=false\
 * 	, String	pass=""\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Push key and value(or only value) to queue for onetime connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] prefix			Specify prefix name for queue
 * @param[in] is_fifo			Specify optional queue mode fifo or lifo.
 * @param[in] is_key_queue		Specify optional queue type queue or keyqueue.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] cbfunc			callback function.
 *
 * @return	Returns value when queue, returns array(key and value) when key queue.
 *			If there is no queued data or something error occurred, returns null.
 *			Returns always true when the callback function is specified.
 *
 * @fn bool QueuePop(\
 *  String		prefix\
 * 	, bool		is_fifo=true\
 *  , bool		is_key_queue=false\
 * 	, String	pass=""\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Push key and value(or only value) to queue for permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] prefix			Specify prefix name for queue
 * @param[in] is_fifo			Specify optional queue mode fifo or lifo.
 * @param[in] is_key_queue		Specify optional queue type queue or keyqueue.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] cbfunc			callback function.
 *
 * @return	Returns value when queue, returns array(key and value) when key queue.
 *			If there is no queued data or something error occurred, returns null.
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::QueuePop(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_QUEUEPOP]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strprefix;
	bool		is_fifo		= true;
	bool		is_key_queue= false;
	std::string	strpass;

	// target prefix
	if(info.Length() <= argpos){
		Napi::TypeError::New(env, "There is no prefix parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The prefix name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strprefix = info[argpos++].As<Napi::String>().Utf8Value();

	// other arguments for target
	if(argpos < info.Length() && info[argpos].IsBoolean()){
		// 2'nd argument is fifo/lifo mode
		is_fifo = info[argpos++].As<Napi::Boolean>().Value();

		if(argpos < info.Length() && info[argpos].IsBoolean()){
			// 3'rd argument is queue/keyqueue type
			is_key_queue = info[argpos++].As<Napi::Boolean>().Value();

			if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
				// 4'th argument is pass
				if(info[argpos].IsNull()){
					argpos++;
				}else{
					strpass = info[argpos++].As<Napi::String>().Utf8Value();
				}

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// 5'th argument is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// 3'rd argument is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 3'rd argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
		// 2'nd argument is pass
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			strpass = info[argpos++].As<Napi::String>().Utf8Value();
		}

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// 3'rd argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// 2'nd argument is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		QueuePopWorker*	worker = new QueuePopWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strprefix, is_fifo, is_key_queue, strpass);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComQPop*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComQPop(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComQPop(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		dkcres_type_t	rescode = DKC_NORESTYPE;
		bool			result	= false;
		if(is_key_queue){
			// key queue
			const unsigned char*	pkeytmp		= NULL;
			size_t					keytmplen	= 0L;
			const unsigned char*	pvaltmp		= NULL;
			size_t					valtmplen	= 0L;
			result = pComObj->KeyQueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, is_fifo, true, (strpass.empty() ? NULL : strpass.c_str()), &pkeytmp, &keytmplen, &pvaltmp, &valtmplen, &rescode);

			if(result && (pkeytmp && 0 < keytmplen)){
				Napi::Array	retarr = Napi::Array::New(env, 2);
				std::string	strtmpkey(reinterpret_cast<const char*>(pkeytmp), ('\0' == pkeytmp[keytmplen - 1] ? keytmplen - 1 : keytmplen));

				if(pvaltmp && 0 < valtmplen){
					std::string	strtmpval(reinterpret_cast<const char*>(pvaltmp), ('\0' == pvaltmp[valtmplen - 1] ? valtmplen - 1 : valtmplen));
					retarr.Set((uint32_t)0, Napi::String::New(env, strtmpkey));
					retarr.Set((uint32_t)1, Napi::String::New(env, strtmpval));
				}else{
					retarr.Set((uint32_t)0, Napi::String::New(env, strtmpkey));
					retarr.Set((uint32_t)1, env.Null());
				}
				DKC_DELETE(pComObj);
				return retarr;
			}else{
				DKC_DELETE(pComObj);
				return env.Null();
			}
		}else{
			// queue
			const unsigned char*	pvaltmp		= NULL;
			size_t					valtmplen	= 0L;
			result = pComObj->QueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, is_fifo, true, (strpass.empty() ? NULL : strpass.c_str()), &pvaltmp, &valtmplen, &rescode);

			if(result && (pvaltmp && 0 < valtmplen)){
				std::string	strresult(reinterpret_cast<const char*>(pvaltmp), ('\0' == pvaltmp[valtmplen - 1] ? valtmplen - 1 : valtmplen));
				DKC_DELETE(pComObj);
				return Napi::String::New(env, strresult);
			}else{
				DKC_DELETE(pComObj);
				return env.Null();
			}
		}
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool QueueRemove(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	prefix\
 *  , int		count=0\
 * 	, bool		is_fifo=true\
 *  , bool		is_key_queue=false\
 * 	, String	pass=""\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Remove key and value(or only value) from queue for onetime connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] prefix			Specify prefix name for queue
 * @param[in] count				Specify optional remove queue count
 * @param[in] is_fifo			Specify optional queue mode fifo or lifo.
 * @param[in] is_key_queue		Specify optional queue type queue or keyqueue.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 * @fn bool QueueRemove(\
 *  String		prefix\
 *  , int		count=0\
 * 	, bool		is_fifo=true\
 *  , bool		is_key_queue=false\
 * 	, String	pass=""\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Remove key and value(or only value) from queue for permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] prefix			Specify prefix name for queue
 * @param[in] count				Specify optional remove queue count
 * @param[in] is_fifo			Specify optional queue mode fifo or lifo.
 * @param[in] is_key_queue		Specify optional queue type queue or keyqueue.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::QueueRemove(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_QUEUEREMOVE]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strprefix;
	int			count			= 0;
	bool		is_fifo			= true;
	bool		is_key_queue	= false;
	std::string	strpass;

	// target prefix
	if(info.Length() <= argpos){
		Napi::TypeError::New(env, "There is no prefix parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The prefix name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strprefix = info[argpos++].As<Napi::String>().Utf8Value();

	// other arguments for target
	if(argpos < info.Length() && info[argpos].IsNumber()){
		// 2'nd argument is count
		count = info[argpos++].As<Napi::Number>().Int32Value();

		if(argpos < info.Length() && info[argpos].IsBoolean()){
			// 3'rd argument is fifo/lifo mode
			is_fifo = info[argpos++].As<Napi::Boolean>().Value();

			if(argpos < info.Length() && info[argpos].IsBoolean()){
				// 4'th argument is queue/keyqueue type
				is_key_queue = info[argpos++].As<Napi::Boolean>().Value();

				if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
					// 5'th argument is pass
					if(info[argpos].IsNull()){
						argpos++;
					}else{
						strpass = info[argpos++].As<Napi::String>().Utf8Value();
					}

					if(argpos < info.Length() && info[argpos].IsFunction()){
						// 6'th argument is callback
						maybeCallback	= info[argpos++].As<Napi::Function>();
						hasCallback		= true;
					}
				}else if(argpos < info.Length() && info[argpos].IsFunction()){
					// 5'th argument is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
				// 4'th argument is pass
				if(info[argpos].IsNull()){
					argpos++;
				}else{
					strpass = info[argpos++].As<Napi::String>().Utf8Value();
				}

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// 5'th argument is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// 3'rd argument is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 3'rd argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsBoolean()){
		// 2'nd argument is fifo/lifo mode
		is_fifo = info[argpos++].As<Napi::Boolean>().Value();

		if(argpos < info.Length() && info[argpos].IsBoolean()){
			// 3'rd argument is queue/keyqueue type
			is_key_queue = info[argpos++].As<Napi::Boolean>().Value();

			if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
				// 4'th argument is pass
				if(info[argpos].IsNull()){
					argpos++;
				}else{
					strpass = info[argpos++].As<Napi::String>().Utf8Value();
				}

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// 5'th argument is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// 3'rd argument is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 3'rd argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
		// 2'nd argument is pass
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			strpass = info[argpos++].As<Napi::String>().Utf8Value();
		}

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// 3'rd argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// 2'nd argument is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		QueueRemoveWorker*	worker = new QueueRemoveWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strprefix, count, is_fifo, is_key_queue, strpass);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComQDel*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComQDel(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComQDel(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		dkcres_type_t	rescode = DKC_NORESTYPE;
		bool			result	= false;
		if(is_key_queue){
			// key queue
			result = pComObj->KeyQueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, count, is_fifo, true, (strpass.empty() ? NULL : strpass.c_str()), &rescode);
		}else{
			// queue
			result = pComObj->QueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, count, is_fifo, true, (strpass.empty() ? NULL : strpass.c_str()), &rescode);
		}
		DKC_DELETE(pComObj);
		return Napi::Boolean::New(env, result);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool CasInit(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 *  , uint		value=0\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Initialize key and value for CAS operation on onetime connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name
 * @param[in] value				Specify optional the value.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 * @fn bool CasInit(\
 *  String		key\
 *  , uint		value=0\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Initialize key and value for CAS operation on  permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key				Specify the key name
 * @param[in] value				Specify optional the value.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::CasInit(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_CASINIT]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strkey;
	uint32_t	value	= 0;
	std::string	strpass;
	time_t		expire	= 0;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Napi::TypeError::New(env, "There is no key name parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strkey = info[argpos++].As<Napi::String>().Utf8Value();

	// other arguments for target
	if(argpos < info.Length() && info[argpos].IsNumber()){
		// 2'nd argument is value
		value = info[argpos++].As<Napi::Number>().Int32Value();

		if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// 3'rd argument is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsNumber()){
				// 4'th argument is expire
				int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// 5'th argument is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}

		}else if(argpos < info.Length() && info[argpos].IsNumber()){
			// 3'rd argument is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire				= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 3'rd argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
		// 2'nd argument is pass
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			strpass = info[argpos++].As<Napi::String>().Utf8Value();
		}

		if(argpos < info.Length() && info[argpos].IsNumber()){
			// 3'rd argument is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 3'rd argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}

	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// 2'nd argument is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		CasInitWorker*	worker = new CasInitWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey, value, strpass, expire);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComCasInit*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComCasInit(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComCasInit(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		dkcres_type_t	rescode = DKC_NORESTYPE;
		bool			result	= pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(&value), sizeof(uint32_t), (strpass.empty() ? NULL : strpass.c_str()), (expire > 0 ? &expire : NULL), &rescode);

		DKC_DELETE(pComObj);
		return Napi::Boolean::New(env, result);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool CasGet(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 * 	, String	pass=""\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Get CAS value by key on onetime connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] cbfunc			callback function.
 *
 * @return	Returns the value(uint32). If there is no key(subkey), returns undefined.
 *			Returns always true when the callback function is specified.
 *
 * @fn bool CasGet(\
 *  String		key\
 * 	, String	pass=""\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	Get CAS value by key on  permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key				Specify the key name
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] cbfunc			callback function.
 *
 * @return	Returns the value(uint32). If there is no key(subkey), returns undefined.
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::CasGet(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_CASGET]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strkey;
	std::string	strpass;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Napi::TypeError::New(env, "There is no key name parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strkey = info[argpos++].As<Napi::String>().Utf8Value();

	// other arguments for target
	if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
		// 3'rd argument is pass
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			strpass = info[argpos++].As<Napi::String>().Utf8Value();
		}

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// 4'th argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// 3'rd argument is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		CasGetWorker*	worker = new CasGetWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey, strpass);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComCasGet*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComCasGet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComCasGet(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		dkcres_type_t			rescode		= DKC_NORESTYPE;
		const unsigned char*	pvaltmp		= NULL;
		size_t					valtmplen	= 0L;
		if(pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, true, (strpass.empty() ? NULL : strpass.c_str()), &pvaltmp, &valtmplen, &rescode) && (valtmplen == sizeof(uint32_t))){
			uint32_t	resultval = 0;
			std::memcpy(reinterpret_cast<unsigned char*>(&resultval), pvaltmp, std::min<size_t>(valtmplen, sizeof(resultval)));

			DKC_DELETE(pComObj);
			return Napi::Number::New(env, resultval);
		}else{
			DKC_DELETE(pComObj);
			return env.Undefined();
		}
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool CasSet(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 *  , uint		oldval=0\
 *  , uint		newval=0\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	CAS operation to key on onetime connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name
 * @param[in] oldval			Specify old value.
 * @param[in] newval			Specify new value.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 * @fn bool CasSet(\
 *  String		key\
 *  , uint		oldval=0\
 *  , uint		newval=0\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	CAS operation to key on  permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key				Specify the key name
 * @param[in] oldval			Specify old value.
 * @param[in] newval			Specify new value.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::CasSet(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_CASSET]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strkey;
	uint32_t	oldval	= 0;
	uint32_t	newval	= 0;
	std::string	strpass;
	time_t		expire	= 0;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= (argpos + 2)){
		Napi::TypeError::New(env, "There is no key name parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strkey = info[argpos++].As<Napi::String>().Utf8Value();

	// old value argument
	if(!info[argpos].IsNumber()){
		Napi::TypeError::New(env, "The old value must be uint.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	oldval = info[argpos++].As<Napi::Number>().Uint32Value();

	// new value argument
	if(!info[argpos].IsNumber()){
		Napi::TypeError::New(env, "The new value must be uint.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	newval = info[argpos++].As<Napi::Number>().Uint32Value();

	// other arguments for target
	if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
		// 4'th argument is pass
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			strpass = info[argpos++].As<Napi::String>().Utf8Value();
		}

		if(argpos < info.Length() && info[argpos].IsNumber()){
			// 5'th argument is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 6'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}

		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 5'th argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsNumber()){
		// 4'th argument is expire
		int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
		expire			= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// 5'th argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// 4'th argument is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		CasSetWorker*	worker = new CasSetWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey, oldval, newval, strpass, expire);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComCasSet*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComCasSet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComCasSet(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		dkcres_type_t	rescode = DKC_NORESTYPE;
		bool			result	= pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(&oldval), sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&newval), sizeof(uint32_t), true, (strpass.empty() ? NULL : strpass.c_str()), (expire > 0 ? &expire : NULL), &rescode);

		DKC_DELETE(pComObj);
		return Napi::Boolean::New(env, result);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool CasIncDec(\
 *  String		conf\
 *  , int		port=CHM_INVALID_PORT\
 *  , bool		auto_rejoin=false\
 *  , bool		no_giveup_rejoin=false\
 *  , String	key\
 *  , bool		is_increment=true\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	CAS increment/decrement operation to key on onetime connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] conf				specify configuration for onetime connection
 * @param[in] port				specify control port number for chmpx for onetime connection(optional)
 * @param[in] cuk				specify cuk for chmpx for onetime connection(optional)
 * @param[in] auto_rejoin		specify automatic rejoin flag value for onetime connection(optional)
 * @param[in] no_giveup_rejoin	specify never giveup for rejoin flag value for onetime connection(optional)
 * @param[in] key				Specify the key name
 * @param[in] is_increment		Specify optional increment/decrement mode flag.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 * @fn bool CasIncDec(\
 *  String		key\
 *  , bool		is_increment=true\
 * 	, String	pass=""\
 * 	, int		expire=0\
 *  , Callback	cbfunc=null\
 * )
 *
 * @brief	CAS increment/decrement operation to key on  permanent connection.
 *			If the callback function is specified, or on callback handles for this,
 * 			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key				Specify the key name
 * @param[in] is_increment		Specify optional increment/decrement mode flag.
 * @param[in] pass				Specify optional passphrase if encrypting the key
 * @param[in] expire			Specify optional timeout seconds if you need
 * @param[in] cbfunc			callback function.
 *
 * @return	Return true for success, false for failure
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hdkcNode::CasIncDec(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hdkcNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hdkcNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hdkcNode*		obj		= Napi::ObjectWrap<K2hdkcNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_emitters[EMITTER_POS_CASINCDEC]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// For onetime connection
	size_t		argpos			= 0;
	std::string	conf;
	int16_t		ctlport			= static_cast<int16_t>(CHM_INVALID_PORT);
	std::string	cuk;
	bool		auto_rejoin		= false;
	bool		no_giveup_rejoin= false;
	if(!K2hdkcNode::ParseArgumentsForOnetime(info, obj, argpos, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin)){
		return env.Undefined();
	}

	// target arguments
	std::string	strkey;
	bool		is_increment= true;
	std::string	strpass;
	time_t		expire		= 0;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Napi::TypeError::New(env, "There is no key name parameter.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[argpos].IsString()){
		Napi::TypeError::New(env, "The key name must be string.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	strkey = info[argpos++].As<Napi::String>().Utf8Value();

	// other arguments for target
	if(argpos < info.Length() && info[argpos].IsBoolean()){
		// 2'nd argument is increment/decrement mode
		is_increment = info[argpos++].As<Napi::Boolean>().Value();

		if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
			// 3'rd argument is pass
			if(info[argpos].IsNull()){
				argpos++;
			}else{
				strpass = info[argpos++].As<Napi::String>().Utf8Value();
			}

			if(argpos < info.Length() && info[argpos].IsNumber()){
				// 4'th argument is expire
				int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos].IsFunction()){
					// 5'th argument is callback
					maybeCallback	= info[argpos++].As<Napi::Function>();
					hasCallback		= true;
				}
			}else if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsNumber()){
			// 3'rd argument is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 3'rd argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && (info[argpos].IsString() || info[argpos].IsNull())){
		// 2'nd argument is pass
		if(info[argpos].IsNull()){
			argpos++;
		}else{
			strpass = info[argpos++].As<Napi::String>().Utf8Value();
		}

		if(argpos < info.Length() && info[argpos].IsNumber()){
			// 3'rd argument is expire
			int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos].IsFunction()){
				// 4'th argument is callback
				maybeCallback	= info[argpos++].As<Napi::Function>();
				hasCallback		= true;
			}
		}else if(argpos < info.Length() && info[argpos].IsFunction()){
			// 3'rd argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsNumber()){
		// 2'nd argument is expire
		int32_t	nexpire = static_cast<int32_t>(info[argpos++].As<Napi::Number>().Int32Value());
		expire			= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos].IsFunction()){
			// 3'rd argument is callback
			maybeCallback	= info[argpos++].As<Napi::Function>();
			hasCallback		= true;
		}
	}else if(argpos < info.Length() && info[argpos].IsFunction()){
		// 2'nd argument is callback
		maybeCallback	= info[argpos++].As<Napi::Function>();
		hasCallback		= true;
	}

	// check over arguments
	if(argpos < info.Length()){
		Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// work
	if(hasCallback){
		CasIncDecWorker*	worker = new CasIncDecWorker(maybeCallback, obj->_k2hdkcslave, conf, ctlport, cuk, auto_rejoin, no_giveup_rejoin, strkey, is_increment, strpass, expire);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2hdkcComCasIncDec*	pComObj;
		if(!obj->IsInitialize()){
			pComObj = GetOtSlaveK2hdkcComCasIncDec(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComCasIncDec(&(obj->_k2hdkcslave));
		}
		if(!pComObj){
			Napi::TypeError::New(env, "Internal error: Could not create command object.").ThrowAsJavaScriptException();
			return env.Undefined();
		}

		dkcres_type_t	rescode = DKC_NORESTYPE;
		bool			result	= pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, is_increment, true, (strpass.empty() ? NULL : strpass.c_str()), (expire > 0 ? &expire : NULL), &rescode);

		DKC_DELETE(pComObj);
		return Napi::Boolean::New(env, result);
	}
}

/**
 * @memberof K2hdkcNode
 *
 * @fn bool PrintVersion(\
 *  int		fd=1\
 * )
 *
 * @brief	Print K2HDKC library version
 *			If fd is not specified, the status is put to stdout.
 *
 * @param[in] fd	Specify output stream
 *
 * @return return true for success, false for failure
 *
 */

Napi::Value K2hdkcNode::PrintVersion(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	k2hdkc_print_version(fp);

	// [NOTE]
	// Must flush at here, because nodejs's file descriptor is used for fd.
	// Otherwise, calling flash on nodejs(javascript) is not effected.
	fflush(fp);

	return Napi::Boolean::New(env, true);
}

//@}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
