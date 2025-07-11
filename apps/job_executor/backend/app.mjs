import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import APIv1 from "#bzd/api.json" with { type: "json" };
import Jobs from "#bzd/apps/job_executor/jobs.json" with { type: "json" };
import Args from "#bzd/apps/job_executor/backend/args.mjs";
import Backend from "#bzd/nodejs/vue/apps/backend.mjs";
import TerminalWebsocket from "#bzd/nodejs/vue/components/terminal/backend/websocket.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

(async () => {
	const backend = Backend.makeFromCli(process.argv)
		.useAuthentication()
		.useRest(APIv1.rest)
		.useStatistics()
		.useForm()
		.setup();

	const terminalWebsocket = new TerminalWebsocket();
	backend.rest.installPlugins(terminalWebsocket);

	// Convert the raw form data to a more usable format.
	//
	// This function also converts the file inputs to their paths and returns a list of files.
	function formDataToData(inputs, formData) {
		let data = {};
		let files = [];
		for (const item of inputs) {
			const key = item.name;
			const type = item.type;
			Exception.assert(type, "Input item {} has no type", key);
			Exception.assert(key, "Input item {} has no name", item);
			const value = formData[key];
			if (value !== undefined) {
				switch (type) {
					case "File":
						data[key] = value.map((entry) => backend.form.getUploadFile(entry.file.path).path);
						files = files.concat(data[key]);
						break;
					default:
						data[key] = value;
				}
			}
		}
		return [data, files];
	}

	backend.rest.handle("post", "/job/send", async (inputs) => {
		Exception.assertPrecondition(inputs.id in Jobs, "Job id is not known: {}", inputs.id);
		Log.info("Received job request: {:j}", inputs);
		const job = Jobs[inputs.id];
		const [data, files] = formDataToData(job.inputs, inputs.data);
		const args = new Args(job.args, data);
		const command = args.process();
		console.log("Command", command);
	});

	await backend.start();
})();
