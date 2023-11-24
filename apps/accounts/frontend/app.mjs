import { createApp } from "vue";

import App from "#bzd/apps/accounts/frontend/app.vue";
import APIPlugin from "#bzd/nodejs/vue/api.mjs";
import APIv1 from "#bzd/api.json" assert { type: "json" };
import RouterPlugin from "#bzd/nodejs/vue/router/router.mjs";
import LangPlugin from "#bzd/nodejs/vue/lang.mjs";
import Authentication from "#bzd/nodejs/core/authentication/token/client.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";
import Notification from "#bzd/nodejs/vue/notification.mjs";

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
			const route = app.config.globalProperties.$routerGet();
			await app.config.globalProperties.$routerDispatch("/login", route ? { query: { redirect: route } } : {});
		} else {
			await app.config.globalProperties.$routerDispatch("/404");
		}
	},
});
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
	plugins: [authentication],
});

app.mount("#app");
