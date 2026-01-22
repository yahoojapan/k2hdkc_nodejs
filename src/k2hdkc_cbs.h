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

#ifndef K2HDKC_CBS_H
#define K2HDKC_CBS_H

#include <string>
#include <unordered_map>
#include "k2hdkc_common.h"

//---------------------------------------------------------
// Typedefs
//---------------------------------------------------------
typedef std::unordered_map<std::string, Napi::FunctionReference>	cbsmap;

//---------------------------------------------------------
// StackEmitCB Class
//---------------------------------------------------------
class StackEmitCB
{
	public:
		StackEmitCB();
		virtual ~StackEmitCB();

		// Set returns true if set succeeded
		bool Set(const std::string& emitter, const Napi::Function& cb);

		// Unset returns true if removed
		bool Unset(const std::string& emitter);

		// Find returns pointer to FunctionReference if set, otherwise nullptr
		Napi::FunctionReference* Find(const std::string& emitter);

	private:
		cbsmap			EmitCbsMap;
		volatile int	lockval;				// lock variable for mapping
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
