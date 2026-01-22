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

#include <fullock/flckstructure.h>
#include <fullock/flckbaselist.tcc>
#include "k2hdkc_cbs.h"

using namespace std;
using namespace fullock;

//---------------------------------------------------------
// StackEmitCB Class
//---------------------------------------------------------
StackEmitCB::StackEmitCB() : lockval(FLCK_NOSHARED_MUTEX_VAL_UNLOCKED)
{
}

StackEmitCB::~StackEmitCB()
{
	while(!flck_trylock_noshared_mutex(&lockval));	// LOCK
	EmitCbsMap.clear();
	flck_unlock_noshared_mutex(&lockval);			// UNLOCK
}

bool StackEmitCB::Set(const std::string& emitter, const Napi::Function& cb)
{
	while(!flck_trylock_noshared_mutex(&lockval));	// LOCK

	// clear if existed
	auto it = EmitCbsMap.find(emitter);
	if(EmitCbsMap.end() != it){
		it->second.Reset();
		EmitCbsMap.erase(it);
	}

	// Insert new persistent reference
	Napi::FunctionReference ref = Napi::Persistent(cb);
	EmitCbsMap.emplace(emitter, std::move(ref));

	flck_unlock_noshared_mutex(&lockval);			// UNLOCK

	return true;
}

bool StackEmitCB::Unset(const std::string& emitter)
{
	while(!flck_trylock_noshared_mutex(&lockval));	// LOCK

	auto it = EmitCbsMap.find(emitter);
	if(EmitCbsMap.end() == it){
		flck_unlock_noshared_mutex(&lockval);		// UNLOCK
		return false;
	}
	it->second.Reset();
	EmitCbsMap.erase(it);

	flck_unlock_noshared_mutex(&lockval);			// UNLOCK

	return true;
}

Napi::FunctionReference* StackEmitCB::Find(const std::string& emitter)
{
	while(!flck_trylock_noshared_mutex(&lockval));	// LOCK

	auto it = EmitCbsMap.find(emitter);
	if(EmitCbsMap.end() == it){
		flck_unlock_noshared_mutex(&lockval);		// UNLOCK
		return nullptr;
	}
	flck_unlock_noshared_mutex(&lockval);			// UNLOCK

	return &it->second;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
