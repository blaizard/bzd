import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import toString from "#bzd/apps/artifacts/plugins/nodes/handlers/to_string.mjs";
import history from "#bzd/apps/artifacts/plugins/nodes/handlers/history.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes", "handlers");
const Log = LogFactory("apps", "plugin", "nodes", "handlers");

const availableHandlers = {
	toString: toString,
	history: history,
};

// Gather all the fragments that need to be processed by a handler group.
const availableHandlersWithGroup = Object.entries(availableHandlers)
	.filter(([_, data]) => "group" in data)
	.map(([name, _]) => name);

export default class Handlers {
	constructor(configuration) {
		this.configuration = Object.entries(configuration).map(([root, handlers]) => {
			return [KeyMapping.pathToKey(root), handlers];
		});
	}

	/// Get the handlers associated with an internal path.
	*getHandlers(key) {
		for (const [root, handlers] of this.configuration) {
			if (KeyMapping.keyStartsWith(key, root)) {
				for (const [handler, options] of Object.entries(handlers)) {
					yield [handler, options];
				}
			}
		}
	}

	/// Create groups by handlers.
	///
	/// Each group of fragment created will be removed from the fragments and returned separately with the following format,
	/// example using [["a", "b", "c", "d"], 12];
	/// {
	///    [handler]: [
	///        [["a", "b", "c", "d"], 12, options],
	///    ]
	/// }
	groupByHandler(fragments, handlers) {
		let groups = Object.fromEntries([...handlers].map((key) => [key, {}]));
		const fragmentsRest = fragments.filter(([key, value]) => {
			let keepEntry = true;
			for (const [handler, options] of this.getHandlers(key)) {
				if (handler in groups) {
					Exception.assert(keepEntry, "The path {} has been processed by 2 handlers.", KeyMapping.keyToPath(key));
					const name = key.pop();
					const parent = KeyMapping.keyToInternal(key);
					groups[handler][parent] ??= {
						key: key,
						options: options,
						data: {},
					};
					groups[handler][parent]["data"][name] = value;
					keepEntry = false;
				}
			}
			return keepEntry;
		});
		return [fragmentsRest, groups];
	}

	/// Process the fragments before being inserted.
	processBeforeInsert(fragments) {
		const [fragmentsRest, groups] = this.groupByHandler(fragments, availableHandlersWithGroup);

		// Process the group handlers.
		let fragmentsGroup = [];
		for (const [handlerName, groupByPath] of Object.entries(groups)) {
			const groupOptions = availableHandlers[handlerName].group;
			for (const data of Object.values(groupByPath)) {
				try {
					if ("format" in groupOptions) {
						fragmentsGroup.push([data.key, groupOptions.format(data["data"], data["options"])]);
					} else {
						Exception.unreachable("Missing group handler.");
					}
				} catch (e) {
					Log.error("Error while handling path {}: {}", KeyMapping.internalToPath(internal), e.toString());
					fragmentsGroup.push([data.key, data["data"]]);
				}
			}
		}

		return [...fragmentsRest, ...fragmentsGroup];
	}

	/// Process a handler.
	///
	/// \param The name of the handler.
	/// \param The internal path.
	process(name, internal, ...args) {
		Exception.assert(name in availableHandlers, "The handler '{}' is not registered.", name);
		const options = [...this.getHandlers(internal)]
			.filter(([handler, _]) => handler == name)
			.map(([_, options]) => options);
		return availableHandlers[name].process(options, ...args);
	}
}
