import LangPlugin from "#bzd/nodejs/vue/lang.js";
import Authentication from "#bzd/nodejs/core/authentication/token/client.js";
import AuthenticationGoogle from "#bzd/nodejs/core/authentication/google/client.js";
import AuthenticationFacebook from "#bzd/nodejs/core/authentication/facebook/client.js";
import Frontend from "#bzd/nodejs/vue/apps/frontend.js";
import APIv1 from "#bzd/api.json" with { type: "json" };
import config from "#bzd/apps/accounts/config.json" with { type: "json" };

import App from "#bzd/apps/accounts/frontend/app.vue";

// ---- Frontend ----

const frontend = Frontend.make(App);

// ---- Authentication ----

const authentication = new Authentication({
	unauthorizedCallback: async (needAuthentication) => {
		if (needAuthentication) {
			await frontend.app.config.globalProperties.$router.dispatch("/login", {
				query: { redirect: window.location.href },
			});
		} else {
			await frontend.app.config.globalProperties.$router.dispatch("/404");
		}
	},
});

const authenticationGoogle = new AuthenticationGoogle(config.googleClientId, config.url);
const authenticationFacebook = new AuthenticationFacebook(config.facebookAppId);

// ---- Frontend ----

const plugins = [authentication, authenticationGoogle, authenticationFacebook];
frontend
	.useMetadata({
		title: "Accounts",
	})
	.useRest(APIv1.rest)
	.useServices()
	.useLogger()
	.useAuthentication({}, authentication)
	.setup(plugins);

// ---- Languages ----

frontend.app.use(LangPlugin, {
	gb: () => import("./lang/gb.js"),
	fr: () => import("./lang/fr.js"),
});

frontend.mount("#app");
