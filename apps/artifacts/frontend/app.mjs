import { createApp } from "vue";

import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";
import RestPlugin from "#bzd/nodejs/vue/rest.mjs";
import CachePlugin from "#bzd/nodejs/vue/cache.mjs";
import Notification from "#bzd/nodejs/vue/notification.mjs";
import Router from "#bzd/nodejs/vue/router/router.mjs";
import AsyncComputed from "vue-async-computed";
import Authentication from "#bzd/apps/accounts/authentication/client.mjs";
import AuthenticationPlugin from "#bzd/nodejs/vue/authentication.mjs";
import configGlobal from "#bzd/apps/artifacts/config.json" assert { type: "json" };

import APIv1 from "#bzd/api.json" assert { type: "json" };

import App from "./app.vue";

const app = createApp(App);

// ---- Authentication ----

const authentication = new Authentication({
	accounts: configGlobal.accounts,
	unauthorizedCallback: async (needAuthentication) => {
		console.log("Unauthorized!", needAuthentication);
	},
});
app.use(AuthenticationPlugin, {
	authentication: authentication,
});

// ---- REST ----

app.use(AsyncComputed);
app.use(Notification);
app.use(Router, {
	hash: false,
});
app.use(RestPlugin, {
	schema: APIv1.rest,
	authentication: authentication,
});

app.use(CachePlugin, {
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
				const response = await app.config.globalProperties.$rest.request("post", "/list", {
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

app.mount("#app");
