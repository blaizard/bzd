import Vue from "vue";

import App from "#bzd/apps/accounts/frontend/app.vue";
import APIPlugin from "#bzd/nodejs/vue/api.mjs";
import APIv1 from "#bzd/api.json" assert { type: "json" };
import RouterPlugin from "#bzd/nodejs/vue/router/router.mjs";
import LangPlugin from "#bzd/nodejs/vue/lang.mjs";
import Authentication from "#bzd/nodejs/core/authentication/token/client.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";
import Notification from "#bzd/nodejs/vue/notification.mjs";

// ---- Languages ----

Vue.use(LangPlugin, {
	gb: () => import("./lang/gb.mjs"),
	fr: () => import("./lang/fr.mjs"),
});

// ---- Notifications ----

Vue.use(Notification);

// ---- Authentication ----

const authentication = new Authentication({
	unauthorizedCallback: () => {
		const route = Vue.prototype.$routerGet();
		Vue.prototype.$routerDispatch("/login", route ? { query: { redirect: route } } : {});
	},
});
Vue.use(AuthenticationPlugin, authentication);
Vue.use(RouterPlugin, {
	hash: false,
	authentication: authentication,
});

// ---- API ----

Vue.use(APIPlugin, {
	schema: APIv1,
	authentication: authentication,
	plugins: [authentication],
});

new Vue({
	el: "#app",
	render: (h) => h(App),
});
