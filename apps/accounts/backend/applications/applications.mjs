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

	/// Create a new application.
	///
	/// \param uid UID associated with this entry.
	/// \param redirect The redirect URL for this application.
	///
	/// \return The application.
	async create(uid, redirect, scopes) {
		Exception.assert(
			(await this.get(uid, /*allowNull*/ true)) == null,
			"An application with this UID '{}' already exists.",
			uid,
		);

		const application = Application.create(uid, redirect, scopes);
		await this.keyValueStore.set(this.config.bucket, uid, application.data());
		Log.info("{}: created.", uid);

		return application;
	}

	/// Get application data.
	///
	/// \param uid The application UID.
	/// \param allowNull If no match, it will return null. If unset, it will throw.
	///
	/// \return The application.
	async get(uid, allowNull = false) {
		const data = await this.keyValueStore.get(this.config.bucket, uid, null);
		if (data === null) {
			Exception.assert(allowNull, "Application '{}' does not exists.", uid);
			return null;
		}
		return new Application(uid, data);
	}

	/// List all available applications UID.
	async list() {
		let uids = [];
		for await (const [uid, _] of CollectionPaging.makeIterator(async (maxOrPaging) => {
			return await this.keyValueStore.list(this.config.bucket, maxOrPaging);
		}, 50)) {
			uids.push(uid);
		}
		return uids;
	}

	/// Delete an existing application.
	///
	/// \param uid User ID.
	async delete(uid) {
		await this.keyValueStore.delete(this.config.bucket, uid);
		Log.info("Application with uid '{}' has been deleted.", uid);
	}

	installRest(api) {
		Log.info("Installing 'Applications' REST");

		// ---- Admin specific API

		api.handle("get", "/admin/applications", async (inputs) => {
			const result = await this.keyValueStore.list(
				this.config.bucket,
				CollectionPaging.pagingFromParam(inputs.max, inputs.page),
			);
			return {
				data: result.data(),
				next: result.getNextPaging(),
			};
		});

		api.handle("post", "/admin/application", async (inputs) => {
			const application = await this.create(inputs.uid, inputs.redirect, inputs.scopes);
			return application.dataPublic();
		});

		api.handle("delete", "/admin/application", async (inputs) => {
			await this.delete(inputs.uid);
		});
	}
}
