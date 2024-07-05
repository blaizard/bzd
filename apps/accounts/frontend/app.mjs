import { createApp } from "vue";

import App from "#bzd/apps/accounts/frontend/app.vue";
import RestPlugin from "#bzd/nodejs/vue/rest.mjs";
import APIv1 from "#bzd/api.json" assert { type: "json" };
import RouterPlugin from "#bzd/nodejs/vue/router/router.mjs";
import LangPlugin from "#bzd/nodejs/vue/lang.mjs";
import Authentication from "#bzd/nodejs/core/authentication/token/client.mjs";
import AuthenticationGoogle from "#bzd/nodejs/core/authentication/google/client.mjs";
import AuthenticationFacebook from "#bzd/nodejs/core/authentication/facebook/client.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";
import Notification from "#bzd/nodejs/vue/notification.mjs";
import config from "#bzd/apps/accounts/config.json" assert { type: "json" };

const app = createApp(App);

// ---- Languages ----

app.use(LangPlugin, {
	gb: () => import("./lang/gb.mjs"),
	fr: () => import("./lang/fr.mjs"),
});

// ---- Notifications ----

app.use(Notification);

// ---- Authentication ----

const authentication = new Authentication({
	unauthorizedCallback: async (needAuthentication) => {
		if (needAuthentication) {
			await app.config.globalProperties.$router.dispatch("/login", { query: { redirect: window.location.href } });
		} else {
			await app.config.globalProperties.$router.dispatch("/404");
		}
	},
});

const authenticationGoogle = new AuthenticationGoogle(config.googleClientId, config.url);
const authenticationFacebook = new AuthenticationFacebook(config.facebookAppId);

app.use(AuthenticationPlugin, {
	authentication: authentication,
});
app.use(RouterPlugin, {
	hash: false,
	authentication: authentication,
	plugins: [authentication, authenticationGoogle, authenticationFacebook],
});

// ---- REST ----

app.use(RestPlugin, {
	schema: APIv1.rest,
	authentication: authentication,
	plugins: [authentication, authenticationGoogle, authenticationFacebook],
});

app.mount("#app");
