import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import {
	scopeSelfBasicRead,
	scopeSelfSubscriptionsRead,
	scopeAdminApplications,
} from "#bzd/apps/accounts/backend/users/scopes.mjs";
import { PaymentRecurrency } from "#bzd/nodejs/payment/payment.mjs";

const Log = LogFactory("test", "data");
const Exception = ExceptionFactory("test", "data");

export default class TestData {
	constructor(users, applications, payment) {
		this.users = users;
		this.applications = applications;
		this.payment = payment;
		this.adminEmail = "admin@admin.com";
	}

	async install() {
		Log.info("Installing test data...");

		const userAdmin = await this.users.create(this.adminEmail);
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
		await this.applications.create("artifacts", "http://localhost:8081/", [scopeSelfBasicRead, scopeAdminApplications]);
		await this.applications.create("screen-recorder", "http://localhost:8081/", [scopeSelfSubscriptionsRead]);
	}

	async run() {
		Exception.assert(
			await this.payment.triggerPayment("10001", this.adminEmail, [
				{ uid: 1, application: "screen-recorder", duration: 7 * 24 * 3600 },
			]),
		);
		Exception.assert(
			!(await this.payment.triggerPayment("10001", this.adminEmail, [
				{ uid: 1, application: "screen-recorder", duration: 24 * 3600 },
			])),
			"2 payments with the same id should not be processed.",
		);
		Exception.assertThrows(
			async () =>
				await this.payment.triggerPayment("10002", this.adminEmail, [
					{ uid: 2, application: "unknown", duration: 24 * 3600 },
				]),
			"payment with unknown application should fail.",
		);
		Exception.assert(
			await this.payment.triggerPayment(
				"10002",
				this.adminEmail,
				[{ uid: 1, application: "screen-recorder", duration: 7 * 24 * 3600 }],
				new PaymentRecurrency("abc", Date.now() + 3600 * 1000),
			),
			"Recurring payment",
		);
		Exception.assert(
			await this.payment.triggerPayment("10001", "dummy-1@dummy.com", [
				{ uid: 1, application: "screen-recorder", duration: 7 * 24 * 3600 },
			]),
		);
	}
}
