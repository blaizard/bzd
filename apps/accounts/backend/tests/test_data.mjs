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
			user.setPassword("1234");
			user.addRole("admin");
			user.addRole("user");
			return user;
		});

		Log.info("Creating 5 dummy users");
		for (let i = 0; i < 5; ++i) {
			const uid = "dummy-" + i + "@dummy.com";
			await this.users.create(uid);
			await this.users.update(uid, (user) => {
				user.setPassword("1234");
				user.addRole("user");
				return user;
			});
		}
	}
}
