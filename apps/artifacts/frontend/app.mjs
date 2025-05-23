import configGlobal from "#bzd/apps/artifacts/config.json" with { type: "json" };
import APIv1 from "#bzd/api.json" with { type: "json" };
import Frontend from "#bzd/nodejs/vue/apps/frontend.mjs";
import Permissions from "#bzd/nodejs/db/storage/permissions.mjs";
import CachePlugin from "#bzd/nodejs/vue/cache.mjs";
import icon from "#bzd/apps/artifacts/frontend/svg/artifact.svg?url";

import App from "./app.vue";

import AsyncComputed from "vue-async-computed";

const frontend = Frontend.make(App)
	.useMetadata({
		title: "Artifacts",
		icon: icon,
	})
	.useRest(APIv1.rest)
	.useAuthentication(configGlobal.accounts)
	.useRest(APIv1.rest)
	.setup();

frontend.app.use(AsyncComputed);
frontend.app.use(CachePlugin, {
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

frontend.mount("#app");
