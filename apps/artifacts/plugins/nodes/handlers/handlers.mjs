import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import toString from "#bzd/apps/artifacts/plugins/nodes/handlers/to_string.mjs";
import KeyMapping from "#bzd/apps/artifacts/plugins/nodes/key_mapping.mjs";

const Exception = ExceptionFactory("apps", "plugin", "nodes", "handlers");
const Log = LogFactory("apps", "plugin", "nodes", "handlers");

const availableHandlers = {
	toString: toString,
};

// Gather all the fragments that need to be processed by a handler group.
const availableHandlersWithGroup = Object.entries(availableHandlers)
	.filter(([_, data]) => "group" in data)
	.map(([name, _]) => name);

export default class Handlers {
	constructor(configuration) {
		this.configuration = Object.fromEntries(
			Object.entries(configuration).map(([root, handlers]) => {
				return [KeyMapping.pathToInternal(root), handlers];
			}),
		);
	}

	/// Get the handlers associated with an internal path.
	*getHandlers(internal) {
		for (const [root, handlers] of Object.entries(this.configuration)) {
			if (KeyMapping.internalStartsWith(internal, root)) {
				for (const [handler, options] of Object.entries(handlers)) {
					yield [handler, options];
				}
			}
		}
	}

	/// Create groups by handlers.
	///
	/// Each group of fragment created will be removed from the fragments and returned separately with the following format,
	/// example using a.b.c.d = 12;
	/// {
	///    [handler]: {
	///        a.b.c: {
	///           options: [handlerOptions],
	///           data: { d: 12 },
	///        }
	///    }
	/// }
	groupByHandler(fragments, handlers) {
		let groups = Object.fromEntries([...handlers].map((key) => [key, {}]));
		const fragmentsRest = Object.fromEntries(
			Object.entries(fragments).filter(([internal, value]) => {
				let keepEntry = true;
				for (const [handler, options] of this.getHandlers(internal)) {
					if (handler in groups) {
						Exception.assert(
							keepEntry,
							"The path {} has been processed by 2 handlers.",
							KeyMapping.internalToPath(internal),
						);
						const path = KeyMapping.internalToKey(internal);
						const name = path.pop();
						const parent = KeyMapping.keyToInternal(path);
						groups[handler][parent] ??= {
							options: options,
							data: {},
						};
						groups[handler][parent]["data"][name] = value;
						keepEntry = false;
					}
				}
				return keepEntry;
			}),
		);
		return [fragmentsRest, groups];
	}

	/// Process the fragments before being inserted.
	processBeforeInsert(fragments) {
		const [fragmentsRest, groups] = this.groupByHandler(fragments, availableHandlersWithGroup);

		// Process the group handlers.
		let fragmentsGroup = [];
		for (const [handlerName, groupByPath] of Object.entries(groups)) {
			const groupOptions = availableHandlers[handlerName].group;
			for (const [internal, data] of Object.entries(groupByPath)) {
				try {
					if ("format" in groupOptions) {
						fragmentsGroup.push({ [internal]: groupOptions.format(data["data"], data["options"]) });
					} else {
						Exception.unreachable("Missing group handler.");
					}
				} catch (e) {
					Log.error("Error while handling path {}: {}", KeyMapping.internalToPath(internal), e.toString());
					fragmentsGroup.push({ [internal]: data["data"] });
				}
			}
		}
		return Object.assign(fragmentsRest, ...fragmentsGroup);
	}
}
