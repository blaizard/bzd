import { program } from "commander";

import API from "#bzd/nodejs/core/api/server.mjs";
import APIv1 from "#bzd/api.json" assert { type: "json" };
import kvsMakeFromConfig from "#bzd/nodejs/db/key_value_store/make_from_config.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import Authentication from "#bzd/nodejs/core/authentication/session/server.mjs";
import AuthenticationGoogle from "#bzd/nodejs/core/authentication/google/server.mjs";
import HttpServer from "#bzd/nodejs/core/http/server.mjs";
import Services from "#bzd/apps/accounts/backend/services/services.mjs";
import PendingActions from "#bzd/apps/accounts/backend/pending_actions/pending_actions.mjs";
import Users from "#bzd/apps/accounts/backend/users/users.mjs";
import Applications from "#bzd/apps/accounts/backend/applications/applications.mjs";
import TokenInfo from "#bzd/apps/accounts/backend/users/token.mjs";
import TestData from "#bzd/apps/accounts/backend/tests/test_data.mjs";
import Config from "#bzd/apps/accounts/config.json" assert { type: "json" };
import ConfigBackend from "#bzd/apps/accounts/backend/config.json" assert { type: "json" };
import MemoryLogger from "#bzd/apps/accounts/backend/logger/memory/memory.mjs";

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

	const keyValueStore = await kvsMakeFromConfig(ConfigBackend.kvs.accounts);
	const keyValueStoreRegister = await kvsMakeFromConfig(ConfigBackend.kvs.register);

	// Users
	const users = new Users(keyValueStore);

	// Applications
	const appplications = new Applications(keyValueStore);

	// Set-up the web server
	const web = new HttpServer(PORT);

	let authentication = new Authentication({
		verifyIdentity: async (email, password = null) => {
			const maybeUser = await users.getFromEmail(email, /*allowNull*/ true);
			if (maybeUser === null) {
				return false;
			}
			// Check the password if provided.
			if (password !== null) {
				const isEqual = await maybeUser.isPasswordEqual(password);
				if (!isEqual) {
					return false;
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
			return {
				scopes: maybeUser.getScopes().toList(),
				uid: maybeUser.getUid(),
			};
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

	const authenticationGoogle = new AuthenticationGoogle({
		clientId: Config.googleClientId,
	});

	// Create the pending actions module
	const pendingActions = new PendingActions(keyValueStoreRegister, {
		register: {
			run: async (uid, data) => {
				console.log("PendingActions register", uid, data);
				//data.roles = ["users"];
				//await users.create(data);
			},
		},
	});
	const services = new Services();

	pendingActions.registerGarbageCollector(services, "register");

	// ---- API ----

	const api = new API(APIv1, {
		authentication: authentication,
		channel: web,
	});
	await api.installPlugins(
		authentication,
		authenticationGoogle,
		users,
		appplications,
		pendingActions,
		services,
		memoryLogger,
	);

	api.handle("get", "/sso", async function (inputs, session) {
		// Get that the application exists.
		const application = await appplications.get(inputs.application, /*allowNull*/ true);
		Exception.assertPrecondition(application, "Application '{}' does not exists.", inputs.application);

		// Get the SSO token.
		const token = await authentication.makeSSOToken(inputs.application, session, application.getScopes().toList());

		return {
			token: token,
			redirect: application.getRedirect(),
		};
	});

	api.handle("post", "/register", async (inputs, session) => {
		const activationCode = await pendingActions.create("group", session.getUid(), inputs);
		return String(activationCode);
	});

	// ---- services ----

	services.start();

	// ---- tests data ----

	if (options.test) {
		const testData = new TestData(users, appplications);
		await testData.install();
	}

	// ---- serve ----

	web.addStaticRoute("/", PATH_STATIC, {
		headers: {
			// Needed for Google authentication.
			"Cross-Origin-Opener-Policy": "same-origin-allow-popups",
		},
	});
	web.start();
})();
