import StorageBzd from "#bzd/apps/artifacts/plugins/bzd/storage.mjs";
import Nodes from "#bzd/apps/artifacts/plugins/bzd/nodes.mjs";

/// Data structure of bzd nodes is as follow:
/// The following is key-ed by UID:
/// 	pending: [] // an ordered list of pending actions.
/// 	status: {} // a dictionary of well defined statuses (health status) of the node.
///			- cpus
///			- ram
///			- version (SW, HW, ...)
///			- ...
/// 	records: {} // a dictionary of record from the target (those are the internal symbols of the application, it can be anything)
///
/// Note: all data should come from the node, no extra data should be available to ensure a single source of truth.
///		For example no config on the health status check, the node should know it.
///
/// Questions:
/// - What to do with firmware and firmware metadata?
export default {
	services: {
		nodes: {
			async is(params) {
				return true;
			},
			async start(params, context) {
				const nodes = new Nodes(params["bzd.path"]);
				return nodes;
			},
			async stop(server) {
			},
		},
	},
	endpoints: {
		"/{uid}/{category}": {
			"get": {
				async handler(inputs, services) {
					const node = await services.nodes.get(inputs.uid);
					return await node.get(inputs.category);
				}
			},
			"post": {
				async handler(inputs, services) {
					console.log(this.getBody());
					console.log(this.request.body);
				}
			},
		}
	},
	async storage(params, services) {
		return await StorageBzd.make(services.nodes);
	}
};
