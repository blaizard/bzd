import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import APIv1 from "#bzd/api.json" with { type: "json" };
import Jobs from "#bzd/apps/job_executor/jobs.json" with { type: "json" };
import Args from "#bzd/apps/job_executor/backend/args.mjs";
import Backend from "#bzd/nodejs/vue/apps/backend.mjs";
import Command from "#bzd/apps/job_executor/backend/command.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

(async () => {
	const backend = Backend.makeFromCli(process.argv)
		.useAuthentication()
		.useRest(APIv1.rest)
		.useWebsocket(APIv1.websocket)
		.useStatistics()
		.useForm()
		.setup();

	/// Generate the command.
	function makeCommandFromJob(job, args) {
		if ("command" in job) {
			return [job["command"], ...args];
		}
		if ("docker" in job) {
			return ["docker", "run", "--rm", job["docker"], ...args];
		}
		Exception.unreachable("Undefined command for this job: {:j}", job);
	}

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

	let nextCommandId = 0;
	let commands = {};

	backend.rest.handle("post", "/job/send", async (inputs) => {
		Exception.assertPrecondition(inputs.id in Jobs, "Job id is not known: {}", inputs.id);
		const job = Jobs[inputs.id];
		const [data, files] = formDataToData(job.inputs, inputs.data);
		const args = new Args(job.args, data);
		const command = args.process();
		const commandId = nextCommandId++;
		Log.info("Processing job {} with arguments {:j}", commandId, command);
		commands[commandId] = {
			type: inputs.id,
			command: new Command(makeCommandFromJob(job, command)),
		};
		commands[commandId].command.execute();

		return {
			job: commandId,
		};
	});

	backend.rest.handle("get", "/jobs", async (inputs) => {
		let jobs = {};
		for (const [jobId, data] of Object.entries(commands)) {
			const info = data.command.getInfo();
			jobs[jobId] = Object.assign(
				{
					type: data.type,
				},
				info,
			);
		}
		return {
			jobs: jobs,
			timestamp: Date.now(),
		};
	});

	backend.websocket.handle("/job/{id}", (context) => {
		const jobId = context.getParam("id");
		Exception.assertPrecondition(jobId in commands, "Job id is not known: {}", jobId);

		const command = commands[jobId].command;
		const onData = (data) => {
			context.send(data);
		};
		command.on("data", onData);
		context.exit(() => {
			command.remove("data", onData);
		});

		context.read((data) => {
			console.log("read", data);
		});
	});

	await backend.start();
})();
