import LangPlugin from "#bzd/nodejs/vue/lang.mjs";
import Authentication from "#bzd/nodejs/core/authentication/token/client.mjs";
import AuthenticationGoogle from "#bzd/nodejs/core/authentication/google/client.mjs";
import AuthenticationFacebook from "#bzd/nodejs/core/authentication/facebook/client.mjs";
import Frontend from "#bzd/nodejs/vue/apps/frontend.mjs";
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
	.useAuthentication({}, authentication)
	.setup(plugins);

// ---- Languages ----

frontend.app.use(LangPlugin, {
	gb: () => import("./lang/gb.mjs"),
	fr: () => import("./lang/fr.mjs"),
});

frontend.mount("#app");
