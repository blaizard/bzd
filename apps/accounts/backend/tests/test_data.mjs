import LogFactory from "#bzd/nodejs/core/log.mjs";

const Log = LogFactory("test", "data");

export default class TestData {
	constructor(users) {
		this.users = users;
	}

	async install() {
		Log.info("Installing test data...");

		Log.info("Creating admin user 'admin@admin.com'");
		await this.users.create("admin@admin.com");
		await this.users.update("admin@admin.com", (user) => {
			user.addRole("admin");
			return user;
		});
	}
}
