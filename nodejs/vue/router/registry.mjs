import ExceptionFactory from "../../core/exception.mjs";

const Exception = ExceptionFactory("router", "registry");

/// Living collection of all router component currently available on the DOM.
export default class Registry {
	constructor() {
		this.registry = {};
		this.wait = {};
	}

	/// This reset the state of the component registry by clearing waiting actions.
	reset() {
		for (const callbacks of Object.values(this.wait)) {
			for (const [resolve, reject] of callbacks) {
				reject();
			}
		}
		this.wait = {};
	}

	register(uid, setComponent) {
		Exception.assert(!(uid in this.register), "The RouterComponent with name '{}' has been registered twice.", uid);
		this.registry[uid] = {
			setComponent,
		};
		this.resolve(uid);
	}

	unregister(uid) {
		delete this.registry[uid];
	}

	resolve(uid) {
		const callbacks = this.wait[uid] || [];
		delete this.wait[uid];
		for (const [resolve, reject] of callbacks) {
			resolve();
		}
	}

	/// Wait until a specific UID is present or return immediately if available.
	async waitFor(uid) {
		return new Promise((resolve, reject) => {
			if (uid in this.registry) {
				resolve();
			} else {
				this.wait[uid] = this.wait[uid] || [];
				this.wait[uid].push([resolve, reject]);
			}
		});
	}

	async setComponent(uid, component, props) {
		await this.waitFor(uid);
		this.registry[uid].setComponent(component, props);
	}
}
