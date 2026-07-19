import Frontend from "#bzd/nodejs/vue/apps/frontend.js";
import APIv1 from "#bzd/api.json" with { type: "json" };
import icon from "#bzd/nodejs/vue/apps/example/icon.svg?url";

import App from "./frontend.vue";

const frontend = Frontend.make(App)
	.useMetadata({
		title: "Minimal Example",
		icon: icon,
	})
	.useRest(APIv1.rest)
	.useAuthentication()
	.useServices()
	.useStatistics()
	.useLogger()
	.setup();
frontend.mount("#app");
