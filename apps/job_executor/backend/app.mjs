import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import APIv1 from "#bzd/api.json" with { type: "json" };
import Jobs from "#bzd/apps/job_executor/jobs.json" with { type: "json" };
import Args from "#bzd/apps/job_executor/backend/args.mjs";
import Backend from "#bzd/nodejs/vue/apps/backend.mjs";
import Commands from "#bzd/apps/job_executor/backend/commands.mjs";
import FileSystem from "#bzd/nodejs/core/filesystem.mjs";
import { makeUid } from "#bzd/nodejs/utils/uid.mjs";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import config from "#bzd/apps/job_executor/backend/config.json" with { type: "json" };
import Executor from "#bzd/apps/job_executor/backend/executor.mjs";
import ExecutorDocker from "#bzd/apps/job_executor/backend/executor_docker.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

function makeExecutor(root, schema) {
	if ("command" in schema) {
		return new Executor(root, schema);
	}
	if ("docker" in schema) {
		return new ExecutorDocker(root, schema);
	}
	Exception.unreachable("Undefined executor for this job: {:j}", schema);
}

(async () => {
	const backend = Backend.makeFromCli(process.argv)
		.useAuthentication()
		.useRest(APIv1.rest)
		.useWebsocket(APIv1.websocket)
		.useStatistics()
		.useForm()
		.setup();

	const sandboxPath = pathlib.path("sandbox").absolute();
	await FileSystem.rmdir(sandboxPath.asPosix(), { force: true });

	// Convert the raw form data into relative data to the sandbox root.
	async function formDataToSandbox(root, inputs, formData) {
		let data = {};
		for (const item of inputs) {
			const key = item.name;
			const type = item.type;
			Exception.assert(type, "Input item {} has no type", key);
			Exception.assert(key, "Input item {} has no name", item);
			const value = formData[key];
			if (value !== undefined) {
				switch (type) {
					case "File":
						const originalPaths = value.map((entry) => backend.form.getUploadFile(entry.file.path).path);
						data[key] = [];
						// Move all files into an input directory within the sandbox.
						for (const originalPath of originalPaths) {
							const relativePath = pathlib.path("inputs").joinPath(pathlib.path(originalPath).name).asPosix();
							const path = pathlib.path(root).joinPath(relativePath);
							await FileSystem.mkdir(path.parent.asPosix(), { force: true });
							await FileSystem.move(originalPath, path.asPosix());
							data[key].push(relativePath);
						}
						break;
					default:
						data[key] = value;
				}
			}
		}
		return data;
	}

	let commands = new Commands();

	backend.rest.handle("post", "/job/send", async (inputs) => {
		Exception.assertPrecondition(inputs.id in Jobs, "Job id is not known: {}", inputs.id);
		const schema = Jobs[inputs.id];
		const jobId = commands.allocate();
		const root = sandboxPath.joinPath("job-" + jobId).asPosix();
		await FileSystem.mkdir(root, { force: true });

		try {
			// Build the input data.
			const data = await formDataToSandbox(root, schema.inputs, inputs.data);
			const makeArgs = (visitor) => {
				const updatedData = Object.fromEntries(
					Object.entries(data).map(([key, value]) => {
						const item = schema.inputs.find((item) => item.name == key);
						return [key, visitor(item.type || "unknown", value)];
					}),
				);
				const args = new Args(schema.args, updatedData);
				return args.process();
			};
			commands.make(jobId, makeArgs);

			const executor = makeExecutor(root, schema);
			await commands.detach(executor, jobId);

			Log.info("Executing job {}", jobId);
		} catch (error) {
			await FileSystem.rmdir(root, { force: true });
			Log.error("Executing job {}", jobId);
			throw error;
		}

		return {
			job: jobId,
		};
	});

	backend.rest.handle("get", "/jobs", async (inputs) => {
		return {
			jobs: await commands.getAllInfo(),
			timestamp: Date.now(),
		};
	});

	backend.rest.handle("delete", "/job/{id}", async (inputs) => {
		await commands.kill(inputs.id);
		Log.info("Killing job {}", inputs.id);
	});

	backend.websocket.handle("/job/{id}", (context) => {
		const jobId = context.getParam("id");
		commands.installCommandWebsocket(context, jobId);
	});

	await backend.start();
})();
