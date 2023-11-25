import LogFactory from "#bzd/nodejs/core/log.mjs";

const Log = LogFactory("test", "data");

export default class TestData {
	constructor(users, applications) {
		this.users = users;
		this.applications = applications;
	}

	async install() {
		Log.info("Installing test data...");

		const userAdmin = await this.users.create("admin@admin.com");
		await this.users.update(userAdmin.getUid(), (user) => {
			user.setPassword("1234");
			user.addRole("admin");
			user.addRole("user");
			return user;
		});

		for (let i = 0; i < 5; ++i) {
			const email = "dummy-" + i + "@dummy.com";
			const userDummy = await this.users.create(email);
			await this.users.update(userDummy.getUid(), (user) => {
				user.setPassword("1234");
				user.addRole("user");
				return user;
			});
		}

		await this.applications.create("localhost", "http://localhost:8081/redirect");
	}
}
