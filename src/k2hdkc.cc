/*
 * K2HDKC
 * 
 * Copyright 2017 Yahoo! JAPAN corporation.
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

using namespace v8 ;

//---------------------------------------------------------
// k2hdkc node object
//---------------------------------------------------------
NAN_METHOD(CreateObject)
{
	K2hdkcNode::NewInstance(info);
}

void InitAll(Local<Object> exports, Local<Object> module)
{
	K2hdkcNode::Init();
	Nan::SetMethod(module, "exports", CreateObject);
}

NODE_MODULE(k2hdkc , InitAll)

/*
 * VIM modelines
 *
 * vim:set ts=4 fenc=utf-8:
 */
