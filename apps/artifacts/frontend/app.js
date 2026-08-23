import APIv1 from "#bzd/api.json" with { type: "json" };
import Frontend from "#bzd/nodejs/vue/apps/frontend.js";
import Cache2 from "#bzd/nodejs/core/cache2.js";
import Permissions from "#bzd/nodejs/db/storage/permissions.js";
import CachePlugin from "#bzd/nodejs/vue/cache.js";
import icon from "#bzd/apps/artifacts/frontend/svg/artifact.svg?url";

import App from "./app.vue";

import AsyncComputed from "vue-async-computed";

const frontend = Frontend.make(App)
	.useMetadata({
		title: "Artifacts",
		icon: icon,
	})
	.useRest(APIv1.rest)
	.useAuthentication()
	.useServices()
	.useStatistics()
	.useLogger()
	.setup();

frontend.app.use(AsyncComputed);
frontend.app.use(CachePlugin, {
	list: {
		cache: async (key) => {
			const pathList = Cache2.keyToArrayOfString(key);
			let next = 1000;
			let list = [];

			do {
				const response = await frontend.app.config.globalProperties.$rest.request("post", "/list", {
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

			list.sort((a, b) => a.name.localeCompare(b.name));
			return list;
		},
		default: [],
		loading: [],
		options: {
			timeoutMs: 5000,
		},
	},
});

frontend.mount("#app");
