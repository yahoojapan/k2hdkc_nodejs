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

#ifndef K2HDKC_NODE_H
#define K2HDKC_NODE_H

#include "k2hdkc_common.h"
#include "k2hdkc_cbs.h"

//---------------------------------------------------------
// K2hdkcNode Class
//---------------------------------------------------------
class K2hdkcNode : public Napi::ObjectWrap<K2hdkcNode>
{
	public:
		static void Init(Napi::Env env, Napi::Object exports);
		static Napi::Object NewInstance(const Napi::CallbackInfo& info);
		static Napi::Object GetInstance(const Napi::CallbackInfo& info);

		// Constructor / Destructor
		explicit K2hdkcNode(const Napi::CallbackInfo& info);
		~K2hdkcNode();

	private:
		static bool ParseArgumentsForOnetime(const Napi::CallbackInfo& info, K2hdkcNode* obj, size_t& argpos, std::string& conf, int16_t& ctlport, std::string& cuk, bool& auto_rejoin, bool& no_giveup_rejoin);

		void Clean(void);
		bool IsInitialize(void);

		Napi::Value On(const Napi::CallbackInfo& info);
		Napi::Value OnInit(const Napi::CallbackInfo& info);
		Napi::Value OnGet(const Napi::CallbackInfo& info);
		Napi::Value OnGetSubkeys(const Napi::CallbackInfo& info);
		Napi::Value OnGetAttrs(const Napi::CallbackInfo& info);
		Napi::Value OnSet(const Napi::CallbackInfo& info);
		Napi::Value OnSetSubkeys(const Napi::CallbackInfo& info);
		Napi::Value OnSetAll(const Napi::CallbackInfo& info);
		Napi::Value OnRemove(const Napi::CallbackInfo& info);
		Napi::Value OnRename(const Napi::CallbackInfo& info);
		Napi::Value OnQueuePush(const Napi::CallbackInfo& info);
		Napi::Value OnQueuePop(const Napi::CallbackInfo& info);
		Napi::Value OnQueueRemove(const Napi::CallbackInfo& info);
		Napi::Value OnCasInit(const Napi::CallbackInfo& info);
		Napi::Value OnCasGet(const Napi::CallbackInfo& info);
		Napi::Value OnCasSet(const Napi::CallbackInfo& info);
		Napi::Value OnCasIncDec(const Napi::CallbackInfo& info);
		Napi::Value Off(const Napi::CallbackInfo& info);
		Napi::Value OffInit(const Napi::CallbackInfo& info);
		Napi::Value OffGet(const Napi::CallbackInfo& info);
		Napi::Value OffGetSubkeys(const Napi::CallbackInfo& info);
		Napi::Value OffGetAttrs(const Napi::CallbackInfo& info);
		Napi::Value OffSet(const Napi::CallbackInfo& info);
		Napi::Value OffSetSubkeys(const Napi::CallbackInfo& info);
		Napi::Value OffSetAll(const Napi::CallbackInfo& info);
		Napi::Value OffRemove(const Napi::CallbackInfo& info);
		Napi::Value OffRename(const Napi::CallbackInfo& info);
		Napi::Value OffQueuePush(const Napi::CallbackInfo& info);
		Napi::Value OffQueuePop(const Napi::CallbackInfo& info);
		Napi::Value OffQueueRemove(const Napi::CallbackInfo& info);
		Napi::Value OffCasInit(const Napi::CallbackInfo& info);
		Napi::Value OffCasGet(const Napi::CallbackInfo& info);
		Napi::Value OffCasSet(const Napi::CallbackInfo& info);
		Napi::Value OffCasIncDec(const Napi::CallbackInfo& info);

		Napi::Value Init(const Napi::CallbackInfo& info);
		Napi::Value Clean(const Napi::CallbackInfo& info);
		Napi::Value IsPermanent(const Napi::CallbackInfo& info);
		Napi::Value GetValue(const Napi::CallbackInfo& info);
		Napi::Value GetSubkeys(const Napi::CallbackInfo& info);
		Napi::Value GetAttrs(const Napi::CallbackInfo& info);
		Napi::Value SetValue(const Napi::CallbackInfo& info);		// Alias to AddSubkey
		Napi::Value SetSubkeys(const Napi::CallbackInfo& info);		// Alias to ClearSubkeys
		Napi::Value SetAll(const Napi::CallbackInfo& info);
		Napi::Value Remove(const Napi::CallbackInfo& info);
		Napi::Value Rename(const Napi::CallbackInfo& info);
		Napi::Value QueuePush(const Napi::CallbackInfo& info);
		Napi::Value QueuePop(const Napi::CallbackInfo& info);
		Napi::Value QueueRemove(const Napi::CallbackInfo& info);	// Alias to QueueDel
		Napi::Value CasInit(const Napi::CallbackInfo& info);
		Napi::Value CasGet(const Napi::CallbackInfo& info);
		Napi::Value CasSet(const Napi::CallbackInfo& info);
		Napi::Value CasIncDec(const Napi::CallbackInfo& info);
		Napi::Value PrintVersion(const Napi::CallbackInfo& info);

	public:
		// constructor reference
		static Napi::FunctionReference	constructor;

		StackEmitCB	_cbs;

	private:
		K2hdkcSlave	_k2hdkcslave;
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
