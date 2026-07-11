import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import ToStringHandler from "#bzd/nodejs/db/data/handlers/to_string.js";
import ExpiresHandler from "#bzd/nodejs/db/data/handlers/expires.js";
import HistoryHandler from "#bzd/nodejs/db/data/handlers/history.js";
import ValidationHandler from "#bzd/nodejs/db/data/handlers/validation.js";
import KeyMapping from "#bzd/nodejs/db/data/key_mapping.js";

const Exception = ExceptionFactory("db", "data", "handlers");
const Log = LogFactory("db", "data", "handlers");

const sortedAvailableHandlers = [
	["validation", ValidationHandler],
	["toString", ToStringHandler],
	["expires", ExpiresHandler],
	["history", HistoryHandler],
];
const availableHandlers = Object.fromEntries(sortedAvailableHandlers);

const SPECIAL_KEY_FOR_HANDLERS = "\x01";

class FragmentAccessor {
	constructor(fragments, index, root) {
		this.fragments = fragments;
		this.index = index;
		this.root = root;
	}

	/// Get the key of the fragment without the root prefix.
	get key() {
		return this.fragments[this.index][0].slice(this.root.length);
	}

	/// Get the value of the fragment.
	get value() {
		return this.fragments[this.index][1];
	}

	/// Get the options of the fragment.
	get options() {
		return this.fragments[this.index][2];
	}

	/// Check if the fragment has been removed.
	get isRemoved() {
		return this.index == this.fragments.length;
	}

	/// Remove the fragment from the fragments.
	remove() {
		this.index = this.fragments.length;
	}
}

class FragmentRange {
	constructor(fragments, index, root) {
		this.fragments = fragments;
		this.index = index;
		this.root = root;
	}

	/// Loop through all the fragments that are prefixed by the root and yield an accessor for each of them.
	*all() {
		for (let index = this.index; index < this.fragments.length; ++index) {
			if (!KeyMapping.keyStartsWith(this.fragments[index][0], this.root)) {
				break;
			}
			const accessor = new FragmentAccessor(this.fragments, index, this.root);
			yield accessor;
			if (accessor.isRemoved) {
				this.fragments.splice(index, 1);
				--index;
			}
		}
	}

	/// Add a new fragment with the root as prefix.
	add(key, value, options) {
		this.fragments.push([this.root.concat(key), value, options]);
		Handlers.sort(this.fragments);
	}
}

export default class Handlers {
	constructor(configuration, defaultOptions = {}) {
		// Convert the configuration:
		// {
		//	 "a/b/c": {
		//		 "handler1": options1,
		//		 "handler2": options2,
		//	 },
		// }
		// into:
		// {
		//    a: {
		//       b: {
		//          c: {
		//             SPECIAL_KEY_FOR_HANDLERS: {
		//                handlers: [ new handler1(options1), new handler2(options2) ],
		//                root: ["a", "b", "c"]
		//             }
		//          }
		//		 }
		//    }
		// }
		const result = {};
		for (const [path, props] of Object.entries(configuration)) {
			const key = KeyMapping.pathToKey(path);
			const object = key.reduce((r, segment) => {
				r[segment] ??= {};
				return r[segment];
			}, result);

			// Sort the handlers by their order in the sortedAvailableHandlers.
			const sortedProps = Object.entries(props).sort(
				([a], [b]) =>
					sortedAvailableHandlers.findIndex(([h]) => h === a) - sortedAvailableHandlers.findIndex(([h]) => h === b),
			);

			object[SPECIAL_KEY_FOR_HANDLERS] = {
				handlers: sortedProps.map(([handler, options]) => {
					Exception.assert(handler in availableHandlers, "The handler '{}' is not supported.", handler);
					return new availableHandlers[handler](options);
				}),
				root: key,
			};
		}
		this.configuration = result;
		this.defaultOptions = defaultOptions;
	}

	/// Sort in place the fragments by their keys.
	static sort(fragments) {
		return fragments.sort(([keyA], [keyB]) => {
			const a = KeyMapping.keyToInternal(keyA);
			const b = KeyMapping.keyToInternal(keyB);
			if (a < b) return -1;
			if (a > b) return 1;
			return 0;
		});
	}

	process(fragments) {
		// Sort and initialize the fragments with an empty options.
		fragments = Handlers.sort(fragments).map(([key, value]) => [key, value, Object.assign({}, this.defaultOptions)]);

		for (let index = 0; index < fragments.length; ++index) {
			const keys = [...fragments[index][0]];
			let current = this.configuration;
			let key;
			while ((key = keys.shift()) !== undefined) {
				if (!(key in current)) {
					break;
				}
				current = current[key];
				const { handlers, root } = current[SPECIAL_KEY_FOR_HANDLERS] ?? { handlers: [], root: [] };
				for (const handler of handlers) {
					const range = new FragmentRange(fragments, index, root);
					handler.process(range);
				}
			}
		}

		return fragments;
	}
}
