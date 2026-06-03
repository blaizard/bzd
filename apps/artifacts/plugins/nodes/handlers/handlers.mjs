import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import ToStringHandler from "#bzd/apps/artifacts/plugins/nodes/handlers/to_string.mjs";
import HistoryHandler from "#bzd/apps/artifacts/plugins/nodes/handlers/history.mjs";
import ValidationHandler from "#bzd/apps/artifacts/plugins/nodes/handlers/validation.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes", "handlers");
const Log = LogFactory("apps", "plugin", "nodes", "handlers");

const sortedAvailableHandlers = [
	["validation", ValidationHandler],
	["toString", ToStringHandler],
	["history", HistoryHandler],
];
const availableHandlers = Object.fromEntries(sortedAvailableHandlers);

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
		// [
		//   [["a", "b", "c"], new handler1(options1)],
		//   [["a", "b", "c"], new handler2(options2)],
		// ]
		const result = [];
		for (const [path, props] of Object.entries(configuration)) {
			const key = KeyMapping.pathToKey(path);

			// Sort the handlers by their order in the sortedAvailableHandlers.
			const sortedProps = Object.entries(props).sort(
				([a], [b]) =>
					sortedAvailableHandlers.findIndex(([h]) => h === a) - sortedAvailableHandlers.findIndex(([h]) => h === b),
			);
			for (const [handler, options] of sortedProps) {
				Exception.assert(handler in availableHandlers, "The handler '{}' is not supported.", handler);
				const handlerInstance = new availableHandlers[handler](options);
				result.push([key, handlerInstance]);
			}
		}
		this.configuration = Handlers.sort(result);
		this.defaultOptions = defaultOptions;
	}

	/// Sort in place the fragments by their keys.
	static sort(fragments) {
		return fragments.sort((a, b) => Handlers.compare(a[0], b[0]));
	}

	/// Compare two keys.
	///
	/// \param keyA The first key.
	/// \param keyB The second key.
	/// \returns -1 if keyA < keyB, 1 if keyA > keyB, 0 otherwise.
	static compare(keyA, keyB) {
		const a = KeyMapping.keyToInternal(keyA);
		const b = KeyMapping.keyToInternal(keyB);
		if (a < b) return -1;
		if (a > b) return 1;
		return 0;
	}

	process(fragments) {
		// Sort and initialize the fragments with an empty options.
		fragments = Handlers.sort(fragments).map(([key, value]) => [key, value, Object.assign({}, this.defaultOptions)]);

		let indexConfig = 0;
		let indexFragment = 0;
		while (indexConfig < this.configuration.length && indexFragment < fragments.length) {
			const [root, handler] = this.configuration[indexConfig];
			const [key] = fragments[indexFragment];
			// If the root is a prefix of the key, we can process the handler.
			if (KeyMapping.keyStartsWith(key, root)) {
				const range = new FragmentRange(fragments, indexFragment, root);
				handler.process(range);
				++indexConfig;
			}
			// If the root comes before the key, we can move to the next configuration.
			else if (Handlers.compare(root, key) < 0) {
				indexConfig++;
			}
			// Else the root comes after the key, we can move to the next fragment.
			else {
				indexFragment++;
			}
		}

		return fragments;
	}
}
