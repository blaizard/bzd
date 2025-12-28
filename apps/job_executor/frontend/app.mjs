import APIv1 from "#bzd/api.json" with { type: "json" };
import Frontend from "#bzd/nodejs/vue/apps/frontend.mjs";
import icon from "#bzd/apps/job_executor/frontend/icon.svg?url";

import App from "./app.vue";

const frontend = Frontend.make(App)
	.useMetadata({
		title: "Job Executor",
		description: "Execute jobs on the server.",
		keywords: ["Job executor", "Job execution", "Server job execution"],
		icon: icon,
	})
	.useRest(APIv1.rest)
	.useWebsocket(APIv1.websocket)
	.useAuthentication()
	.useStatistics()
	.useServices()
	.useLogger()
	.setup();
frontend.mount("#app");
