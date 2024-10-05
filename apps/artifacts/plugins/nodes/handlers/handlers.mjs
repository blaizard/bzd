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
	constructor(handlers) {
		this.handlers = handlers;
	}

	/// Get the handlers associated with an internal path.
	*getHandlers(internal) {
		for (const [root, handlers] of Object.entries(this.handlers)) {
			if (KeyMapping.internalStartsWithKey(internal, root.split("/").filter(Boolean))) {
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
							KeyMapping.internalToKey(internal).join("/"),
						);
						const path = KeyMapping.internalToKey(internal);
						const name = path.pop();
						const parent = KeyMapping.keyToInternal(...path);
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
					Log.error("Error while handling path {}: {}", KeyMapping.internalToKey(internal).join("/"), e.toString());
					fragmentsGroup.push({ [internal]: data["data"] });
				}
			}
		}
		return Object.assign(fragmentsRest, ...fragmentsGroup);
	}
}
