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

using namespace v8;
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
		return NULL;
	}
	for(const char** ptmp = &stc_emitters[0]; ptmp && *ptmp; ++ptmp){
		if(0 == strcasecmp(*ptmp, emitter)){
			return *ptmp;
		}
	}
	return NULL;
}

//---------------------------------------------------------
// Utility Macros
//---------------------------------------------------------
//
// Parser macros for onetime connection
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
#define	ParseArgumentsForOnetime(argpos) \
		string	conf; \
		int16_t	ctlport			= CHM_INVALID_PORT; \
		string	cuk; \
		bool	auto_rejoin		= false; \
		bool	no_giveup_rejoin= false; \
		{ \
			if(!obj->_k2hdkcslave){ \
				if(info.Length() < 2){ \
					Nan::ThrowSyntaxError("There is no enough parameters for onetime connection."); \
					return; \
				} \
				if(!info[argpos]->IsString()){ \
					Nan::ThrowSyntaxError("First parameter is not string"); \
					return; \
				} \
				Nan::Utf8String	buf(info[argpos++]); \
				conf							= std::string(*buf); \
				if(argpos < info.Length()){ \
					if(info[argpos]->IsNumber()){ \
						ctlport				= static_cast<int16_t>(Nan::To<int32_t>(info[argpos++]).FromJust()); \
						if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){ \
							Nan::Utf8String	buf(info[argpos++]); \
							cuk						= std::string(*buf); \
							if(argpos < info.Length() && info[argpos]->IsBoolean()){ \
								auto_rejoin		= Nan::To<bool>(info[argpos++]).FromJust(); \
								if(argpos < info.Length() && info[argpos]->IsBoolean()){ \
									no_giveup_rejoin= Nan::To<bool>(info[argpos++]).FromJust(); \
								} \
							} \
						}else if(argpos < info.Length() && info[argpos]->IsBoolean()){ \
							auto_rejoin			= Nan::To<bool>(info[argpos++]).FromJust(); \
							if(argpos < info.Length() && info[argpos]->IsBoolean()){ \
								no_giveup_rejoin= Nan::To<bool>(info[argpos++]).FromJust(); \
							} \
						} \
					}else if(info[argpos]->IsString() || info[argpos]->IsNull()){ \
						Nan::Utf8String	buf(info[argpos++]); \
						cuk						= std::string(*buf); \
						if(argpos < info.Length() && info[argpos]->IsBoolean()){ \
							auto_rejoin		= Nan::To<bool>(info[argpos++]).FromJust(); \
							if(argpos < info.Length() && info[argpos]->IsBoolean()){ \
								no_giveup_rejoin= Nan::To<bool>(info[argpos++]).FromJust(); \
							} \
						} \
					}else if(argpos < info.Length() && info[argpos]->IsBoolean()){ \
						auto_rejoin			= Nan::To<bool>(info[argpos++]).FromJust(); \
						if(argpos < info.Length() && info[argpos]->IsBoolean()){ \
							no_giveup_rejoin= Nan::To<bool>(info[argpos++]).FromJust(); \
						} \
					} \
				} \
			} \
		}

//
// Set callback emitter
//
#define	SetK2hdkcNodeCallback(info, pos, pemitter) \
		{ \
			K2hdkcNode*	obj = Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This()); \
			if(info.Length() <= pos){ \
				Nan::ThrowSyntaxError("No callback is specified."); \
				return; \
			} \
			Nan::Callback* cb = new Nan::Callback(); \
			cb->SetFunction(info[pos].As<v8::Function>()); \
			bool	result = obj->_cbs.Set(pemitter, cb); \
			info.GetReturnValue().Set(Nan::New(result)); \
		}

//
// Unset callback emitter
//
#define	UnsetK2hdkcNodeCallback(info, pemitter) \
		{ \
			K2hdkcNode*	obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This()); \
			bool		result	= obj->_cbs.Unset(pemitter); \
			info.GetReturnValue().Set(Nan::New(result)); \
		}

//---------------------------------------------------------
// K2hdkcNode Class
//---------------------------------------------------------
Nan::Persistent<Function>	K2hdkcNode::constructor;

//---------------------------------------------------------
// K2hdkcNode Methods
//---------------------------------------------------------
K2hdkcNode::K2hdkcNode() : _k2hdkcslave(NULL), _cbs()
{
}

K2hdkcNode::~K2hdkcNode()
{
	Clean();
}

void K2hdkcNode::Clean()
{
	if(_k2hdkcslave){
		_k2hdkcslave->Clean(true);
	}
	K2H_Delete(_k2hdkcslave);
}

void K2hdkcNode::Init()
{
	// Prepare constructor template
	Local<FunctionTemplate>	tpl = Nan::New<FunctionTemplate>(New); 
	tpl->SetClassName(Nan::New("K2hdkcNode").ToLocalChecked()); 
	tpl->InstanceTemplate()->SetInternalFieldCount(1); 

	Nan::SetPrototypeMethod(tpl, "on",				On);
	Nan::SetPrototypeMethod(tpl, "onInit",			OnInit);
	Nan::SetPrototypeMethod(tpl, "onGet",			OnGet);
	Nan::SetPrototypeMethod(tpl, "onGetSubkeys",	OnGetSubkeys);
	Nan::SetPrototypeMethod(tpl, "onGetAttrs",		OnGetAttrs);
	Nan::SetPrototypeMethod(tpl, "onSet",			OnSet);
	Nan::SetPrototypeMethod(tpl, "onSetSubkeys",	OnSetSubkeys);
	Nan::SetPrototypeMethod(tpl, "onSetAll",		OnSetAll);
	Nan::SetPrototypeMethod(tpl, "onRemove",		OnRemove);
	Nan::SetPrototypeMethod(tpl, "onRename",		OnRename);
	Nan::SetPrototypeMethod(tpl, "onQueuePush",		OnQueuePush);
	Nan::SetPrototypeMethod(tpl, "onQueuePop",		OnQueuePop);
	Nan::SetPrototypeMethod(tpl, "onQueueRemove",	OnQueueRemove);
	Nan::SetPrototypeMethod(tpl, "onQueueDel",		OnQueueRemove);
	Nan::SetPrototypeMethod(tpl, "onCasInit",		OnCasInit);
	Nan::SetPrototypeMethod(tpl, "onCasGet",		OnCasGet);
	Nan::SetPrototypeMethod(tpl, "onCasSet",		OnCasSet);
	Nan::SetPrototypeMethod(tpl, "onCasIncDec",		OnCasIncDec);
	Nan::SetPrototypeMethod(tpl, "off",				Off);
	Nan::SetPrototypeMethod(tpl, "offInit",			OffInit);
	Nan::SetPrototypeMethod(tpl, "offGet",			OffGet);
	Nan::SetPrototypeMethod(tpl, "offGetSubkeys",	OffGetSubkeys);
	Nan::SetPrototypeMethod(tpl, "offGetAttrs",		OffGetAttrs);
	Nan::SetPrototypeMethod(tpl, "offSet",			OffSet);
	Nan::SetPrototypeMethod(tpl, "offSetSubkeys",	OffSetSubkeys);
	Nan::SetPrototypeMethod(tpl, "offSetAll",		OffSetAll);
	Nan::SetPrototypeMethod(tpl, "offRemove",		OffRemove);
	Nan::SetPrototypeMethod(tpl, "offRename",		OffRename);
	Nan::SetPrototypeMethod(tpl, "offQueuePush",	OffQueuePush);
	Nan::SetPrototypeMethod(tpl, "offQueuePop",		OffQueuePop);
	Nan::SetPrototypeMethod(tpl, "offQueueRemove",	OffQueueRemove);
	Nan::SetPrototypeMethod(tpl, "offQueueDel",		OffQueueRemove);
	Nan::SetPrototypeMethod(tpl, "offCasInit",		OffCasInit);
	Nan::SetPrototypeMethod(tpl, "offCasGet",		OffCasGet);
	Nan::SetPrototypeMethod(tpl, "offCasSet",		OffCasSet);
	Nan::SetPrototypeMethod(tpl, "offCasIncDec",	OffCasIncDec);

	Nan::SetPrototypeMethod(tpl, "init",			Init);
	Nan::SetPrototypeMethod(tpl, "clean",			Clean);
	Nan::SetPrototypeMethod(tpl, "clear",			Clean);
	Nan::SetPrototypeMethod(tpl, "isPermanent",		IsPermanent);
	Nan::SetPrototypeMethod(tpl, "getValue",		GetValue);
	Nan::SetPrototypeMethod(tpl, "getSubkeys",		GetSubkeys);
	Nan::SetPrototypeMethod(tpl, "getAttrs",		GetAttrs);
	Nan::SetPrototypeMethod(tpl, "setValue",		SetValue);
	Nan::SetPrototypeMethod(tpl, "setSubkeys",		SetSubkeys);
	Nan::SetPrototypeMethod(tpl, "addSubkey",		SetValue);
	Nan::SetPrototypeMethod(tpl, "clearSubkeys",	SetSubkeys);
	Nan::SetPrototypeMethod(tpl, "setAll",			SetAll);
	Nan::SetPrototypeMethod(tpl, "remove",			Remove);
	Nan::SetPrototypeMethod(tpl, "rename",			Rename);
	Nan::SetPrototypeMethod(tpl, "queuePush",		QueuePush);
	Nan::SetPrototypeMethod(tpl, "queuePop",		QueuePop);
	Nan::SetPrototypeMethod(tpl, "queueRemove",		QueueRemove);
	Nan::SetPrototypeMethod(tpl, "queueDel",		QueueRemove);
	Nan::SetPrototypeMethod(tpl, "casInit",			CasInit);
	Nan::SetPrototypeMethod(tpl, "casGet",			CasGet);
	Nan::SetPrototypeMethod(tpl, "casSet",			CasSet);
	Nan::SetPrototypeMethod(tpl, "casIncDec",		CasIncDec);
	Nan::SetPrototypeMethod(tpl, "printVersion",	PrintVersion);

	// Reset
	constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(K2hdkcNode::New)
{
	if(info.IsConstructCall()){
		// Invoked as constructor: new K2hdkcNode()
		K2hdkcNode*	obj = new K2hdkcNode();
		obj->Wrap(info.This()); 

		// called with arguments
		//
		// [NOTE] this logic as same as Init() except callback function parameter
		//
		if(0 < info.Length()){
			// create permanent connection at initializing
			int				argpos			= 0;
			int16_t			ctlport			= CHM_INVALID_PORT;

			if(!info[argpos]->IsString()){
				Nan::ThrowSyntaxError("First parameter is not configuration");
			}else{
				// 1'st argument is conf
				Nan::Utf8String	buf(info[argpos++]);
				std::string		conf			= std::string(*buf);
				std::string		cuk("");
				bool			auto_rejoin		= false;
				bool			no_giveup_rejoin= false;
				if(argpos < info.Length() && info[argpos]->IsNumber()){
					// 2'nd argument is port
					ctlport					= static_cast<int16_t>(Nan::To<int32_t>(info[argpos++]).FromJust());
					if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
						// 3'rd argument is cuk
						Nan::Utf8String	buf2(info[argpos++]);
						cuk				= std::string(*buf2);
						if(argpos < info.Length() && info[argpos]->IsBoolean()){
							// 4'th argument is auto rejoin
							auto_rejoin			= Nan::To<bool>(info[argpos++]).FromJust();
							if(argpos < info.Length() && info[argpos]->IsBoolean()){
								// 5'th argument is no giveup rejoin
								no_giveup_rejoin= Nan::To<bool>(info[argpos++]).FromJust();
							}
						}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
							// 4'th argument is no giveup rejoin
							no_giveup_rejoin	= Nan::To<bool>(info[argpos++]).FromJust();
						}
					}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
						// 3'rd argument is auto rejoin
						auto_rejoin			= Nan::To<bool>(info[argpos++]).FromJust();
						if(argpos < info.Length() && info[argpos]->IsBoolean()){
							// 4'th argument is no giveup rejoin
							no_giveup_rejoin= Nan::To<bool>(info[argpos++]).FromJust();
						}
					}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
						// 3'rd argument is no giveup rejoin
						no_giveup_rejoin	= Nan::To<bool>(info[argpos++]).FromJust();
					}

				}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
					// 2'nd argument is cuk
					Nan::Utf8String	buf3(info[argpos++]);
					cuk				= std::string(*buf3);
					if(argpos < info.Length() && info[argpos]->IsBoolean()){
						// 3'rd argument is auto rejoin
						auto_rejoin			= Nan::To<bool>(info[argpos++]).FromJust();
						if(argpos < info.Length() && info[argpos]->IsBoolean()){
							// 4'th argument is no giveup rejoin
							no_giveup_rejoin= Nan::To<bool>(info[argpos++]).FromJust();
						}
					}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
						// 3'rd argument is no giveup rejoin
						no_giveup_rejoin	= Nan::To<bool>(info[argpos++]).FromJust();
					}

				}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
					// 2'nd argument is auto rejoin
					auto_rejoin				= Nan::To<bool>(info[argpos++]).FromJust();
					if(argpos < info.Length() && info[argpos]->IsBoolean()){
						// 3'rd argument is no giveup rejoin
						no_giveup_rejoin	= Nan::To<bool>(info[argpos++]).FromJust();
					}
				}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
					// 2'nd argument is no giveup rejoin
					no_giveup_rejoin		= Nan::To<bool>(info[argpos++]).FromJust();
				}

				// check over arguments
				if(argpos < info.Length()){
					Nan::ThrowSyntaxError("Too many parameter.");
				}else{
					// build permanent connection object
					obj->_k2hdkcslave = new K2hdkcSlave();
					if(!obj->_k2hdkcslave->Initialize(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin)){
						obj->Clean();
						Nan::ThrowError("Could not initialize k2hdkc slave object");
					}else{
						if(!obj->_k2hdkcslave->Open(no_giveup_rejoin)){
							obj->Clean();
							Nan::ThrowError("Could not open msgid by k2hdkc slave object");
						}
					}
				}
			}
		}
		info.GetReturnValue().Set(info.This());

	}else{ 
		// Invoked as plain function K2hdkcNode(), turn into construct call.
		Local<Function>	cons		= Nan::New<Function>(constructor);
		const unsigned	argc		= info.Length();
		const unsigned	arraysize	= (0 < argc ? argc : 1);
		Local<Value>	argv[arraysize];
		if(0 < argc){
			for(unsigned cnt = 0; cnt < argc; ++cnt){
				argv[cnt]			= info[cnt];
			}
		}else{
			argv[0]					= Nan::Null();		// dummy
		}
		info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked()); 
	}
}

NAN_METHOD(K2hdkcNode::NewInstance)
{
	Local<Function>	cons		= Nan::New<Function>(constructor);
	const unsigned	argc		= info.Length();
	const unsigned	arraysize	= (0 < argc ? argc : 1);
	Local<Value>	argv[arraysize];
	if(0 < argc){
		for(unsigned cnt = 0; cnt < argc; ++cnt){
			argv[cnt]			= info[cnt];
		}
	}else{
		argv[0]					= Nan::Null();		// dummy
	}
	info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked()); 
}

Local<Object> K2hdkcNode::GetInstance(Nan::NAN_METHOD_ARGS_TYPE info)
{
	Nan::EscapableHandleScope	scope;

	Local<Function>	cons		= Nan::New<Function>(constructor);
	const unsigned	argc		= info.Length();
	const unsigned	arraysize	= (0 < argc ? argc : 1);
	Local<Value>	argv[arraysize];
	if(0 < argc){
		for(unsigned cnt = 0; cnt < argc; ++cnt){
			argv[cnt]			= info[cnt];
		}
	}else{
		argv[0]					= Nan::Null();		// dummy
	}
	Local<Object> 	instance	= Nan::NewInstance(cons, argc, argv).ToLocalChecked();

	return scope.Escape(instance);
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

NAN_METHOD(K2hdkcNode::On)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No handle emitter name is specified.");
		return;
	}else if(info.Length() < 2){
		Nan::ThrowSyntaxError("No callback is specified.");
		return;
	}

	// check emitter name
	Nan::Utf8String	emitter(info[0]);
	const char*		pemitter;
	if(NULL == (pemitter = GetNormalizationEmitter(*emitter))){
		string	msg = "Unknown ";
		msg			+= *emitter;
		msg			+= " emitter";
		Nan::ThrowSyntaxError(msg.c_str());
		return;
	}
	// add callback
	SetK2hdkcNodeCallback(info, 1, pemitter);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnInit(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnInit)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_INIT]);
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

NAN_METHOD(K2hdkcNode::OnGet)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_GET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnGetSubkeys(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnGetSubkeys)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_GETSUBKEYS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnGetAttrs(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnGetAttrs)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_GETATTRS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnSet(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnSet)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_SET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnSetSubkeys(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnSetSubkeys)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_SETSUBKEYS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnSetAll(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnSetAll)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_SETALL]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnRemove(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnRemove)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_REMOVE]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnRename(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnRename)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_RENAME]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnQueuePush(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnQueuePush)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_QUEUEPUSH]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnQueuePop(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnQueuePop)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_QUEUEPOP]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnQueueRemove(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnQueueRemove)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_QUEUEREMOVE]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnCasInit(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnCasInit)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_CASINIT]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnCasGet(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnCasGet)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_CASGET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnCasSet(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnCasSet)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_CASSET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OnCasIncDec(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OnCasIncDec)
{
	SetK2hdkcNodeCallback(info, 0, stc_emitters[EMITTER_POS_CASINCDEC]);
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

NAN_METHOD(K2hdkcNode::Off)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No handle emitter name is specified.");
		return;
	}

	// check emitter name
	Nan::Utf8String	emitter(info[0]);
	const char*		pemitter;
	if(NULL == (pemitter = GetNormalizationEmitter(*emitter))){
		string	msg = "Unknown ";
		msg			+= *emitter;
		msg			+= " emitter";
		Nan::ThrowSyntaxError(msg.c_str());
		return;
	}
	// unset callback
	UnsetK2hdkcNodeCallback(info, pemitter);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffInit(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffInit)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_INIT]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffGet(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffGet)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_GET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffGetSubkeys(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffGetSubkeys)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_GETSUBKEYS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffGetAttrs(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffGetAttrs)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_GETATTRS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffSet(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffSet)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_SET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffSetSubkeys(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffSetSubkeys)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_SETSUBKEYS]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffSetAll(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffSetAll)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_SETALL]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffRemove(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffRemove)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_REMOVE]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffRename(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffRename)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_RENAME]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffQueuePush(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffQueuePush)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_QUEUEPUSH]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffQueuePop(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffQueuePop)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_QUEUEPOP]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffQueueRemove(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffQueueRemove)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_QUEUEREMOVE]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffCasInit(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffCasInit)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_CASINIT]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffCasGet(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffCasGet)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_CASGET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffCasSet(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffCasSet)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_CASSET]);
}

/**
 * @memberof K2hdkcNode
 * @fn void OffCasIncDec(void)
 *
 * @brief	set callback handling for initializing chmpx object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hdkcNode::OffCasIncDec)
{
	UnsetK2hdkcNodeCallback(info, stc_emitters[EMITTER_POS_CASINCDEC]);
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

NAN_METHOD(K2hdkcNode::Init)
{
	K2hdkcNode*		obj	= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());

	// reinitialize at first
	obj->Clean();

	// check arguments
	if(0 == info.Length()){
		Nan::Callback*	callback = obj->_cbs.Find(stc_emitters[EMITTER_POS_INIT]);
		if(callback){
			// [NOTE]
			// If callback is set, it calls worker. But it returns error,
			// because no configuration is specified.
			//
			Nan::AsyncQueueWorker(new InitWorker(callback, obj->_k2hdkcslave, NULL, 0, NULL, false, false));
		}
		// onetime connection type : nothing to do
		info.GetReturnValue().Set(Nan::True());
	}else{
		// permanent connection type
		Nan::Callback*	callback		= obj->_cbs.Find(stc_emitters[EMITTER_POS_INIT]);
		int				argpos			= 0;
		std::string		conf("");
		int16_t			ctlport			= CHM_INVALID_PORT;
		std::string		cuk("");
		bool			auto_rejoin		= false;
		bool			no_giveup_rejoin= false;

		if(info[argpos]->IsString()){
			// 1'st argument is conf
			Nan::Utf8String	buf(info[argpos++]);
			conf						= std::string(*buf);

			if(argpos < info.Length() && info[argpos]->IsNumber()){
				// 2'nd argument is port
				ctlport					= static_cast<int16_t>(Nan::To<int32_t>(info[argpos++]).FromJust());

				if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
					// 3'rd argument is cuk
					Nan::Utf8String	buf2(info[argpos++]);
					cuk					= std::string(*buf2);
					if(argpos < info.Length() && info[argpos]->IsBoolean()){
						// 4'th argument is auto rejoin
						auto_rejoin			= Nan::To<bool>(info[argpos++]).FromJust();

						if(argpos < info.Length() && info[argpos]->IsBoolean()){
							// 5'th argument is no giveup rejoin
							no_giveup_rejoin= Nan::To<bool>(info[argpos++]).FromJust();

							if(argpos < info.Length() && info[argpos]->IsFunction()){
								// 6'th argument is callback
								callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
							}
						}else if(argpos < info.Length() && info[argpos]->IsFunction()){
							// 5'th argument is callback
							callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
						}
					}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
						// 4'th argument is no giveup rejoin
						no_giveup_rejoin	= Nan::To<bool>(info[argpos++]).FromJust();

						if(argpos < info.Length() && info[argpos]->IsFunction()){
							// 5'th argument is callback
							callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
						}
					}else if(argpos < info.Length() && info[argpos]->IsFunction()){
						// 4'th argument is callback
						callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
					}

				}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
					// 3'rd argument is auto rejoin
					auto_rejoin			= Nan::To<bool>(info[argpos++]).FromJust();

					if(argpos < info.Length() && info[argpos]->IsBoolean()){
						// 4'th argument is no giveup rejoin
						no_giveup_rejoin= Nan::To<bool>(info[argpos++]).FromJust();

						if(argpos < info.Length() && info[argpos]->IsFunction()){
							// 5'th argument is callback
							callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
						}
					}else if(argpos < info.Length() && info[argpos]->IsFunction()){
						// 4'th argument is callback
						callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
					}
				}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
					// 3'rd argument is no giveup rejoin
					no_giveup_rejoin	= Nan::To<bool>(info[argpos++]).FromJust();

					if(argpos < info.Length() && info[argpos]->IsFunction()){
						// 4'th argument is callback
						callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
					}
				}else if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 3'rd argument is callback
					callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
				}

			}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
				// 2'nd argument is cuk
				Nan::Utf8String	buf3(info[argpos++]);
				cuk					= std::string(*buf3);
				if(argpos < info.Length() && info[argpos]->IsBoolean()){
					// 3'rd argument is auto rejoin
					auto_rejoin			= Nan::To<bool>(info[argpos++]).FromJust();

					if(argpos < info.Length() && info[argpos]->IsBoolean()){
						// 4'th argument is no giveup rejoin
						no_giveup_rejoin= Nan::To<bool>(info[argpos++]).FromJust();

						if(argpos < info.Length() && info[argpos]->IsFunction()){
							// 5'th argument is callback
							callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
						}
					}else if(argpos < info.Length() && info[argpos]->IsFunction()){
						// 4'th argument is callback
						callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
					}
				}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
					// 3'rd argument is no giveup rejoin
					no_giveup_rejoin	= Nan::To<bool>(info[argpos++]).FromJust();

					if(argpos < info.Length() && info[argpos]->IsFunction()){
						// 4'th argument is callback
						callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
					}
				}else if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 3'rd argument is callback
					callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
				}

			}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
				// 2'nd argument is auto rejoin
				auto_rejoin				= Nan::To<bool>(info[argpos++]).FromJust();

				if(argpos < info.Length() && info[argpos]->IsBoolean()){
					// 3'rd argument is no giveup rejoin
					no_giveup_rejoin	= Nan::To<bool>(info[argpos++]).FromJust();

					if(argpos < info.Length() && info[argpos]->IsFunction()){
						// 4'th argument is callback
						callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
					}
				}else if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 3'rd argument is callback
					callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
				// 2'nd argument is no giveup rejoin
				no_giveup_rejoin		= Nan::To<bool>(info[argpos++]).FromJust();

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 3'rd argument is callback
					callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 2'nd argument is callback
				callback				= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else{
			Nan::ThrowSyntaxError("First parameter is not configuration");
			return;
		}

		// check over arguments
		if(argpos < info.Length()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}

		// work
		obj->_k2hdkcslave	= new K2hdkcSlave();
		if(callback){
			Nan::AsyncQueueWorker(new InitWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin));
		}else{
			// build permanent connection object
			if(!obj->_k2hdkcslave->Initialize(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin)){
				obj->Clean();
				info.GetReturnValue().Set(Nan::False());
				return;
			}
			if(!obj->_k2hdkcslave->Open(no_giveup_rejoin)){
				obj->Clean();
				info.GetReturnValue().Set(Nan::False());
				return;
			}
		}
		info.GetReturnValue().Set(Nan::True());
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

NAN_METHOD(K2hdkcNode::Clean)
{
	K2hdkcNode*		obj = Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());

	obj->Clean();
	info.GetReturnValue().Set(Nan::True());
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

NAN_METHOD(K2hdkcNode::IsPermanent)
{
	K2hdkcNode*		obj = Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());

	if(obj->_k2hdkcslave){
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::False());
	}
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

NAN_METHOD(K2hdkcNode::GetValue)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_GET]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string	strkey;
	bool	is_subkey_set	= false;
	string	strsubkey;
	bool	attrchk			= true;
	bool	is_pass_set		= false;
	string	strpass;

	// target arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Nan::ThrowSyntaxError("There is no key name parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}
	Nan::Utf8String	buf2(info[argpos++]);
	strkey					= std::string(*buf2);

	// other arguments for target
	if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
		// target 2'nd parameter is subkey(string or null)
		if(info[argpos]->IsNull()){
			argpos++;
			is_subkey_set	= false;
		}else{
			Nan::Utf8String	buf3(info[argpos++]);
			strsubkey		= std::string(*buf3);
			is_subkey_set	= true;
		}

		if(argpos < info.Length() && info[argpos]->IsBoolean()){
			// target 3'rd parameter is attrchk
			attrchk			= Nan::To<bool>(info[argpos++]).FromJust();

			if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
				// target 4'th parameter is pass
				if(info[argpos]->IsNull()){
					argpos++;
					is_pass_set	= false;
				}else{
					Nan::Utf8String	buf3(info[argpos++]);
					strpass		= std::string(*buf3);
					is_pass_set	= true;
				}

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// target 5'th parameter is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}

			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 4'th parameter is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}

		}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// target 3'rd parameter is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set	= false;
			}else{
				Nan::Utf8String	buf3(info[argpos++]);
				strpass		= std::string(*buf3);
				is_pass_set	= true;
			}

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 4'th parameter is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}

		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 3'rd parameter is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}

	}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
		// target 2'nd parameter is attrchk
		attrchk				= Nan::To<bool>(info[argpos++]).FromJust();

		if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// target 3'rd parameter is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set	= false;
			}else{
				Nan::Utf8String	buf3(info[argpos++]);
				strpass		= std::string(*buf3);
				is_pass_set	= true;
			}

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 4'th parameter is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}

		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 3'rd parameter is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}

	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// target 2'nd parameter is callback
		callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new GetValueWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str(), (is_subkey_set ? strsubkey.c_str() : NULL), attrchk, (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		dkcres_type_t	rescode = DKC_NORESTYPE;
		if(is_subkey_set){
			// subkey is specified, thus need to check the key has it.
			K2hdkcComGetSubkeys*	pSubComObj;
			if(!obj->_k2hdkcslave){
				pSubComObj = GetOtSlaveK2hdkcComGetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pSubComObj = GetPmSlaveK2hdkcComGetSubkeys(obj->_k2hdkcslave);
			}
			if(!pSubComObj){
				Nan::ThrowSyntaxError("Internal error: Could not create command object.");
				return;
			}
			// get subkey list in key
			K2HSubKeys*	pSubKeys= NULL;
			if(!pSubComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, attrchk, &pSubKeys, &rescode) || !pSubKeys){
				// key does not have any subkey
				DKC_DELETE(pSubComObj);
				info.GetReturnValue().Set(Nan::Null());
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
				info.GetReturnValue().Set(Nan::Null());
				return;
			}
			// switch key to subkey
			strkey = strsubkey;
		}

		// get value
		K2hdkcComGet*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComGet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComGet(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
			return;
		}
		const unsigned char*	pvaltmp		= NULL;
		size_t					valtmplen	= 0L;
		bool					result		= pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, attrchk, (is_pass_set ? strpass.c_str() : NULL), &pvaltmp, &valtmplen, &rescode);
		if(result && (pvaltmp && 0 < valtmplen)){
			string	strresult(reinterpret_cast<const char*>(pvaltmp), valtmplen);
			info.GetReturnValue().Set(Nan::New<String>(strresult.c_str()).ToLocalChecked());
		}else{
			info.GetReturnValue().Set(Nan::Null());
		}
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::GetSubkeys)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_GETSUBKEYS]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string	strkey;
	bool	attrchk			= true;

	// target arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Nan::ThrowSyntaxError("There is no key name parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}
	Nan::Utf8String	buf2(info[argpos++]);
	strkey					= std::string(*buf2);

	// other arguments for target
	if(argpos < info.Length() && info[argpos]->IsBoolean()){
		// target 2'nd parameter is attrchk
		attrchk				= Nan::To<bool>(info[argpos++]).FromJust();

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 3'rd parameter is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}

	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// target 2'nd parameter is callback
		callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
	}
	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new GetSubkeysWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str(), attrchk));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComGetSubkeys*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComGetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComGetSubkeys(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
			return;
		}
		// get subkey list in key
		dkcres_type_t	rescode = DKC_NORESTYPE;
		K2HSubKeys*		pSubKeys= NULL;
		if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, attrchk, &pSubKeys, &rescode) || !pSubKeys){
			// key does not have any subkey
			info.GetReturnValue().Set(Nan::Null());
		}else{
			// convert subkey binary data to string array
			strarr_t	strarr;
			pSubKeys->StringArray(strarr);
			DKC_DELETE(pSubKeys);

			Local<Array>	retarr	= Nan::New<Array>();
			int				pos		= 0 ;
			for(strarr_t::const_iterator iter = strarr.begin(); iter != strarr.end(); ++iter, ++pos){
				Nan::Set(retarr, pos, Nan::New<String>(iter->c_str()).ToLocalChecked());
			}
			info.GetReturnValue().Set(retarr);
		}
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::GetAttrs)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_GETATTRS]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string	strkey;

	// target arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Nan::ThrowSyntaxError("There is no key name parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}
	Nan::Utf8String	buf(info[argpos++]);
	strkey			= std::string(*buf);

	// other arguments for target
	if(argpos < info.Length() && info[argpos]->IsFunction()){
		// target 2'nd parameter is callback
		callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
	}
	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new GetAttrsWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str()));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComGetAttrs*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComGetAttrs(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComGetAttrs(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
			return;
		}

		// get attribute list in key
		dkcres_type_t	rescode = DKC_NORESTYPE;
		K2HAttrs*		pAttrs	= NULL;
		if(!pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, &pAttrs, &rescode) || !pAttrs){
			// key does not have any attribute key
			info.GetReturnValue().Set(Nan::Null());
		}else{
			// convert attribute key binary data to string array
			strarr_t	strarr;
			pAttrs->KeyStringArray(strarr);
			DKC_DELETE(pAttrs);

			Local<Array>	retarr	= Nan::New<Array>();
			int				pos		= 0 ;
			for(strarr_t::const_iterator iter = strarr.begin(); iter != strarr.end(); ++iter, ++pos){
				Nan::Set(retarr, pos, Nan::New<String>(iter->c_str()).ToLocalChecked());
			}
			info.GetReturnValue().Set(retarr);
		}
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::SetValue)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_SET]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string	strkey;
	bool	is_val_set		= false;
	string	strval;
	bool	is_subkey_set	= false;
	string	strsubkey;
	bool	is_pass_set		= false;
	string	strpass;
	time_t	expire			= 0;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= (argpos + 1)){
		Nan::ThrowSyntaxError("There is no key name nor value parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strkey					= std::string(*buf);
	}
	// target value arguments(for both ontime and permanent connection)
	if(info[argpos]->IsString()){
		Nan::Utf8String	buf(info[argpos++]);
		strval					= std::string(*buf);
		is_val_set				= true;
	}else if(info[argpos]->IsNull()){
		argpos++;
		is_val_set				= false;
	}

	// other arguments for target
	if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
		// target 2'nd parameter is subkey(string or null)
		if(info[argpos]->IsNull()){
			argpos++;
			is_subkey_set	= false;
		}else{
			Nan::Utf8String	buf(info[argpos++]);
			strsubkey		= std::string(*buf);
			is_subkey_set	= true;
		}

		if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// target 3'rd parameter is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set		= false;
			}else{
				Nan::Utf8String	buf(info[argpos++]);
				strpass			= std::string(*buf);
				is_pass_set		= true;
			}

			if(argpos < info.Length() && info[argpos]->IsInt32()){
				// target 4'th parameter is expire
				int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// target 5'th parameter is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 4'th parameter is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsInt32()){
			// target 3'rd parameter is expire
			int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire				= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 4'th parameter is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 3'rd parameter is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsInt32()){
		// target 2'nd parameter is expire
		int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
		expire				= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 3'rd parameter is callback
			callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// target 2'nd parameter is callback
		callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new SetValueWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str(), (is_val_set ? strval.c_str() : NULL), (is_subkey_set ? strsubkey.c_str() : NULL), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		dkcres_type_t	rescode = DKC_NORESTYPE;
		bool			result	= false;
		if(is_subkey_set){
			// subkey is specified, set value into subkey
			K2hdkcComAddSubkey*	pComObj;
			if(!obj->_k2hdkcslave){
				pComObj = GetOtSlaveK2hdkcComAddSubkey(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComAddSubkey(obj->_k2hdkcslave);
			}
			if(!pComObj){
				Nan::ThrowSyntaxError("Internal error: Could not create command object.");
				return;
			}
			result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(strsubkey.c_str()), strsubkey.length() + 1, (is_val_set ? reinterpret_cast<const unsigned char*>(strval.c_str()) : NULL), (is_val_set ? strval.length() + 1 : 0), true, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode);
			DKC_DELETE(pComObj);
		}else{
			// set value to key
			K2hdkcComSet*	pComObj;
			if(!obj->_k2hdkcslave){
				pComObj = GetOtSlaveK2hdkcComSet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComSet(obj->_k2hdkcslave);
			}
			if(!pComObj){
				Nan::ThrowSyntaxError("Internal error: Could not create command object.");
				return;
			}
			result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, (is_val_set ? reinterpret_cast<const unsigned char*>(strval.c_str()) : NULL), (is_val_set ? strval.length() + 1 : 0), false, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode);
			DKC_DELETE(pComObj);
		}
		info.GetReturnValue().Set(Nan::New(result));
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

NAN_METHOD(K2hdkcNode::SetSubkeys)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_SETSUBKEYS]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string			strkey;
	unsigned char*	bysubkeys	= NULL;
	size_t			skeylen		= 0UL;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Nan::ThrowSyntaxError("There is no key name parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strkey					= std::string(*buf);
	}

	// other arguments for target
	if(argpos < info.Length() && (info[argpos]->IsArray() || info[argpos]->IsNull())){
		// target 2'nd parameter is subkeys(array or null)
		if(info[argpos]->IsNull()){
			argpos++;
		}else{
			// Array to binary
			Local<Array>	inSubkeys = Local<Array>::Cast(info[argpos++]);
			K2HSubKeys		Subkeys;
			for(int pos = 0; pos < static_cast<int>(inSubkeys->Length()); ++pos){
				string		tmpkey;
				{
					Nan::Utf8String	buf(Nan::Get(inSubkeys, pos).ToLocalChecked());
					tmpkey = std::string(*buf);
				}
				if(Subkeys.end() == Subkeys.insert(tmpkey.c_str())){
					// failed to set subkey
					info.GetReturnValue().Set(Nan::False());
					return;
				}
			}
			// serialize
			if(0UL < Subkeys.size()){
				if(!Subkeys.Serialize(&bysubkeys, skeylen)){
					// failed to set subkey
					info.GetReturnValue().Set(Nan::False());
					return;
				}
			}
		}

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 3'rd parameter is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}

	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// target 2'nd parameter is callback
		callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new SetSubkeysWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str(), bysubkeys, skeylen));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComSetSubkeys*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComSetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComSetSubkeys(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
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
		info.GetReturnValue().Set(Nan::New(result));
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::SetAll)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_SETALL]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string			strkey;
	bool			is_val_set	= false;
	string			strval;
	unsigned char*	bysubkeys	= NULL;
	size_t			skeylen		= 0UL;
	bool			is_pass_set	= false;
	string			strpass;
	time_t			expire		= 0;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= (argpos + 1)){
		Nan::ThrowSyntaxError("There is no key name nor value parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strkey					= std::string(*buf);
	}
	// target value arguments(for both ontime and permanent connection)
	if(info[argpos]->IsString()){
		Nan::Utf8String	buf(info[argpos++]);
		strval					= std::string(*buf);
		is_val_set				= true;
	}else if(info[argpos]->IsNull()){
		argpos++;
		is_val_set				= false;
	}

	// other arguments for target
	if(argpos < info.Length() && (info[argpos]->IsArray() || info[argpos]->IsNull())){
		// target 2'nd parameter is subkeys(array or null)
		if(info[argpos]->IsNull()){
			argpos++;
		}else{
			// Array to binary
			Local<Array>	inSubkeys = Local<Array>::Cast(info[argpos++]);
			K2HSubKeys		Subkeys;
			for(int pos = 0; pos < static_cast<int>(inSubkeys->Length()); ++pos){
				string		tmpkey;
				{
					Nan::Utf8String	buf(Nan::Get(inSubkeys, pos).ToLocalChecked());
					tmpkey = std::string(*buf);
				}
				if(Subkeys.end() == Subkeys.insert(tmpkey.c_str())){
					// failed to set subkey
					info.GetReturnValue().Set(Nan::False());
					return;
				}
			}
			// serialize
			if(0UL < Subkeys.size()){
				if(!Subkeys.Serialize(&bysubkeys, skeylen)){
					// failed to set subkey
					info.GetReturnValue().Set(Nan::False());
					return;
				}
			}
		}

		if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// target 3'rd parameter is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set		= false;
			}else{
				Nan::Utf8String	buf(info[argpos++]);
				strpass			= std::string(*buf);
				is_pass_set		= true;
			}

			if(argpos < info.Length() && info[argpos]->IsInt32()){
				// target 4'th parameter is expire
				int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// target 5'th parameter is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}

			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 4'th parameter is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}

		}else if(argpos < info.Length() && info[argpos]->IsInt32()){
			// target 3'rd parameter is expire
			int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire				= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 4'th parameter is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}

		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 3'rd parameter is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}

	}else if(argpos < info.Length() && info[argpos]->IsInt32()){
		// target 2'nd parameter is expire
		int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
		expire				= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 3'rd parameter is callback
			callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
		}

	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// target 2'nd parameter is callback
		callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new SetAllWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str(), (is_val_set ? strval.c_str() : NULL), bysubkeys, skeylen, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		bool			result	= false;
		dkcres_type_t	rescode = DKC_NORESTYPE;
		if(is_pass_set && 0 < expire){
			// set value with passphrase and expire, then the operation is separated.
			K2hdkcComSet*	pComObj;
			if(!obj->_k2hdkcslave){
				pComObj = GetOtSlaveK2hdkcComSet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComSet(obj->_k2hdkcslave);
			}
			if(!pComObj){
				Nan::ThrowSyntaxError("Internal error: Could not create command object.");
				return;
			}

			// set value to key
			result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, (is_val_set ? reinterpret_cast<const unsigned char*>(strval.c_str()) : NULL), (is_val_set ? strval.length() + 1 : 0), false, strpass.c_str(), &expire, &rescode);
			DKC_DELETE(pComObj);

			// set subkeys
			if(result && bysubkeys && 0UL < skeylen){

				K2hdkcComSetSubkeys*	pSubComObj;
				if(!obj->_k2hdkcslave){
					pSubComObj = GetOtSlaveK2hdkcComSetSubkeys(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
				}else{
					pSubComObj = GetPmSlaveK2hdkcComSetSubkeys(obj->_k2hdkcslave);
				}
				if(!pSubComObj){
					Nan::ThrowSyntaxError("Internal error: Could not create command object.");
					return;
				}

				// set subkeys to key
				result = pSubComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, bysubkeys, skeylen, &rescode);
				DKC_DELETE(pSubComObj);
			}

		}else{
			// no passphrase and no expire, then one action
			K2hdkcComSetAll*	pComObj;
			if(!obj->_k2hdkcslave){
				pComObj = GetOtSlaveK2hdkcComSetAll(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
			}else{
				pComObj = GetPmSlaveK2hdkcComSetAll(obj->_k2hdkcslave);
			}
			if(!pComObj){
				Nan::ThrowSyntaxError("Internal error: Could not create command object.");
				return;
			}
			// set value and subkeys
			result = pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, (is_val_set ? reinterpret_cast<const unsigned char*>(strval.c_str()) : NULL), (is_val_set ? strval.length() + 1 : 0), bysubkeys, skeylen, NULL, 0UL, &rescode);
			DKC_DELETE(pComObj);
		}
		info.GetReturnValue().Set(Nan::New(result));
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

NAN_METHOD(K2hdkcNode::Remove)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_REMOVE]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string			strkey;
	bool			is_subkeys	= false;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Nan::ThrowSyntaxError("There is no key name parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strkey					= std::string(*buf);
	}

	// other arguments for target
	if(argpos < info.Length() && info[argpos]->IsBoolean()){
		// target 2'nd parameter is subkey flag
		is_subkeys				= Nan::To<bool>(info[argpos++]).FromJust();

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 3'rd parameter is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// target 2'nd parameter is callback
		callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new RemoveWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str(), is_subkeys));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComDel*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComDel(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComDel(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
			return;
		}

		// remove key
		dkcres_type_t	rescode = DKC_NORESTYPE;
		info.GetReturnValue().Set(Nan::New(
			pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, is_subkeys, true, &rescode)
		));
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::Rename)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_RENAME]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string			strold;
	string			strnew;
	bool			is_parent_set	= false;
	string			strparent;
	bool			is_pass_set		= false;
	string			strpass;
	bool			attrchk			= true;
	time_t			expire			= 0;

	// target old key arguments(for both ontime and permanent connection)
	if(info.Length() <= (argpos + 1)){
		Nan::ThrowSyntaxError("There is no key name nor value parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The old key name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strold					= std::string(*buf);
	}
	// target new key arguments(for both ontime and permanent connection)
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The new key name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strnew					= std::string(*buf);
	}

	// other arguments for target
	if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
		// target 3'rd parameter is parent key
		if(info[argpos]->IsNull()){
			argpos++;
			is_parent_set		= false;
		}else{
			Nan::Utf8String	buf(info[argpos++]);
			strparent			= std::string(*buf);
			is_parent_set		= true;
		}

		if(argpos < info.Length() && info[argpos]->IsBoolean()){
			// target 4'th parameter is attrchk
			attrchk				= Nan::To<bool>(info[argpos++]).FromJust();

			if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
				// target 5'th parameter is pass
				if(info[argpos]->IsNull()){
					argpos++;
					is_pass_set	= false;
				}else{
					Nan::Utf8String	buf2(info[argpos++]);
					strpass		= std::string(*buf2);
					is_pass_set	= true;
				}

				if(argpos < info.Length() && info[argpos]->IsInt32()){
					// target 6'th parameter is expire
					int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
					expire				= static_cast<time_t>(nexpire);

					if(argpos < info.Length() && info[argpos]->IsFunction()){
						// target 7'th parameter is callback
						callback= new Nan::Callback(info[argpos++].As<v8::Function>());
					}
				}else if(argpos < info.Length() && info[argpos]->IsFunction()){
					// target 6'th parameter is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsInt32()){
				// target 5'th parameter is expire
				int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
				expire				= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// target 6'th parameter is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 5'th parameter is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// target 4'th parameter is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set	= false;
			}else{
				Nan::Utf8String	buf2(info[argpos++]);
				strpass		= std::string(*buf2);
				is_pass_set	= true;
			}

			if(argpos < info.Length() && info[argpos]->IsInt32()){
				// target 5'th parameter is expire
				int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
				expire				= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// target 6'th parameter is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 5'th parameter is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsInt32()){
			// target 4'th parameter is expire
			int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire				= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 5'th parameter is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 4'th parameter is callback
			callback= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
		// target 3'rd parameter is attrchk
		attrchk			= Nan::To<bool>(info[argpos++]).FromJust();

		if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// target 4'th parameter is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set	= false;
			}else{
				Nan::Utf8String	buf2(info[argpos++]);
				strpass		= std::string(*buf2);
				is_pass_set	= true;
			}

			if(argpos < info.Length() && info[argpos]->IsInt32()){
				// target 5'th parameter is expire
				int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
				expire				= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// target 6'th parameter is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 5'th parameter is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsInt32()){
			// target 4'th parameter is expire
			int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire				= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// target 5'th parameter is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 4'th parameter is callback
			callback= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsInt32()){
		// target 3'rd parameter is expire
		int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
		expire				= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// target 4'th parameter is callback
			callback= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// target 3'rd parameter is callback
		callback= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new RenameWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strold.c_str(), strnew.c_str(), (is_parent_set ? strparent.c_str() : NULL), attrchk, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComRen*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComRen(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComRen(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
			return;
		}

		dkcres_type_t	rescode = DKC_NORESTYPE;
		info.GetReturnValue().Set(Nan::New(
			pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strold.c_str()), strold.length() + 1, reinterpret_cast<const unsigned char*>(strnew.c_str()), strnew.length() + 1, (is_parent_set ? reinterpret_cast<const unsigned char*>(strparent.c_str()) : NULL), (is_parent_set ? strparent.length() + 1 : 0), attrchk, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode)
		));
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::QueuePush)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_QUEUEPUSH]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string			strprefix;
	bool			is_key_set		= false;
	string			strkey;
	string			strval;
	bool			is_fifo			= true;
	bool			attrchk			= true;
	bool			is_pass_set		= false;
	string			strpass;
	time_t			expire			= 0;

	// target prefix
	if(info.Length() <= (argpos + 1)){
		Nan::ThrowSyntaxError("There is no prefix nor value parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The prefix name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strprefix				= std::string(*buf);
	}

	// target (key and val) or val
	if(info[argpos]->IsString()){
		if((argpos + 1) < info.Length() && (info[argpos + 1]->IsString() || info[argpos + 1]->IsNull())){
			// 2'nd argument is key, 3'rd argument is value.
			Nan::Utf8String	buf1(info[argpos++]);
			strkey				= std::string(*buf1);					// temporary set to key buffer
			is_key_set			= true;
			Nan::Utf8String	buf2(info[argpos++]);
			strval				= std::string(*buf2);
		}else{
			// 2'nd argument is value(key is not set).
			Nan::Utf8String	buf(info[argpos++]);
			strval				= std::string(*buf);
			is_key_set			= false;
		}
	}else if(info[argpos]->IsNull()){
		if((argpos + 1) < info.Length() && (info[argpos + 1]->IsString() || info[argpos + 1]->IsNull())){
			// 2'nd argument is key( = null), 3'rd argument is value.
			is_key_set			= false;
			argpos++;
			Nan::Utf8String	buf(info[argpos++]);
			strval				= std::string(*buf);
		}else{
			Nan::ThrowSyntaxError("The key name or value must be string");
			return;
		}
	}else{
		Nan::ThrowSyntaxError("The key name or value must be string");
		return;
	}

	// other arguments for target
	if(argpos < info.Length() && info[argpos]->IsBoolean()){
		// 3'rd argument is fifo/lifo mode
		is_fifo					= Nan::To<bool>(info[argpos++]).FromJust();

		if(argpos < info.Length() && info[argpos]->IsBoolean()){
			// 4'th argument is attrchk
			attrchk				= Nan::To<bool>(info[argpos++]).FromJust();

			if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
				// 5'th argument is pass
				if(info[argpos]->IsNull()){
					argpos++;
					is_pass_set	= false;
				}else{
					Nan::Utf8String	buf(info[argpos++]);
					strpass		= std::string(*buf);
					is_pass_set	= true;
				}

				if(argpos < info.Length() && info[argpos]->IsInt32()){
					// 6'th argument is expire
					int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
					expire			= static_cast<time_t>(nexpire);

					if(argpos < info.Length() && info[argpos]->IsFunction()){
						// 7'th argument is callback
						callback= new Nan::Callback(info[argpos++].As<v8::Function>());
					}
				}else if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 6'th argument is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsInt32()){
				// 5'th argument is expire
				int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 6'th argument is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 5'th argument is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// 4'th argument is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set	= false;
			}else{
				Nan::Utf8String	buf(info[argpos++]);
				strpass		= std::string(*buf);
				is_pass_set	= true;
			}

			if(argpos < info.Length() && info[argpos]->IsInt32()){
				// 5'th argument is expire
				int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 6'th argument is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 5'th argument is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsInt32()){
			// 4'th argument is expire
			int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 5'th argument is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 4'th argument is callback
			callback= new Nan::Callback(info[argpos++].As<v8::Function>());
		}

	}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
		// 3'rd argument is attrchk
		attrchk				= Nan::To<bool>(info[argpos++]).FromJust();

		if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// 4'th argument is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set	= false;
			}else{
				Nan::Utf8String	buf(info[argpos++]);
				strpass		= std::string(*buf);
				is_pass_set	= true;
			}

			if(argpos < info.Length() && info[argpos]->IsInt32()){
				// 5'th argument is expire
				int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 6'th argument is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 5'th argument is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsInt32()){
			// 4'th argument is expire
			int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 5'th argument is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 4'th argument is callback
			callback= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
		// 3'rd argument is pass
		if(info[argpos]->IsNull()){
			argpos++;
			is_pass_set	= false;
		}else{
			Nan::Utf8String	buf(info[argpos++]);
			strpass		= std::string(*buf);
			is_pass_set	= true;
		}

		if(argpos < info.Length() && info[argpos]->IsInt32()){
			// 4'th argument is expire
			int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 5'th argument is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 5'th argument is callback
			callback= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsInt32()){
		// 3'rd argument is expire
		int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
		expire			= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 4'th argument is callback
			callback= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// 3'rd argument is callback
		callback= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new QueuePushWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strprefix.c_str(), (is_key_set ? strkey.c_str() : NULL), strval.c_str(),  is_fifo, attrchk, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComQPush*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComQPush(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComQPush(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
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
		info.GetReturnValue().Set(Nan::New(result));
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::QueuePop)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_QUEUEPOP]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string			strprefix;
	bool			is_fifo			= true;
	bool			is_key_queue	= false;
	bool			is_pass_set		= false;
	string			strpass;

	// target prefix
	if(info.Length() <= argpos){
		Nan::ThrowSyntaxError("There is no prefix parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The prefix name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strprefix				= std::string(*buf);
	}

	// other arguments for target
	if(argpos < info.Length() && info[argpos]->IsBoolean()){
		// 2'nd argument is fifo/lifo mode
		is_fifo					= Nan::To<bool>(info[argpos++]).FromJust();

		if(argpos < info.Length() && info[argpos]->IsBoolean()){
			// 3'rd argument is queue/keyqueue type
			is_key_queue		= Nan::To<bool>(info[argpos++]).FromJust();

			if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
				// 4'th argument is pass
				if(info[argpos]->IsNull()){
					argpos++;
					is_pass_set	= false;
				}else{
					Nan::Utf8String	buf(info[argpos++]);
					strpass		= std::string(*buf);
					is_pass_set	= true;
				}

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 5'th argument is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// 3'rd argument is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set	= false;
			}else{
				Nan::Utf8String	buf(info[argpos++]);
				strpass		= std::string(*buf);
				is_pass_set	= true;
			}

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 3'rd argument is callback
			callback= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
		// 2'nd argument is pass
		if(info[argpos]->IsNull()){
			argpos++;
			is_pass_set	= false;
		}else{
			Nan::Utf8String	buf(info[argpos++]);
			strpass		= std::string(*buf);
			is_pass_set	= true;
		}

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 3'rd argument is callback
			callback= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// 2'nd argument is callback
		callback= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new QueuePopWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strprefix.c_str(), is_fifo, is_key_queue, (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComQPop*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComQPop(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComQPop(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
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
				Local<Array>	retarr	= Nan::New<Array>();
				string			strtmpkey(reinterpret_cast<const char*>(pkeytmp), keytmplen);
				if(pvaltmp && 0 < valtmplen){
					string		strtmpval(reinterpret_cast<const char*>(pvaltmp), valtmplen);
					Nan::Set(retarr, 0, Nan::New<String>(strtmpkey.c_str()).ToLocalChecked());
					Nan::Set(retarr, 1, Nan::New<String>(strtmpval.c_str()).ToLocalChecked());
				}else{
					Nan::Set(retarr, 0, Nan::New<String>(strtmpkey.c_str()).ToLocalChecked());
					Nan::Set(retarr, 1, Nan::Null());
				}
				info.GetReturnValue().Set(retarr);
			}else{
				info.GetReturnValue().Set(Nan::Null());
			}
		}else{
			// queue
			const unsigned char*	pvaltmp		= NULL;
			size_t					valtmplen	= 0L;
			result = pComObj->QueueCommandSend(reinterpret_cast<const unsigned char*>(strprefix.c_str()), strprefix.length() + 1, is_fifo, true, (is_pass_set ? strpass.c_str() : NULL), &pvaltmp, &valtmplen, &rescode);

			if(result && (pvaltmp && 0 < valtmplen)){
				string	strresult(reinterpret_cast<const char*>(pvaltmp), valtmplen);
				info.GetReturnValue().Set(Nan::New<String>(strresult.c_str()).ToLocalChecked());
			}else{
				info.GetReturnValue().Set(Nan::Null());
			}
		}
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::QueueRemove)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_QUEUEREMOVE]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string			strprefix;
	int				count			= 0;
	bool			is_fifo			= true;
	bool			is_key_queue	= false;
	bool			is_pass_set		= false;
	string			strpass;

	// target prefix
	if(info.Length() <= argpos){
		Nan::ThrowSyntaxError("There is no prefix parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The prefix name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strprefix				= std::string(*buf);
	}

	// other arguments for target
	if(argpos < info.Length() && info[argpos]->IsInt32()){
		// 2'nd argument is count
		count					= Nan::To<int64_t>(info[argpos++]).FromJust();

		if(argpos < info.Length() && info[argpos]->IsBoolean()){
			// 3'rd argument is fifo/lifo mode
			is_fifo				= Nan::To<bool>(info[argpos++]).FromJust();

			if(argpos < info.Length() && info[argpos]->IsBoolean()){
				// 4'th argument is queue/keyqueue type
				is_key_queue	= Nan::To<bool>(info[argpos++]).FromJust();

				if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
					// 5'th argument is pass
					if(info[argpos]->IsNull()){
						argpos++;
						is_pass_set	= false;
					}else{
						Nan::Utf8String	buf(info[argpos++]);
						strpass		= std::string(*buf);
						is_pass_set	= true;
					}

					if(argpos < info.Length() && info[argpos]->IsFunction()){
						// 6'th argument is callback
						callback= new Nan::Callback(info[argpos++].As<v8::Function>());
					}
				}else if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 5'th argument is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
				// 4'th argument is pass
				if(info[argpos]->IsNull()){
					argpos++;
					is_pass_set	= false;
				}else{
					Nan::Utf8String	buf(info[argpos++]);
					strpass		= std::string(*buf);
					is_pass_set	= true;
				}

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 5'th argument is callback
					callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// 3'rd argument is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set	= false;
			}else{
				Nan::Utf8String	buf(info[argpos++]);
				strpass		= std::string(*buf);
				is_pass_set	= true;
			}

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 3'rd argument is callback
			callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsBoolean()){
		// 2'nd argument is fifo/lifo mode
		is_fifo					= Nan::To<bool>(info[argpos++]).FromJust();

		if(argpos < info.Length() && info[argpos]->IsBoolean()){
			// 3'rd argument is queue/keyqueue type
			is_key_queue		= Nan::To<bool>(info[argpos++]).FromJust();

			if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
				// 4'th argument is pass
				if(info[argpos]->IsNull()){
					argpos++;
					is_pass_set	= false;
				}else{
					Nan::Utf8String	buf(info[argpos++]);
					strpass		= std::string(*buf);
					is_pass_set	= true;
				}

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 5'th argument is callback
					callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// 3'rd argument is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set	= false;
			}else{
				Nan::Utf8String	buf(info[argpos++]);
				strpass		= std::string(*buf);
				is_pass_set	= true;
			}

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 3'rd argument is callback
			callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
		// 2'nd argument is pass
		if(info[argpos]->IsNull()){
			argpos++;
			is_pass_set	= false;
		}else{
			Nan::Utf8String	buf(info[argpos++]);
			strpass		= std::string(*buf);
			is_pass_set	= true;
		}

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 3'rd argument is callback
			callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// 2'nd argument is callback
		callback				= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new QueueRemoveWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strprefix.c_str(), count, is_fifo, is_key_queue, (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComQDel*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComQDel(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComQDel(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
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
		info.GetReturnValue().Set(Nan::New(result));
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::CasInit)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_CASINIT]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string		strkey;
	uint32_t	value		= 0;
	bool		is_pass_set	= false;
	string		strpass;
	time_t		expire		= 0;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Nan::ThrowSyntaxError("There is no key name parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strkey					= std::string(*buf);
	}

	// other arguments for target
	if(argpos < info.Length() && info[argpos]->IsUint32()){
		// 2'nd argument is value
		value			= Nan::To<uint32_t>(info[argpos++]).FromJust();

		if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// 3'rd argument is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set		= false;
			}else{
				Nan::Utf8String	buf(info[argpos++]);
				strpass			= std::string(*buf);
				is_pass_set		= true;
			}

			if(argpos < info.Length() && info[argpos]->IsInt32()){
				// 4'th argument is expire
				int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 5'th argument is callback
					callback= new Nan::Callback(info[argpos++].As<v8::Function>());
				}

			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}

		}else if(argpos < info.Length() && info[argpos]->IsInt32()){
			// 3'rd argument is expire
			int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire				= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}

		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 3'rd argument is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
		// 2'nd argument is pass
		if(info[argpos]->IsNull()){
			argpos++;
			is_pass_set		= false;
		}else{
			Nan::Utf8String	buf(info[argpos++]);
			strpass			= std::string(*buf);
			is_pass_set		= true;
		}

		if(argpos < info.Length() && info[argpos]->IsInt32()){
			// 3'rd argument is expire
			int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback= new Nan::Callback(info[argpos++].As<v8::Function>());
			}

		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 3'rd argument is callback
			callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
		}

	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// 2'nd argument is callback
		callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new CasInitWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str(), value, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComCasInit*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComCasInit(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComCasInit(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
			return;
		}
		dkcres_type_t	rescode = DKC_NORESTYPE;
		info.GetReturnValue().Set(Nan::New(pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(&value), sizeof(uint32_t), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode)));
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::CasGet)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_CASGET]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string		strkey;
	bool		is_pass_set	= false;
	string		strpass;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Nan::ThrowSyntaxError("There is no key name parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strkey					= std::string(*buf);
	}

	// other arguments for target
	if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
		// 3'rd argument is pass
		if(info[argpos]->IsNull()){
			argpos++;
			is_pass_set		= false;
		}else{
			Nan::Utf8String	buf(info[argpos++]);
			strpass			= std::string(*buf);
			is_pass_set		= true;
		}

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 4'th argument is callback
			callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// 3'rd argument is callback
		callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new CasGetWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str(), (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComCasGet*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComCasGet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComCasGet(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
			return;
		}

		dkcres_type_t			rescode		= DKC_NORESTYPE;
		const unsigned char*	pvaltmp		= NULL;
		size_t					valtmplen	= 0L;
		if(pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, true, (is_pass_set ? strpass.c_str() : NULL), &pvaltmp, &valtmplen, &rescode) && (valtmplen == sizeof(uint32_t))){
			uint32_t	resultval = 0;
			memcpy(reinterpret_cast<unsigned char*>(&resultval), pvaltmp, valtmplen);
			info.GetReturnValue().Set(Nan::New(resultval));
		}else{
			info.GetReturnValue().Set(Nan::Undefined());
		}
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::CasSet)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_CASSET]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string		strkey;
	uint32_t	oldval		= 0;
	uint32_t	newval		= 0;
	bool		is_pass_set	= false;
	string		strpass;
	time_t		expire		= 0;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= (argpos + 2)){
		Nan::ThrowSyntaxError("There is no key name parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strkey				= std::string(*buf);
	}
	// old value argument
	if(!info[argpos]->IsUint32()){
		Nan::ThrowSyntaxError("The old value must be uint");
		return;
	}else{
		oldval				= Nan::To<uint32_t>(info[argpos++]).FromJust();
	}
	// new value argument
	if(!info[argpos]->IsUint32()){
		Nan::ThrowSyntaxError("The new value must be uint");
		return;
	}else{
		newval				= Nan::To<uint32_t>(info[argpos++]).FromJust();
	}

	// other arguments for target
	if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
		// 4'th argument is pass
		if(info[argpos]->IsNull()){
			argpos++;
			is_pass_set		= false;
		}else{
			Nan::Utf8String	buf(info[argpos++]);
			strpass			= std::string(*buf);
			is_pass_set		= true;
		}

		if(argpos < info.Length() && info[argpos]->IsInt32()){
			// 5'th argument is expire
			int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 6'th argument is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}

		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 5'th argument is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}

	}else if(argpos < info.Length() && info[argpos]->IsInt32()){
		// 4'th argument is expire
		int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
		expire				= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 5'th argument is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}

	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// 4'th argument is callback
		callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new CasSetWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str(), oldval, newval, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComCasSet*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComCasSet(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComCasSet(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
			return;
		}

		dkcres_type_t	rescode = DKC_NORESTYPE;
		info.GetReturnValue().Set(Nan::New(pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, reinterpret_cast<const unsigned char*>(&oldval), sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&newval), sizeof(uint32_t), true, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode)));
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::CasIncDec)
{
	K2hdkcNode*		obj		= Nan::ObjectWrap::Unwrap<K2hdkcNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_emitters[EMITTER_POS_CASINCDEC]);
	int				argpos	= 0;

	// for onetime connection
	ParseArgumentsForOnetime(argpos);

	// target arguments
	string		strkey;
	bool		is_increment= true;
	bool		is_pass_set	= false;
	string		strpass;
	time_t		expire		= 0;

	// target key arguments(for both ontime and permanent connection)
	if(info.Length() <= argpos){
		Nan::ThrowSyntaxError("There is no key name parameter.");
		return;
	}
	if(!info[argpos]->IsString()){
		Nan::ThrowSyntaxError("The key name must be string");
		return;
	}else{
		Nan::Utf8String	buf(info[argpos++]);
		strkey				= std::string(*buf);
	}

	// other arguments for target
	if(argpos < info.Length() && info[argpos]->IsBoolean()){
		// 2'nd argument is increment/decrement mode
		is_increment		= Nan::To<bool>(info[argpos++]).FromJust();

		if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
			// 3'rd argument is pass
			if(info[argpos]->IsNull()){
				argpos++;
				is_pass_set		= false;
			}else{
				Nan::Utf8String	buf(info[argpos++]);
				strpass			= std::string(*buf);
				is_pass_set		= true;
			}

			if(argpos < info.Length() && info[argpos]->IsInt32()){
				// 4'th argument is expire
				int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
				expire			= static_cast<time_t>(nexpire);

				if(argpos < info.Length() && info[argpos]->IsFunction()){
					// 5'th argument is callback
					callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
				}
			}else if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsInt32()){
			// 3'rd argument is expire
			int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire				= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 3'rd argument is callback
			callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && (info[argpos]->IsString() || info[argpos]->IsNull())){
		// 2'nd argument is pass
		if(info[argpos]->IsNull()){
			argpos++;
			is_pass_set		= false;
		}else{
			Nan::Utf8String	buf(info[argpos++]);
			strpass			= std::string(*buf);
			is_pass_set		= true;
		}

		if(argpos < info.Length() && info[argpos]->IsInt32()){
			// 3'rd argument is expire
			int32_t	nexpire	= Nan::To<int32_t>(info[argpos++]).FromJust();
			expire			= static_cast<time_t>(nexpire);

			if(argpos < info.Length() && info[argpos]->IsFunction()){
				// 4'th argument is callback
				callback	= new Nan::Callback(info[argpos++].As<v8::Function>());
			}
		}else if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 3'rd argument is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsInt32()){
		// 2'nd argument is expire
		int32_t	nexpire		= Nan::To<int32_t>(info[argpos++]).FromJust();
		expire				= static_cast<time_t>(nexpire);

		if(argpos < info.Length() && info[argpos]->IsFunction()){
			// 3'rd argument is callback
			callback		= new Nan::Callback(info[argpos++].As<v8::Function>());
		}
	}else if(argpos < info.Length() && info[argpos]->IsFunction()){
		// 2'nd argument is callback
		callback			= new Nan::Callback(info[argpos++].As<v8::Function>());
	}

	// check over arguments
	if(argpos < info.Length()){
		Nan::ThrowSyntaxError("Last parameter is not callback function.");
		return;
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new CasIncDecWorker(callback, obj->_k2hdkcslave, conf.c_str(), ctlport, cuk.c_str(), auto_rejoin, no_giveup_rejoin, strkey.c_str(), is_increment, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2hdkcComCasIncDec*	pComObj;
		if(!obj->_k2hdkcslave){
			pComObj = GetOtSlaveK2hdkcComCasIncDec(conf.c_str(), ctlport, (cuk.empty() ? NULL : cuk.c_str()), auto_rejoin, no_giveup_rejoin);
		}else{
			pComObj = GetPmSlaveK2hdkcComCasIncDec(obj->_k2hdkcslave);
		}
		if(!pComObj){
			Nan::ThrowSyntaxError("Internal error: Could not create command object.");
			return;
		}

		dkcres_type_t	rescode = DKC_NORESTYPE;
		info.GetReturnValue().Set(Nan::New(pComObj->CommandSend(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, is_increment, true, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL), &rescode)));
		DKC_DELETE(pComObj);
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

NAN_METHOD(K2hdkcNode::PrintVersion)
{
	int			fd	= (0 < info.Length() && info[0]->IsInt32()) ? Nan::To<int32_t>(info[0]).FromJust() : -1;
	FILE*		fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}
	k2hdkc_print_version(fp);

	// [NOTE]
	// Must flush at here, because nodejs's file descriptor is used for fd.
	// Otherwise, calling flash on nodejs(javascript) is not effected.
	fflush(fp);

	info.GetReturnValue().Set(Nan::True());
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
