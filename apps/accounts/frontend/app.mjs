import Vue from "vue";

import App from "#bzd/apps/accounts/frontend/app.vue";
import RouterPlugin from "#bzd/nodejs/vue/router/router.mjs";
import LangPlugin from "#bzd/nodejs/vue/lang.mjs";

Vue.use(LangPlugin, {
	gb: () => import("./lang/gb.mjs"),
	fr: () => import("./lang/fr.mjs"),
});

Vue.use(RouterPlugin, {
	hash: false,
	//authentication: authentication,
});

new Vue({
	el: "#app",
	render: (h) => h(App),
});
