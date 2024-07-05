import { program } from "commander";

import RestServer from "#bzd/nodejs/core/rest/server.mjs";
import APIv1 from "#bzd/api.json" assert { type: "json" };
import kvsMakeFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";
import emailMakeFromConfig from "#bzd/nodejs/email/make_from_config.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Authentication from "#bzd/nodejs/core/authentication/session/server.mjs";
import AuthenticationGoogle from "#bzd/nodejs/core/authentication/google/server.mjs";
import AuthenticationFacebook from "#bzd/nodejs/core/authentication/facebook/server.mjs";
import Result from "#bzd/nodejs/utils/result.mjs";
import HttpServer from "#bzd/nodejs/core/http/server.mjs";
import HttpClient from "#bzd/nodejs/core/http/client.mjs";
import Users from "#bzd/apps/accounts/backend/users/users.mjs";
import Applications from "#bzd/apps/accounts/backend/applications/applications.mjs";
import TokenInfo from "#bzd/apps/accounts/backend/users/token.mjs";
import TestData from "#bzd/apps/accounts/backend/tests/test_data.mjs";
import config from "#bzd/apps/accounts/config.json" assert { type: "json" };
import configBackend from "#bzd/apps/accounts/backend/config.json" assert { type: "json" };
import MemoryLogger from "#bzd/apps/accounts/backend/logger/memory/memory.mjs";
import paymentMakeFromConfig from "#bzd/nodejs/payment/make_from_config.mjs";
import EmailManager from "#bzd/apps/accounts/backend/email/manager.mjs";
import Services from "#bzd/nodejs/core/services/services.mjs";
import Subscription from "#bzd/apps/accounts/backend/users/subscription.mjs";
import { delayMs } from "#bzd/nodejs/utils/delay.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

program
	.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option(
		"-p, --port <number>",
		"Port to be used to serve the application, can also be set with BZD_PORT.",
		8080,
		parseInt,
	)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.option("--test", "Include test data.")
	.parse(process.argv);

const options = program.opts();
const PORT = Number(process.env.BZD_PORT || options.port);
const PATH_STATIC = options.static;

(async () => {
	const memoryLogger = new MemoryLogger();

	const keyValueStore = await kvsMakeFromConfig(configBackend.kvs.accounts);
	const keyValueStoreRegister = await kvsMakeFromConfig(configBackend.kvs.register);
	const email = await emailMakeFromConfig(configBackend.email);

	// Set-up the mail object
	const emails = new EmailManager(email);

	// Users
	const users = new Users(keyValueStore);

	// Applications
	const applications = new Applications(keyValueStore);

	// Set-up the web server
	const web = new HttpServer(PORT);

	// Services
	const services = new Services();

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
		refreshToken: async (uid, hash, factoryNewTokenInfo) => {
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
				const info = factoryNewTokenInfo();
				let updatedToken = null;
				await users.update(uid, (user) => {
					updatedToken = user.rollToken(hash, info.hash);
					updatedToken.updateMinDuration(info.minDuration);
					return user;
				});
				result.hash = info.hash;
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

	// ---- API ----

	const api = new RestServer(APIv1.rest, {
		authentication: authentication,
		channel: web,
	});
	api.installPlugins(
		authentication,
		authenticationGoogle,
		authenticationFacebook,
		users,
		applications,
		services,
		memoryLogger,
		payment,
	);

	api.handle("get", "/sso", async function (inputs, session) {
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

	api.handle("post", "/password-reset", async (inputs) => {
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

	api.handle("post", "/password-change", async function (inputs) {
		const maybeUser = await users.get(inputs.uid, /*allowNull*/ true);
		if (maybeUser === null) {
			throw this.httpError(401, "Unauthorized");
		}
		if (!inputs.token || maybeUser.getPassword() !== inputs.token) {
			throw this.httpError(401, "Unauthorized");
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

	api.handle("post", "/contact", async (inputs) => {
		await sendContactMessage(inputs.captcha, inputs.email, inputs.subject, inputs.content);
	});

	api.handle("post", "/contact-authenticated", async (inputs, session) => {
		const user = await users.get(session.getUid());
		await sendContactMessage(inputs.captcha, user.getEmail(), inputs.subject, inputs.content);
	});

	// ---- start services ----

	await services.installServices(payment);
	await services.start();

	// ---- Headers to be used ----

	let headers = {
		// Needed for Google authentication.
		"Cross-Origin-Opener-Policy": "same-origin-allow-popups",
	};

	// ---- tests data ----

	if (options.test) {
		const testData = new TestData(users, applications, payment);
		await testData.install();
		await testData.run();
		// This allow http and https interoperability.
		headers["Referrer-Policy"] = "no-referrer-when-downgrade";
	}

	// ---- serve ----

	web.addStaticRoute("/", PATH_STATIC, {
		headers: headers,
	});
	web.start();
})();
