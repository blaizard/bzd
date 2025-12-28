import { Command } from "commander";

import Backend from "#bzd/nodejs/vue/apps/backend.mjs";
import APIv1 from "#bzd/api.json" with { type: "json" };
import kvsMakeFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";
import emailMakeFromConfig from "#bzd/nodejs/email/make_from_config.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Authentication from "#bzd/nodejs/core/authentication/session/server.mjs";
import AuthenticationGoogle from "#bzd/nodejs/core/authentication/google/server.mjs";
import AuthenticationFacebook from "#bzd/nodejs/core/authentication/facebook/server.mjs";
import Result from "#bzd/nodejs/utils/result.mjs";
import HttpClient from "#bzd/nodejs/core/http/client.mjs";
import Users from "#bzd/apps/accounts/backend/users/users.mjs";
import Applications from "#bzd/apps/accounts/backend/applications/applications.mjs";
import TokenInfo from "#bzd/apps/accounts/backend/users/token.mjs";
import TestData from "#bzd/apps/accounts/backend/test_data/test_data.mjs";
import config from "#bzd/apps/accounts/config.json" with { type: "json" };
import configBackend from "#bzd/apps/accounts/backend/config.json" with { type: "json" };
import paymentMakeFromConfig from "#bzd/nodejs/payment/make_from_config.mjs";
import EmailManager from "#bzd/apps/accounts/backend/email/manager.mjs";
import Subscription from "#bzd/apps/accounts/backend/users/subscription.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import { delayMs } from "#bzd/nodejs/utils/delay.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

(async () => {
	// ---- Additional options to be used ----

	const program = new Command();
	program
		.option("--dump <path>", "Dump the database to a specific path.")
		.option("--test-data", "Include test data and run some self tests.");
	const backend = Backend.makeFromCli(process.argv, program);
	const options = program.opts();

	// ---- Headers to be used ----

	const headers = {
		// Needed for Google authentication.
		"Cross-Origin-Opener-Policy": "same-origin-allow-popups",
	};
	if (program.opts().testData) {
		// This allow http and https interoperability.
		headers["Referrer-Policy"] = "no-referrer-when-downgrade";
	}
	backend
		.useStaticContentOptions({
			headers: headers,
		})
		.useServices();

	const keyValueStore = await kvsMakeFromConfig(configBackend.kvs.accounts);
	const email = await emailMakeFromConfig(configBackend.email);

	// Set-up the mail object
	const emails = new EmailManager(email);

	// Users
	const users = new Users(keyValueStore);

	// Applications
	const applications = new Applications(keyValueStore);

	// ---- Helpers ----

	/// Generate a reset password like for a specific user.
	async function createResetPasswordLink(user, newPassword = false) {
		// Set a password if there is none and timestamp the operation.
		user = await users.update(
			user.getUid(),
			async (user) => {
				user.setLastPasswordReset();
				if (!user.getPassword()) {
					await user.setRandomPassword();
				}
				return user;
			},
			/*silent*/ true,
		);

		const password = user.getPassword();
		Exception.assert(password, "The user must have a valid password.");

		return (
			config.url +
			"/" +
			(newPassword ? "new" : "reset") +
			"/" +
			encodeURIComponent(user.getUid()) +
			"/" +
			encodeURIComponent(password)
		);
	}

	// ---- Authentication ----

	let authentication = new Authentication({
		verifyIdentity: async (email, password = null) => {
			const maybeUser = await users.getFromEmail(email, /*allowNull*/ true);
			if (maybeUser === null) {
				return Result.makeError(Authentication.ErrorVerifyIdentity.unauthorized);
			}
			// Limit the number of login attempt against brute forcing for an interval of 2s.
			if (maybeUser.getLastFailedLoginTimestamp() + 2000 > Date.now()) {
				return Result.makeError(Authentication.ErrorVerifyIdentity.tooManyAttempts);
			}
			// Check the password if provided.
			if (password !== null) {
				const isEqual = await maybeUser.isPasswordEqual(password);
				if (!isEqual) {
					await users.update(
						maybeUser.getUid(),
						(user) => {
							user.setLastFailedLogin();
							return user;
						},
						/*silent*/ true,
					);
					return Result.makeError(Authentication.ErrorVerifyIdentity.unauthorized);
				}
			}
			await users.update(
				maybeUser.getUid(),
				(user) => {
					user.setLastLogin();
					return user;
				},
				/*silent*/ true,
			);
			return new Result({
				scopes: maybeUser.getScopes().toList(),
				uid: maybeUser.getUid(),
			});
		},
		saveRefreshToken: async (session, hash, timeoutS, identifier, rolling) => {
			await users.update(session.getUid(), (user) => {
				const token = TokenInfo.make(identifier, session.getScopes().toList(), timeoutS, rolling);
				user.addToken(hash, token);
				user.sanitizedTokens();
				return user;
			});
		},
		removeRefreshToken: async (uid, hash) => {
			await users.update(uid, (user) => {
				user.removeToken(hash);
				return user;
			});
		},
		refreshToken: async (uid, hash, minDuration, hashNext) => {
			// If there is no user
			const maybeUser = await users.get(uid, /*allowNull*/ true);
			if (maybeUser === null) {
				return false;
			}
			// If there is no token
			const maybeToken = maybeUser.getToken(hash, null);
			if (!maybeToken) {
				return false;
			}
			// If the token is expired
			if (maybeToken.isExpired()) {
				return false;
			}

			let result = {
				scopes: maybeToken.getScopes().toList(),
				uid: uid,
			};

			// Rolling token
			if (maybeToken.isRolling()) {
				let updatedToken = null;
				await users.update(uid, (user) => {
					updatedToken = user.rollToken(hash, hashNext);
					updatedToken.updateMinDuration(minDuration);
					return user;
				});
				result.hash = hashNext;
				result.timeout = updatedToken.duration();
			}

			return result;
		},
	});

	const authenticationGoogle = new AuthenticationGoogle(
		config.googleClientId,
		configBackend.googleClientSecret,
		config.url,
	);
	const authenticationFacebook = new AuthenticationFacebook();

	// ---- Payment ----

	/// All products must define in Stripe the following metadata:
	/// - application: "screen_recorder"
	/// - duration: <seconds>
	const payment = await paymentMakeFromConfig(
		async (uid, email, products, maybeRecurrency = null) => {
			// Check if email account exists, if not create one.
			let maybeUser = await users.getFromEmail(email, /*allowNull*/ true);
			if (maybeUser === null) {
				maybeUser = await users.create(email);
				maybeUser = await users.update(maybeUser.getUid(), async (user) => {
					user.addRole("user");
					return user;
				});

				const link = await createResetPasswordLink(maybeUser, /*newPassword*/ true);

				Log.info("Welcome email sent to: {}.", email);
				await emails.sendWelcome(email, {
					email: email,
					support: configBackend.supportURL,
					link: link,
				});
			}
			Exception.assert(maybeUser, "The user is not defined.");

			// Check if the payment uid is already processed, if so ignore the rest.
			if (maybeUser.hasPayment(uid)) {
				return false;
			}

			// Check the products, make sure they are valid and retrieve the subscription time.
			let subscriptions = [];
			for (const product of products) {
				// Make sure this application exists.
				await applications.get(product.application, /*allowNull*/ false);
				let subscription = null;
				if (maybeRecurrency && maybeRecurrency.timestampEndMs) {
					subscription = Subscription.makeFromTimestamp(maybeRecurrency.timestampEndMs);
					subscription.addRecurringSubscription(maybeRecurrency.uid, maybeRecurrency.timestampEndMs);
				} else {
					const duration = parseInt(product.duration);
					Exception.assert(duration > 0, "The product must embed a duration value in seconds: '{}'.", product.duration);
					subscription = Subscription.makeFromDuration(duration * 1000);
				}

				subscriptions.push({
					application: product.application,
					subscription: subscription,
				});
			}

			// Register the payment to ensure it will not be processed again.
			maybeUser = await users.update(maybeUser.getUid(), async (user) => {
				// Update the subscription of the application(s) and make sure it is started.
				for (const subscription of subscriptions) {
					user.addSubscription(subscription.application, subscription.subscription);
					user.getSubscription(subscription.application).start();
				}

				// Register the payment.
				user.registerPayment(uid);

				return user;
			});

			return true;
		},
		async (uid, email) => {
			let maybeUser = await users.getFromEmail(email, /*allowNull*/ true);
			if (maybeUser === null) {
				Log.warning(
					"A notification for deletion of recurrent payment '{}' for user '{}' was made, but the user does not exists.",
					uid,
					email,
				);
				return true;
			}
			await users.update(maybeUser.getUid(), async (user) => {
				user.stopRecurringSubscription(uid);
				return user;
			});
			return true;
		},
		configBackend.payment,
	);

	// ---- REST ----

	backend
		.useRest(APIv1.rest, {
			authentication: authentication,
		})
		.useLoggerMemory()
		.setup();

	backend.rest.installPlugins(
		authentication,
		authenticationGoogle,
		authenticationFacebook,
		users,
		applications,
		payment,
	);

	backend.rest.handle("get", "/sso", async function (inputs, session) {
		// Get that the application exists.
		const application = await applications.get(inputs.application, /*allowNull*/ true);
		Exception.assertPrecondition(application, "Application '{}' does not exists.", inputs.application);

		// Get the SSO token.
		const token = await authentication.makeSSOToken(inputs.application, session, application.getScopes().toList());

		return {
			token: token,
			redirect: application.getRedirect(),
		};
	});

	backend.rest.handle("post", "/password-reset", async (inputs) => {
		const maybeUser = await users.getFromEmail(inputs.uid, /*allowNull*/ true);
		if (maybeUser === null) {
			// Don't return any error code if the account does not exists.
			await delayMs(Math.random() * 2000);
			return;
		}

		// Limit the number of password reset attempt for an interval of 1h.
		if (maybeUser.getLastPasswordResetTimestamp() + 1 * 3600 * 1000 > Date.now()) {
			await delayMs(Math.random() * 2000);
			return;
		}

		const link = await createResetPasswordLink(maybeUser);

		Log.info("Reset password email sent to: {}.", inputs.uid);
		await emails.sendResetPassword(inputs.uid, {
			email: inputs.uid,
			support: configBackend.supportURL,
			link: link,
		});
	});

	backend.rest.handle("post", "/password-change", async function (inputs) {
		const maybeUser = await users.get(inputs.uid, /*allowNull*/ true);
		if (maybeUser === null) {
			throw authentication.httpErrorUnauthorized(/*requestAuthentication*/ false);
		}
		if (!inputs.token || maybeUser.getPassword() !== inputs.token) {
			throw authentication.httpErrorUnauthorized(/*requestAuthentication*/ false);
		}
		await users.update(maybeUser.getUid(), async (user) => {
			await user.setPassword(inputs.password);
			return user;
		});
	});

	const sendContactMessage = async (captcha, from, subject, content) => {
		const response = await HttpClient.post("https://www.google.com/recaptcha/api/siteverify", {
			query: {
				secret: configBackend.googleCaptchaSecretKey,
				response: captcha,
			},
		});
		const data = JSON.parse(response);
		Exception.assertPrecondition(data.success, "The captcha token is invalid: {:j}", data["error-codes"]);
		await email.send(
			[configBackend.email.from, from],
			"[contact-" + Math.floor(Math.random() * 10000) + "] " + subject,
			{
				text:
					"==== Contact form " +
					config.url +
					" ====\n" +
					"Created: " +
					new Date().toUTCString() +
					"\n" +
					"From: " +
					from +
					"\n" +
					"Subject: " +
					subject +
					"\n\n" +
					content,
			},
		);
	};

	backend.rest.handle("post", "/contact", async (inputs) => {
		await sendContactMessage(inputs.captcha, inputs.email, inputs.subject, inputs.content);
	});

	backend.rest.handle("post", "/contact-authenticated", async (inputs, session) => {
		const user = await users.get(session.getUid());
		await sendContactMessage(inputs.captcha, user.getEmail(), inputs.subject, inputs.content);
	});

	// ---- start services ----

	await backend.services.installServices(payment);

	// ---- tests data ----

	if (program.opts().testData) {
		const testData = new TestData(users, applications, payment);
		await testData.install();
		await testData.run();
		// This allow http and https interoperability.
		headers["Referrer-Policy"] = "no-referrer-when-downgrade";
	}

	// ---- dump -----

	if (options.dump) {
		Log.info("Dumping content to {}...", options.dump);
		const usersData = await users.dump();
		const applicationsData = await applications.dump();

		await FileSystem.mkdir(options.dump);
		await FileSystem.writeFile(options.dump + "/users.json", JSON.stringify(usersData, null, 2));
		await FileSystem.writeFile(options.dump + "/applications.json", JSON.stringify(applicationsData, null, 2));
	}

	// ---- serve ----

	await backend.start();

	// ---- run tests ----

	if (backend.test) {
		await backend.web.test(config.tests || []);
		await backend.stop();
	}
})();
