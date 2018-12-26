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

#ifndef K2HDKC_CBS_H
#define K2HDKC_CBS_H

#include "k2hdkc_common.h"

//---------------------------------------------------------
// Typedefs
//---------------------------------------------------------
typedef std::map<std::string, Nan::Callback*>	cbsmap;

//---------------------------------------------------------
// StackEmitCB Class
//---------------------------------------------------------
class StackEmitCB
{
	protected:
		cbsmap			EmitCbsMap;
		volatile int	lockval;				// lock variable for mapping

	protected:
		Nan::Callback* RawFind(const char* pemitname);

	public:
		StackEmitCB();
		virtual ~StackEmitCB();

		Nan::Callback* Find(const char* pemitname);
		bool Set(const char* pemitname, Nan::Callback* cbfunc);
		bool Unset(const char* pemitname);
};

#endif

/*
 * VIM modelines
 *
 * vim:set ts=4 fenc=utf-8:
 */
