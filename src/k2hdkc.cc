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

#include <napi.h>
#include "k2hdkc_node.h"

//---------------------------------------------------------
// k2hdkc node object
//---------------------------------------------------------
// [NOTE]
// The logic for receiving arguments when switching to N-API has been removed.
// This is because the arguments were not used in the first place and did not
// need to be defined.
//
Napi::Value CreateObject(const Napi::CallbackInfo& info)
{
	return K2hdkcNode::NewInstance(info);
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
	// Class registration (creating a constructor)
	K2hdkcNode::Init(env, exports);

	// Create a factory function that returns module.exports
	Napi::Function createFn = Napi::Function::New(env, CreateObject, "k2hdkc");

	// Allow to use "require('k2hdkc').K2hdkcNode"
	createFn.Set("K2hdkcNode", K2hdkcNode::constructor.Value());

	// Replace module.exports with this function (does not break existing "require('k2hdkc')()".)
	return createFn;
}

NODE_API_MODULE(k2hdkc, InitAll)

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
