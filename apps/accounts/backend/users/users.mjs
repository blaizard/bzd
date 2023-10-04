import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import User from "#bzd/apps/accounts/backend/users/user.mjs";
import APISchema from "./api.json" assert { type: "json" };

const Exception = ExceptionFactory("users");
const Log = LogFactory("users");

export default class Users {
	constructor(keyValueStore, config) {
		this.keyValueStore = keyValueStore;
		this.config = Object.assign(
			{
				bucket: "users",
			},
			config,
		);
	}

	/// Create a new user.
	/// @param uid User ID.
	async create(uid) {
		Exception.assert(
			(await this.get(uid, /*allowNull*/ true)) == null,
			"A user with this UID '{}' already exists.",
			uid,
		);

		Log.info("{}: created.", uid);
		const user = User.create(uid);
		await this.keyValueStore.set(this.config.bucket, uid, user.data());

		return user;
	}

	/// Update user data.
	/// @param uid User ID.
	/// @param modifier The modifier callback that takes into argument the user to be modified.
	async update(uid, modifier) {
		let user = null;

		// Update the value atomically
		const isSuccess = await this.keyValueStore.update(
			this.config.bucket,
			uid,
			async (data) => {
				Exception.assert(data !== null, "User '{}' does not exists.", uid);
				user = await modifier(new User(uid, data));
				Log.info("{}: updated {}.", uid, user.getModifiedAsString());
				return user.data();
			},
			null,
		);

		if (!isSuccess) {
			Log.error("{}: updated {}.", uid, user.getModifiedAsString());
			return null;
		}

		return user;
	}

	/// Delete an existing user.
	/// @param uid User ID.
	async delete(uid) {
		await this.keyValueStore.delete(this.config.bucket, uid);
		Log.info("User with uid '{}' has been deleted.", uid);
	}

	/// Get user data.
	/// @param uid The user UID.
	/// @param allowNull If no match, it will return null. If unset, it will throw.
	async get(uid, allowNull = false) {
		const data = await this.keyValueStore.get(this.config.bucket, uid, null);
		if (data === null) {
			Exception.assert(allowNull, "User '{}' does not exists.", uid);
			return null;
		}
		return new User(uid, data);
	}

	_preprocessAndMerge(values, user) {
		Exception.assert(
			Object.keys(values).every((key) => key in ["password"]),
			"Some values cannot be changed: {}",
			Object.keys(values),
		);

		// Update password if any.
		if ("password" in values) {
			user.setPassword(values.password);
		}

		return user;
	}

	_preprocessAndMergePublic(values, user) {
		Exception.assert(
			Object.keys(values).every((key) => key in ["password", "oldpassword"]),
			"Some values cannot be changed: {}",
			Object.keys(values),
		);

		// Update password if any.
		if ("password" in values) {
			Exception.assert("oldpassword" in values, "Missing old password: '{:j}'", values);
			Exception.assert(user.getPassword() === values.oldpassword, "Old password is different");

			user.setPassword(values.password);
		}

		return user;
	}

	installAPI(api) {
		Log.info("Installing 'Users' API");

		const users = this;

		api.addSchema(APISchema);

		api.handle("get", "/user", async function (inputs, user) {
			const tempUser = await users.get(user.getUid());
			return tempUser.dataPublic();
		});

		api.handle("put", "/user", async (inputs, user) => {
			const uid = user.getUid();
			const tempUser = await this.update(uid, async (u) => {
				return this._preprocessAndMergePublic(inputs, u);
			});
			return tempUser.dataPublic();
		});

		api.handle("delete", "/user", async (inputs, user) => {
			await this.delete(user.getUid());
		});

		// ---- Admin specific API

		api.handle("get", "/admin/users", async (inputs) => {
			const result = await this.keyValueStore.list(this.config.bucket, inputs.paging);
			return {
				data: result.data(),
				next: result.getNextPaging(),
			};
		});

		api.handle("put", "/admin/user", async (inputs) => {
			await this.update(inputs.uid, async (u) => {
				return this._preprocessAndMerge(inputs, u);
			});
		});

		api.handle("delete", "/admin/user", async (inputs) => {
			await this.delete(inputs.uid);
		});
	}
}
