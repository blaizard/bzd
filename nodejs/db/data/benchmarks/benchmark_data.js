import { performance } from "node:perf_hooks";
import Data from "#bzd/nodejs/db/data/data.js";

const singleIterations = 100000;
const batchIterations = 1000;
const batchSize = 100;
const singleWarmup = 1000;
const batchWarmup = 200;

/// Build `count` distinct keys of the given path depth.
function makeKeys(count, depth) {
	const tails = ["b", "c", "d", "e", "f", "g", "h", "i", "j"];
	const keys = new Array(count);
	for (let i = 0; i < count; ++i) {
		const key = ["k" + i];
		for (let d = 1; d < depth; ++d) key.push(tails[d - 1]);
		keys[i] = key;
	}
	return keys;
}

/// Run `fn(ctx)` `iterations` times after discarding `warmup` runs.
function bench(label, setup, fn, iterations, warmup) {
	const ctx = setup(iterations + warmup);
	ctx.counter = 0;
	for (let i = 0; i < warmup; ++i) fn(ctx);
	// ctx.counter is now at `warmup` — do NOT reset it.
	const start = performance.now();
	for (let i = 0; i < iterations; ++i) fn(ctx);
	const totalMs = Math.max(performance.now() - start, 1e-9);
	const opsPerSec = (iterations / totalMs) * 1000;
	const nsPerOp = (totalMs * 1e6) / iterations;
	return { label, iterations, totalMs, opsPerSec, nsPerOp };
}

function printResults(results) {
	const header = ["label", "iterations", "total ms", "ops/sec", "ns/op"];
	const rows = results.map((r) => [
		r.label,
		String(r.iterations),
		r.totalMs.toFixed(3),
		Math.round(r.opsPerSec).toLocaleString("en-US"),
		r.nsPerOp.toFixed(1),
	]);
	const widths = header.map((_, i) => Math.max(header[i].length, ...rows.map((r) => r[i].length)));
	const fmt = (cells) => cells.map((c, i) => c.padEnd(widths[i])).join(" | ");
	console.log(fmt(header));
	console.log(widths.map((w) => "-".repeat(w)).join("-+-"));
	for (const row of rows) console.log(fmt(row));
}

const results = [
	// 1. First/new key + auto timestamp (Date.now() branch).
	bench(
		"single-insert (auto timestamp, new key)",
		(total) => ({ data: new Data(), keys: makeKeys(total, 2) }),
		(ctx) => {
			const key = ctx.keys[ctx.counter++];
			ctx.data.insert("uid", [[key, ctx.counter]]);
		},
		singleIterations,
		singleWarmup,
	),

	// 2. In-order append into one key: EWMA auto-expiry branch.
	bench(
		"single-insert (append in-order)",
		(total) => {
			const data = new Data();
			data.insert("uid", [[["root"], 0]], 1000);
			return { data, t: 1000 };
		},
		(ctx) => {
			ctx.data.insert("uid", [[["root"], ctx.counter]], ++ctx.t);
		},
		singleIterations,
		singleWarmup,
	),

	// 3. Out-of-order (older than all existing): full findIndex scan.
	bench(
		"single-insert (out-of-order, older than all)",
		(total) => {
			const data = new Data();
			for (let i = 0; i < 100; ++i) {
				data.insert("uid", [[["root"], i]], 1000 + i);
			}
			return { data, t: 999 };
		},
		(ctx) => {
			ctx.data.insert("uid", [[["root"], ctx.counter, { history: 10000 }]], ctx.t--);
		},
		singleIterations,
		singleWarmup,
	),

	// 4. History trimming: pre-fill to cap (history=10).
	bench(
		"single-insert (history trim)",
		(total) => {
			const data = new Data();
			for (let i = 0; i < 10; ++i) {
				data.insert("uid", [[["root"], i]], 2000 + i);
			}
			return { data, t: 2010 };
		},
		(ctx) => {
			ctx.data.insert("uid", [[["root"], ctx.counter, { history: 10 }]], ++ctx.t);
		},
		singleIterations,
		singleWarmup,
	),

	// 5. Manual expires option.
	bench(
		"single-insert (manual expires option)",
		(total) => ({ data: new Data(), keys: makeKeys(total, 2) }),
		(ctx) => {
			const key = ctx.keys[ctx.counter++];
			ctx.data.insert("uid", [[key, ctx.counter, { expires: 5000 }]]);
		},
		singleIterations,
		singleWarmup,
	),

	// 6. Explicit timestamp (no Date.now() call). Compare with #1.
	bench(
		"single-insert (explicit timestamp, new key)",
		(total) => ({ data: new Data(), keys: makeKeys(total, 2) }),
		(ctx) => {
			const key = ctx.keys[ctx.counter++];
			ctx.data.insert("uid", [[key, ctx.counter]], 12345 + ctx.counter);
		},
		singleIterations,
		singleWarmup,
	),

	// 7. Deep nested key (depth 6). Compare with #1 (depth 2).
	bench(
		"single-insert (deep nested key depth 6)",
		(total) => ({ data: new Data(), keys: makeKeys(total, 6) }),
		(ctx) => {
			const key = ctx.keys[ctx.counter++];
			ctx.data.insert("uid", [[key, ctx.counter]], 12345);
		},
		singleIterations,
		singleWarmup,
	),

	// 8. Batch insert, flat keys, 100 fragments per call.
	bench(
		"batch-insert (flat keys, 100 frags)",
		(total) => {
			const data = new Data();
			const batches = new Array(total);
			for (let b = 0; b < total; ++b) {
				const frags = new Array(batchSize);
				for (let f = 0; f < batchSize; ++f) {
					frags[f] = [["k" + (b * batchSize + f)], b * batchSize + f];
				}
				batches[b] = frags;
			}
			return { data, batches };
		},
		(ctx) => {
			const frags = ctx.batches[ctx.counter++];
			ctx.data.insert("uid", frags, 12345);
		},
		batchIterations,
		batchWarmup,
	),

	// 9. Batch insert, deep keys (depth 5), 100 fragments per call.
	bench(
		"batch-insert (deep keys depth 5, 100 frags)",
		(total) => {
			const data = new Data();
			const tails = ["b", "c", "d", "e"];
			const batches = new Array(total);
			for (let b = 0; b < total; ++b) {
				const frags = new Array(batchSize);
				for (let f = 0; f < batchSize; ++f) {
					const key = ["k" + (b * batchSize + f)];
					for (const t of tails) key.push(t);
					frags[f] = [key, b * batchSize + f];
				}
				batches[b] = frags;
			}
			return { data, batches };
		},
		(ctx) => {
			const frags = ctx.batches[ctx.counter++];
			ctx.data.insert("uid", frags, 12345);
		},
		batchIterations,
		batchWarmup,
	),
];

printResults(results);
