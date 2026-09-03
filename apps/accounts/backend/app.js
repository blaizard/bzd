import { Command } from "commander";

import Backend from "#bzd/nodejs/vue/apps/backend.js";
import APIv1 from "#bzd/api.json" with { type: "json" };
import kvsMakeFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.js";
import emailMakeFromConfig from "#bzd/nodejs/email/make_from_config.js";
import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import RateLimiter from "#bzd/nodejs/core/rate_limit.js";
import Authentication from "#bzd/nodejs/core/authentication/session/server.js";
import AuthenticationGoogle from "#bzd/nodejs/core/authentication/google/server.js";
import AuthenticationFacebook from "#bzd/nodejs/core/authentication/facebook/server.js";
import Result from "#bzd/nodejs/utils/result.js";
import { HttpClient } from "#bzd/nodejs/core/http/client.js";
import User from "#bzd/apps/accounts/backend/users/user.js";
import Users from "#bzd/apps/accounts/backend/users/users.js";
import Applications from "#bzd/apps/accounts/backend/applications/applications.js";
import TokenInfo from "#bzd/apps/accounts/backend/users/token.js";
import TestData from "#bzd/apps/accounts/backend/test_data/test_data.js";
import { configUrl, configGoogleClientId } from "#bzd/apps/accounts/config_nodejs.js";
import {
	configKvs,
	configEmail,
	configGoogleClientSecret,
	configGoogleCaptchaSecretKey,
	configSupportURL,
	configPayment,
	configTests,
} from "#bzd/apps/accounts/backend/config_nodejs.js";
import paymentMakeFromConfig from "#bzd/nodejs/payment/make_from_config.js";
import EmailManager from "#bzd/apps/accounts/backend/email/manager.js";
import Subscription from "#bzd/apps/accounts/backend/users/subscription.js";
import FileSystem from "#bzd/nodejs/core/filesystem.js";
import { delayMs } from "#bzd/nodejs/utils/delay.js";

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
		.useServices()
		.useCache();

	const keyValueStore = await kvsMakeFromConfig(configKvs().accounts);
	const email = await emailMakeFromConfig(configEmail());

	// Set-up the mail object
	const emails = new EmailManager(email);

	// Users
	const users = new Users(keyValueStore);

	// Applications
	const applications = new Applications(keyValueStore);

	// Rate limiting
	let emailLimiter;
	let ipLimiter;

	// ---- Helpers ----

	/// Generate a reset password like for a specific user.
	async function createResetPasswordLink(user, newPassword = false) {
		// Create a single-use, expiring reset token and persist only its hash.
		let token;
		user = await users.update(
			user.getUid(),
			(user) => {
				token = user.setRandomPasswordResetToken();
				return user;
			},
			/*silent*/ true,
		);

		return (
			configUrl() +
			"/" +
			(newPassword ? "new" : "reset") +
			"/" +
			encodeURIComponent(user.getUid()) +
			"/" +
			encodeURIComponent(token)
		);
	}

	// ---- Authentication ----

	let authentication = new Authentication({
		verifyIdentity: async (email, password = null, callerId = null) => {
			// Per-caller (IP) rate limiting applies first, if a caller identifier is available.
			if (callerId && (await ipLimiter.isOverLimit(callerId))) {
				return Result.makeError(Authentication.ErrorVerifyIdentity.tooManyAttempts);
			}
			if (await emailLimiter.isOverLimit(email)) {
				return Result.makeError(Authentication.ErrorVerifyIdentity.tooManyAttempts);
			}

			// Add timing randomness to avoid account enumeration.
			await delayMs(Math.random() * 20);

			const maybeUser = await users.getFromEmail(email, /*allowNull*/ true);
			if (maybeUser === null) {
				await emailLimiter.record(email);
				if (callerId) {
					await ipLimiter.record(callerId);
				}
				return Result.makeError(Authentication.ErrorVerifyIdentity.unauthorized);
			}

			const passwordOk =
				password !== null
					? await maybeUser.isPasswordEqual(password)
					: // SSO login: the identity was already verified upstream, no password to check.
						true;

			if (!passwordOk) {
				await emailLimiter.record(email);
				if (callerId) {
					await ipLimiter.record(callerId);
				}
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

			await emailLimiter.reset(email);
			if (callerId) {
				await ipLimiter.reset(callerId);
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
		configGoogleClientId(),
		configGoogleClientSecret(),
		configUrl(),
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
					support: configSupportURL(),
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
		configPayment(),
	);

	// ---- REST ----

	backend.useAuthentication(authentication).useRest(APIv1.rest).useLoggerMemory().setup();

	emailLimiter = new RateLimiter(backend.cache, {
		bucket: "rate.limit.login",
		threshold: 5,
		windowMs: 900 * 1000,
	});
	ipLimiter = new RateLimiter(backend.cache, {
		bucket: "rate.limit.ip",
		threshold: 30,
		windowMs: 900 * 1000,
	});

	backend.rest.installPlugins(authenticationGoogle, authenticationFacebook, users, applications, payment);

	backend.rest.handle("get", "/sso", async function (inputs, session) {
		// Get that the application exists.
		const application = await applications.get(inputs.application, /*allowNull*/ true);
		Exception.assertPrecondition(application, "Application '{}' does not exists.", inputs.application);

		// Get the SSO token.
		const token = await backend.authentication.makeSSOToken(
			inputs.application,
			session,
			application.getScopes().toList(),
		);

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
			support: configSupportURL(),
			link: link,
		});
	});

	backend.rest.handle("post", "/password-change", async function (inputs) {
		const maybeUser = await users.get(inputs.uid, /*allowNull*/ true);
		if (maybeUser === null) {
			throw backend.authentication.httpErrorUnauthorized(/*requestAuthentication*/ false);
		}
		await users.update(maybeUser.getUid(), async (user) => {
			if (!user.checkPasswordResetToken(inputs.token)) {
				throw backend.authentication.httpErrorUnauthorized(/*requestAuthentication*/ false);
			}
			await user.setPassword(inputs.password);
			user.clearPasswordResetToken();
			return user;
		});
	});

	const sendContactMessage = async (captcha, from, subject, content) => {
		const response = await HttpClient.post("https://www.google.com/recaptcha/api/siteverify", {
			query: {
				secret: configGoogleCaptchaSecretKey(),
				response: captcha,
			},
		});
		const data = JSON.parse(response);
		Exception.assertPrecondition(data.success, "The captcha token is invalid: {:?}", data["error-codes"]);
		await email.send([configEmail().from, from], "[contact-" + Math.floor(Math.random() * 10000) + "] " + subject, {
			text:
				"==== Contact form " +
				configUrl() +
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
		});
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
		await backend.web.test(configTests());
		await backend.stop();
	}
})();
