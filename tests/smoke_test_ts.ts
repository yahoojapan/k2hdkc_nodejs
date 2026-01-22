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
 * CREATE:   Fri 9 Nov 2018
 * REVISION:
 */

//---------------------------------------------------------
// TypeScript Smoke Test
//---------------------------------------------------------
// [Purpose]
//	- Verify that TypeScript type definitions(.d.ts) are written
//	  correctly(pass type checks).
//	  Only compile(type check) without executing(tsc --noEmit).
//	- Verify that type representations(constructible/callable
//	   k2hdkc, classes within namespaces, etc.) are provided
//	   to users correctly.
//
// [Outline]
//	- Sample code that uses TypeScript import statements to
//	  retrieve types/values from packages and instantiate and
//	  reference types.
//	- Perform type checks only with "tsc --noEmit tests/test_ts.ts".
//	  If no errors are found, determine that the .d.ts is
//	  functioning as expected.
//
// [Expected]
//	- tsc returns no errors(exit code 0).
//	  If there are no errors, the type definitions cover basic APIs.
//
// [Meaning of Failure]
//	- TS2309 / TS2300, etc.(mixing export = and top-level export,
//	  duplicate definitions)
//		Invalid .d.ts structure.
//	- Type mismatch(whether a function can be newed / whether
//	  method overloading is correct, etc.)
//		Discrepancy between the .d.ts signature and the
//		actual implementation.
//	- Module resolution error
//		Invalid "types" field in package.json, inappropriate
//		moduleResolution in tsconfig, etc.
//
// [NOTE]
// This does not verify the runtime behavior of values(no node
// execution). Note that only types are verified.
// This test can also cover whether type-specific imports such
// as "import type { K2hdkcNode } from 'k2hdkc'" work(depending on
// the ambient module settings).
//
// ex)	tsc --noEmit tests/test_ts.ts
//			or
//		npx tsc --noEmit tests/test_ts.ts
//
//---------------------------------------------------------

import { K2hdkcNode }	from '..';

function main()
{
	const	obj = new K2hdkcNode();
	console.log('TS: constructed K2hdkcNode, typeof:', typeof obj);
}

main();

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
