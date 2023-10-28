import Authentication from "#bzd/nodejs/core/authentication/token/client.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";
import API from "#bzd/nodejs/vue/api.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";
import CachePlugin from "#bzd/nodejs/vue/cache.mjs";
import Notification from "#bzd/nodejs/vue/notification.mjs";
import Router from "#bzd/nodejs/vue/router/router.mjs";
import Vue from "vue";
import AsyncComputed from "vue-async-computed";

import APIv1 from "#bzd/api.json" assert { type: "json" };

import App from "./app.vue";

let authentication = new Authentication({
	unauthorizedCallback: () => {
		const route = Vue.prototype.$routerGet();
		Vue.prototype.$routerDispatch("/login", route ? { query: { redirect: route } } : {});
	},
});

Vue.use(AsyncComputed);
Vue.use(Notification);
Vue.use(AuthenticationPlugin, authentication);
Vue.use(Router, {
	hash: false,
	authentication: authentication,
});
Vue.use(API, {
	schema: APIv1,
	authentication: authentication,
	plugins: [authentication],
});

Vue.use(CachePlugin, {
	list: {
		cache: async (...pathList) => {
			// The following 3 statements need to be fixed, not clean

			pathList.pop(); // options
			pathList.pop(); // previous value

			if (pathList.length == 1 && pathList[0] == "default") {
				pathList.pop();
			}

			let next = 1000;
			let list = [];

			do {
				const response = await Vue.prototype.$api.request("post", "/list", {
					path: pathList,
					paging: next,
				});
				next = response.next;
				list = list.concat(
					response.data.map((item) => {
						item.permissions = Permissions.makeFromEntry(item);
						return item;
					}),
				);
			} while (next);
			list.sort((a, b) => Intl.Collator().compare(a.name, b.name));

			return list;
		},
		default: [],
		loading: [],
		options: {
			timeout: 5000, // ms
		},
	},
});

new Vue({
	el: "#app",
	render: (h) => h(App),
});
