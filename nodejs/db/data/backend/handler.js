import { timestampMs } from "#bzd/nodejs/utils/timestamp.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";

const Exception = ExceptionFactory("db", "data");
const Log = LogFactory("db", "data");

function pathToKey(path) {
	return path.split("/").filter(Boolean).map(decodeURIComponent);
}

/// Retrieve values from the data.
///
/// GET <endpoint>
/// ```{data: <value>}``` -> Return the raw value at the given path.
///
/// GET <endpoint>?metadata=1
/// ```{
///   timestamp: <timestamp>, # The current server timestamp for reference,
///   data: [<timestamp>, <value>] # The latest value and its timestamp.
/// }```
///
/// GET <endpoint>?count=2
/// ```{
///   data: [
///      <value1>, # The latest value.
///      <value2>  # Another value.
///   ]
/// }```
///
/// GET <endpoint>?children=3
/// ```{
///   data: [
///      [["a", "b"], <value1>],       # The latest value for /<path>/a/b.
///      [["a", "d", "e"], <value2>]   # The latest value for /<path>/a/d/e.
///   ]
/// }```
///
/// GET <endpoint>?after=<timestamp>
/// Only show entries after a specific timestamp (not including)
///
/// GET <endpoint>?before=<timestamp>
/// Only show entries before a specific timestamp (not including)
///
/// GET <endpoint>?include=/a/b,/a/d/e
/// Only show the path /a/b and /a/d/e
///
/// GET <endpoint>?keys=1&children=99
/// ```{
///   data: [
///      [{key: ["a", "b"], leaf: false}],       # This is an inner key
///      [{key: ["a", "b", "c"], leaf: true}],   # This is a leaf key that contain a value
///   ]
/// }```
///
export async function handleDataGet(
	data,
	{ uid, key, metadata, children, count, after, before, include, sampling, keys },
) {
	Exception.assertPrecondition(uid !== undefined, "'uid' must be set");

	keys = keys ?? false;

	// Sanitiy checks.
	if (keys) {
		Exception.assertPrecondition(metadata === undefined, "'metadata' cannot be set with 'keys'");
		Exception.assertPrecondition(count === undefined, "'count' cannot be set with 'keys'");
		Exception.assertPrecondition(after === undefined, "'after' cannot be set with 'keys'");
		Exception.assertPrecondition(before === undefined, "'before' cannot be set with 'keys'");
		Exception.assertPrecondition(include === undefined, "'include' cannot be set with 'keys'");
		Exception.assertPrecondition(sampling === undefined, "'sampling' cannot be set with 'keys'");
	}

	key = key ?? [];
	metadata = metadata ?? false;
	children = children ?? 99;
	count = count ?? null;
	after = after ?? null;
	before = before ?? null;
	include = include ?? null;
	sampling = sampling ?? null;

	let output = {};
	if (metadata) {
		output = Object.assign(output, {
			timestamp: timestampMs(),
		});
	}

	if (keys) {
		const maybeData = await data.getChildren({ uid, key, children, includeInner: true });
		if (!maybeData) {
			return null;
		}
		output = Object.assign(output, {
			data: maybeData,
		});
	} else {
		const maybeData = await data.get({
			uid,
			key,
			metadata,
			children,
			count,
			after,
			before,
			include,
			sampling,
		});
		if (maybeData.isEmpty()) {
			return null;
		}
		output = Object.assign(output, {
			data: maybeData.value(),
		});
	}

	return output;
}

export function getDataGetInputsFromQuery(context) {
	const metadata = context.getQuery("metadata", undefined, Boolean);
	const children = context.getQuery("children", undefined, parseInt);
	const count = context.getQuery("count", undefined, parseInt);
	const after = context.getQuery("after", undefined, parseInt);
	const before = context.getQuery("before", undefined, parseInt);
	const include = context.getQuery("include", undefined, (value) =>
		value
			.split(",")
			.filter(Boolean)
			.map((path) => pathToKey(path)),
	);
	const sampling = context.getQuery("sampling", undefined, String);
	const keys = context.getQuery("keys", undefined, Boolean);

	return {
		metadata,
		children,
		count,
		after,
		before,
		include,
		sampling,
		keys,
	};
}
