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
export async function handleDataGet(context, data, uid, key) {
	const metadata = context.getQuery("metadata", false, Boolean);
	const children = context.getQuery("children", 0, parseInt);
	const count = context.getQuery("count", null, parseInt);
	const after = context.getQuery("after", null, parseInt);
	const before = context.getQuery("before", null, parseInt);
	const include = context.getQuery("include", null, (value) =>
		value
			.split(",")
			.filter(Boolean)
			.map((path) => pathToKey(path)),
	);
	const sampling = context.getQuery("sampling", null, String);
	const keys = context.getQuery("keys", false, Boolean);

	let output = {};
	if (metadata) {
		output = Object.assign(output, {
			timestamp: timestampMs(),
		});
	}

	if (keys) {
		Exception.assertPrecondition(context.getQuery("metadata", null) === null, "'metadata' cannot be set with 'keys'");
		Exception.assertPrecondition(context.getQuery("count", null) === null, "'count' cannot be set with 'keys'");
		Exception.assertPrecondition(context.getQuery("after", null) === null, "'after' cannot be set with 'keys'");
		Exception.assertPrecondition(context.getQuery("before", null) === null, "'before' cannot be set with 'keys'");
		Exception.assertPrecondition(context.getQuery("include", null) === null, "'include' cannot be set with 'keys'");
		Exception.assertPrecondition(context.getQuery("sampling", null) === null, "'sampling' cannot be set with 'keys'");

		const maybeData = await data.getChildren({ uid, key, children, includeInner: true });
		if (!maybeData) {
			context.sendStatus(404);
			return;
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
			context.sendStatus(404);
			return;
		}
		output = Object.assign(output, {
			data: maybeData.value(),
		});
	}

	context.setStatus(200);
	context.sendJson(output);
}
