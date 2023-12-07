import { createApp } from "vue";

import App from "#bzd/apps/accounts/frontend/app.vue";
import APIPlugin from "#bzd/nodejs/vue/api.mjs";
import APIv1 from "#bzd/api.json" assert { type: "json" };
import RouterPlugin from "#bzd/nodejs/vue/router/router.mjs";
import LangPlugin from "#bzd/nodejs/vue/lang.mjs";
import Authentication from "#bzd/nodejs/core/authentication/token/client.mjs";
import AuthenticationGoogle from "#bzd/nodejs/core/authentication/google/client.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";
import Notification from "#bzd/nodejs/vue/notification.mjs";
import Config from "#bzd/apps/accounts/config.json" assert { type: "json" };

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
			await app.config.globalProperties.$routerDispatch("/login", { query: { redirect: window.location.href } });
		} else {
			await app.config.globalProperties.$routerDispatch("/404");
		}
	},
});
const authenticationGoogle = new AuthenticationGoogle(Config.googleClientId);

app.use(AuthenticationPlugin, {
	authentication: authentication,
});
app.use(RouterPlugin, {
	hash: false,
	authentication: authentication,
});

// ---- API ----

app.use(APIPlugin, {
	schema: APIv1,
	authentication: authentication,
	plugins: [authentication, authenticationGoogle],
});

app.mount("#app");
