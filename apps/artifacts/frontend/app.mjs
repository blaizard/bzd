import Vue from "vue";
import AsyncComputed from "vue-async-computed";

import Router from "bzd/vue/router/router.mjs";
import API from "bzd/vue/api.mjs";
import Notification from "bzd/vue/notification.mjs";
import CachePlugin from "bzd/vue/cache.mjs";
import Permissions from "bzd/db/storage/permissions.mjs";

import App from "./app.vue";
import APIv1 from "../api.v1.json";

Vue.use(AsyncComputed);
Vue.use(Notification);
Vue.use(Router, {
	hash: false
});
Vue.use(API, {
	schema: APIv1
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

			let next = 50;
			let list = [];

			do {
				const response = await Vue.prototype.$api.request("post", "/list", {
					path: pathList,
					paging: next
				});
				next = response.next;
				list = list.concat(
					response.data.map((item) => {
						item.permissions = Permissions.makeFromEntry(item);
						return item;
					})
				);
				list.sort(new Intl.Collator().compare);
			} while (next);

			return list;
		},
		default: [],
		loading: [],
		options: {
			timeout: 5000 // ms
		}
	}
});

new Vue({
	el: "#app",
	render: (h) => h(App)
});
