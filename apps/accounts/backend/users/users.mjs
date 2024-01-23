import Crypto from "crypto";

import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import User from "#bzd/apps/accounts/backend/users/user.mjs";
import { CollectionPaging } from "#bzd/nodejs/db/utils.mjs";

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

	/// Convert an email address into a uid.
	///
	/// \param email The email to be converted.
	_emailToUid(email) {
		return Crypto.createHash("shake256", { outputLength: 16 }).update(email).digest("hex");
	}

	/// Create a new user.
	///
	/// \param email Email address of the user.
	async create(email) {
		const uid = this._emailToUid(email);
		Exception.assertPrecondition(
			(await this.get(uid, /*allowNull*/ true)) == null,
			"A user with this UID '{}' already exists.",
			uid,
		);

		const user = User.create(uid, email);
		await this.keyValueStore.set(this.config.bucket, uid, user.data());
		Log.info("{}: created ({}).", uid, user.getEmail());

		return user;
	}

	/// Update user data.
	/// @param uid User ID.
	/// @param modifier The modifier callback that takes into argument the user to be modified.
	async update(uid, modifier, silent = false) {
		let user = null;

		// Update the value atomically
		const isSuccess = await this.keyValueStore.update(
			this.config.bucket,
			uid,
			async (data) => {
				Exception.assertPrecondition(data !== null, "User '{}' does not exists.", uid);
				user = await modifier(new User(uid, data));
				if (!silent) {
					Log.info("{}: updated {}.", uid, user.getModifiedAsString());
				}
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
	///
	/// \param uid User ID.
	async delete(uid) {
		await this.keyValueStore.delete(this.config.bucket, uid);
		Log.info("User with uid '{}' has been deleted.", uid);
	}

	/// Get user data from the email address.
	///
	/// \param email The user email.
	/// \param allowNull If no match, it will return null. If unset, it will throw.
	async getFromEmail(email, allowNull = false) {
		const uid = this._emailToUid(email);
		return await this.get(uid, allowNull);
	}

	/// Get user data.
	///
	/// \param uid The user UID.
	/// \param allowNull If no match, it will return null. If unset, it will throw.
	async get(uid, allowNull = false) {
		const data = await this.keyValueStore.get(this.config.bucket, uid, null);
		if (data === null) {
			Exception.assertPrecondition(allowNull, "User '{}' does not exists.", uid);
			return null;
		}
		return new User(uid, data);
	}

	_preprocessAndMergeAdmin(values, user) {
		Exception.assertPrecondition(
			Object.keys(values).every((key) => ["roles", "subscriptions"].includes(key)),
			"Some values cannot be changed by the admin: {}",
			Object.keys(values),
		);

		// Update roles.
		if ("roles" in values) {
			user.setRoles(values.roles);
		}

		// Update subscriptions.
		if ("subscriptions" in values) {
			user.setSubscriptions(values.subscriptions);
		}

		return user;
	}

	async _preprocessAndMergePublic(values, user) {
		Exception.assertPrecondition(
			Object.keys(values).every((key) => ["password", "oldpassword", "tokens"].includes(key)),
			"Some values cannot be changed: {}",
			Object.keys(values),
		);

		// Update password if any.
		if ("password" in values) {
			Exception.assertPrecondition("oldpassword" in values, "Missing old password: '{:j}'", values);
			Exception.assertPrecondition(user.getPassword() === values.oldpassword, "Old password is different");

			await user.setPassword(values.password);
		}

		// Update tokens if any.
		if ("tokens" in values) {
			user.setTokens(values.tokens);
		}

		return user;
	}

	async installAPI(api) {
		Log.info("Installing 'Users' API");

		const users = this;

		api.handle("get", "/user", async function (inputs, user) {
			const uid = inputs.uid ? inputs.uid : user.getUid();
			const tempUser = await users.get(uid);
			return tempUser.dataPublic();
		});

		api.handle("put", "/user", async (inputs, user) => {
			const uid = inputs.uid ? inputs.uid : user.getUid();
			delete inputs.uid;
			const tempUser = await this.update(uid, async (u) => {
				return await this._preprocessAndMergePublic(inputs, u);
			});
			return tempUser.dataPublic();
		});

		api.handle("delete", "/user", async (inputs, user) => {
			await this.delete(user.getUid());
		});

		// ---- Admin specific API

		api.handle("get", "/admin/users", async (inputs) => {
			const result = await this.keyValueStore.list(
				this.config.bucket,
				CollectionPaging.pagingFromParam(inputs.max, inputs.page),
			);
			return {
				data: result.data(),
				next: result.getNextPaging(),
			};
		});

		api.handle("post", "/admin/user", async (inputs) => {
			const email = inputs.email;
			const user = await this.create(email);
			await this.update(user.getUid(), async (u) => {
				u.addRole("user");
				return u;
			});
		});

		api.handle("put", "/admin/user", async (inputs) => {
			const uid = inputs.uid;
			delete inputs.uid;
			await this.update(uid, async (u) => {
				return this._preprocessAndMergeAdmin(inputs, u);
			});
		});

		api.handle("delete", "/admin/user", async (inputs) => {
			await this.delete(inputs.uid);
		});
	}
}
