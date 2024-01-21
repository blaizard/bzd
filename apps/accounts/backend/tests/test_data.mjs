import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { scopeSelfBasicRead } from "#bzd/apps/accounts/backend/users/scopes.mjs";

const Log = LogFactory("test", "data");
const Exception = ExceptionFactory("test", "data");

export default class TestData {
	constructor(users, applications, payment) {
		this.users = users;
		this.applications = applications;
		this.payment = payment;
	}

	async install() {
		Log.info("Installing test data...");

		const userAdmin = await this.users.create("admin@admin.com");
		await this.users.update(userAdmin.getUid(), async (user) => {
			await user.setPassword("1234");
			user.addRole("admin");
			user.addRole("user");
			return user;
		});

		for (let i = 0; i < 5; ++i) {
			const email = "dummy-" + i + "@dummy.com";
			const userDummy = await this.users.create(email);
			await this.users.update(userDummy.getUid(), async (user) => {
				await user.setPassword("1234");
				user.addRole("user");
				return user;
			});
		}

		await this.applications.create("localhost", "http://localhost:8081/", [scopeSelfBasicRead]);
		await this.applications.create("artifacts", "http://localhost:8081/", [scopeSelfBasicRead]);
		await this.applications.create("screen_recorder", "http://localhost:8081/", [scopeSelfBasicRead]);
	}

	async run() {
		Exception.assert(
			await this.payment.triggerPayment("10001", "admin@admin.com", [{ uid: 1, application: "localhost", period: 1 }]),
		);
		Exception.assert(
			await this.payment.triggerPayment("10001", "admin@admin.com", [{ uid: 1, application: "localhost", period: 1 }]),
			"2 payments with the same id should not be processed.",
		);
		Exception.assertThrows(
			async () =>
				await this.payment.triggerPayment("10001", "admin@admin.com", [{ uid: 2, application: "unknown", period: 1 }]),
			"payment with unknown application should fail.",
		);
	}
}
