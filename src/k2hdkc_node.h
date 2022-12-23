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
class K2hdkcNode : public Nan::ObjectWrap
{
	public:
		static void	Init(void);
		static NAN_METHOD(NewInstance);
		static v8::Local<v8::Object> GetInstance(Nan::NAN_METHOD_ARGS_TYPE info);

	private:
		K2hdkcNode(void);
		~K2hdkcNode(void);

		void Clean(void);

		static NAN_METHOD(New);

		static NAN_METHOD(On);
		static NAN_METHOD(OnInit);
		static NAN_METHOD(OnGet);
		static NAN_METHOD(OnGetSubkeys);
		static NAN_METHOD(OnGetAttrs);
		static NAN_METHOD(OnSet);
		static NAN_METHOD(OnSetSubkeys);
		static NAN_METHOD(OnSetAll);
		static NAN_METHOD(OnRemove);
		static NAN_METHOD(OnRename);
		static NAN_METHOD(OnQueuePush);
		static NAN_METHOD(OnQueuePop);
		static NAN_METHOD(OnQueueRemove);
		static NAN_METHOD(OnCasInit);
		static NAN_METHOD(OnCasGet);
		static NAN_METHOD(OnCasSet);
		static NAN_METHOD(OnCasIncDec);
		static NAN_METHOD(Off);
		static NAN_METHOD(OffInit);
		static NAN_METHOD(OffGet);
		static NAN_METHOD(OffGetSubkeys);
		static NAN_METHOD(OffGetAttrs);
		static NAN_METHOD(OffSet);
		static NAN_METHOD(OffSetSubkeys);
		static NAN_METHOD(OffSetAll);
		static NAN_METHOD(OffRemove);
		static NAN_METHOD(OffRename);
		static NAN_METHOD(OffQueuePush);
		static NAN_METHOD(OffQueuePop);
		static NAN_METHOD(OffQueueRemove);
		static NAN_METHOD(OffCasInit);
		static NAN_METHOD(OffCasGet);
		static NAN_METHOD(OffCasSet);
		static NAN_METHOD(OffCasIncDec);

		static NAN_METHOD(Init);
		static NAN_METHOD(Clean);
		static NAN_METHOD(IsPermanent);
		static NAN_METHOD(GetValue);
		static NAN_METHOD(GetSubkeys);
		static NAN_METHOD(GetAttrs);
		static NAN_METHOD(SetValue);			// Alias to AddSubkey
		static NAN_METHOD(SetSubkeys);			// Alias to ClearSubkeys
		static NAN_METHOD(SetAll);
		static NAN_METHOD(Remove);
		static NAN_METHOD(Rename);
		static NAN_METHOD(QueuePush);
		static NAN_METHOD(QueuePop);
		static NAN_METHOD(QueueRemove);			// Alias to QueueDel
		static NAN_METHOD(CasInit);
		static NAN_METHOD(CasGet);
		static NAN_METHOD(CasSet);
		static NAN_METHOD(CasIncDec);
		static NAN_METHOD(PrintVersion);

	private:
		static Nan::Persistent<v8::Function>	constructor;
		K2hdkcSlave*							_k2hdkcslave;
		StackEmitCB								_cbs;
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
