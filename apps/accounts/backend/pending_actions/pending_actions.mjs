import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";

const Log = LogFactory("pending-actions");
const Exception = ExceptionFactory("pending-actions");

export default class PendingActions {
	constructor(keyValueStore, actions) {
		this.keyValueStore = keyValueStore;
		this.actions = {};
		for (const type in actions) {
			this.actions[type] = Object.assign(
				{
					timeoutS: 24 * 3600,
					maxAttempts: 3,
					run: async (/*uid, data*/) => {},
				},
				actions[type],
			);
		}
	}

	_getTimestamp() {
		return Math.floor(Date.now() / 1000);
	}

	/// Create some pending actions that should be resolved within a given time
	/// using 2FA for example.
	///
	/// \param actionType The type of action previously registered.
	/// \param uid Unique identifier per action type to map this new action created (for example the user's email).
	/// \param data Data associated with this action.
	///
	/// \return The activation code.
	async create(actionType, uid, data) {
		Exception.assert(actionType in this.actions, "Undefined action type: {}", actionType);

		const code = Math.floor(Math.random() * 1000000);
		const action = this.actions[actionType];
		await this.keyValueStore.set(actionType, uid, {
			code: code,
			timeout: this._getTimestamp() + action.timeoutS,
			attempt: 0,
			data: data,
		});

		return code;
	}

	/// Helper for web handler
	async webHandler(context, actionType, uid, code) {
		const result = await this.run(actionType, uid, code);

		switch (result) {
			case "invalid":
				return context.sendStatus(401, "Invalid code");
			case "delete":
				return context.sendStatus(429, "Too many attempts");
		}
		return context.sendStatus(200, "Activated");
	}

	/// Attempt at running a pending action previously registered.
	async run(actionType, uid, code) {
		Exception.assert(actionType in this.actions, "Undefined action type: {}", actionType);

		const action = this.actions[actionType];
		const data = await this.keyValueStore.get(actionType, uid, /*defaultValue*/ null);

		// Wrong uid or already deleted
		if (data === null) {
			return "invalid";
		}

		// If timeout, remove
		if (this._getTimestamp() > data.timeout) {
			await this.delete(actionType, uid);
			return "invalid";
		}

		// Invalid code
		if (String(data.code) !== String(code)) {
			data.attempt++;
			if (data.attempt >= action.maxAttempts) {
				await this.delete(actionType, uid);
				return "delete";
			}
			await this.keyValueStore.set(actionType, uid, data);
			return "invalid";
		}

		// Valid, run the action
		await action.run(uid, data.data);
		await this.delete(actionType, uid);

		return 0;
	}

	/// Delete an action
	async delete(actionType, uid) {
		Exception.assert(actionType in this.actions, "Undefined action type: {}", actionType);

		await this.keyValueStore.delete(actionType, uid);
	}

	/// Return a dictionary with statistics
	async getStats() {
		let stats = {};
		for (const type in this.actions) {
			stats[type] = {
				count: await this.keyValueStore.count(type),
			};
		}
		return stats;
	}

	registerGarbageCollector(services, actionType, options) {
		Exception.assert(actionType in this.actions, "Undefined action type: {}", actionType);

		services.register(
			actionType + "-gc",
			() => {
				return CollectionPaging.makeIterator(async (maxOrPaging) => {
					return await this.keyValueStore.list(actionType, maxOrPaging);
				});
			},
			async (uid, data) => {
				if (this._getTimestamp() > data.timeout) {
					await this.delete(actionType, uid);
					return true;
				}
				return false;
			},
			options,
		);
	}

	installRest(api) {
		Log.info("Installing 'PendingActions' REST");

		api.handle("get", "/admin/pending_actions/stats", async () => {
			return await this.getStats();
		});
	}
}
