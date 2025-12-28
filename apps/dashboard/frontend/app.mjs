import Frontend from "#bzd/nodejs/vue/apps/frontend.mjs";
import APIv1 from "#bzd/api.json" with { type: "json" };
import icon from "#bzd/apps/dashboard/frontend/icon.svg?url";

import App from "./app.vue";

const frontend = Frontend.make(App)
	.useMetadata({
		title: "Dashboard",
		icon: icon,
	})
	.useRest(APIv1.rest)
	.useAuthentication()
	.useServices()
	.useLogger()
	.setup();
frontend.mount("#app");
