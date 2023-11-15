import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Application from "#bzd/apps/accounts/backend/applications/application.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";

const Exception = ExceptionFactory("applications");
const Log = LogFactory("applications");

export default class Applications {
	constructor(keyValueStore, config) {
		this.keyValueStore = keyValueStore;
		this.config = Object.assign(
			{
				bucket: "applications",
			},
			config,
		);
	}

	async installAPI(api) {
		Log.info("Installing 'Applications' API");

		/*
		const users = this;

		api.handle("get", "/user", async function (inputs, user) {
			const tempUser = await users.get(user.getUid());
			return tempUser.dataPublic();
		});
        */
	}
}
