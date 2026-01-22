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
 * CREATE:   Thu 15 Jan 2026
 * REVISION:
 */

import resolve	from '@rollup/plugin-node-resolve';
import commonjs	from '@rollup/plugin-commonjs';
import terser	from '@rollup/plugin-terser';
import replace	from '@rollup/plugin-replace';

export default
{
	input:	'build/esm/index.js',
	output: {
		file:		'build/esm/index.mjs',
		format:		'es',
		sourcemap:	true
	},
	plugins: [
		resolve({
			preferBuiltins: true
		}),
		commonjs(),
		replace({
			'process.env.NODE_ENV':	JSON.stringify(process.env.NODE_ENV || 'production'),
			preventAssignment:		true
		}),
		terser()
	],
	external: [
		'bindings',
		'node-addon-api'
	]
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
