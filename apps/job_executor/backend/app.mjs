import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import APIv1 from "#bzd/api.json" with { type: "json" };
import Jobs from "#bzd/apps/job_executor/jobs.json" with { type: "json" };
import Args from "#bzd/apps/job_executor/backend/args.mjs";
import Backend from "#bzd/nodejs/vue/apps/backend.mjs";
import Commands from "#bzd/apps/job_executor/backend/commands.mjs";
import pathlib from "#bzd/nodejs/utils/pathlib.mjs";
import config from "#bzd/apps/job_executor/backend/config.json" with { type: "json" };
import { FileNotFoundError } from "#bzd/nodejs/db/storage/storage.mjs";

const Exception = ExceptionFactory("backend");
const Log = LogFactory("backend");

(async () => {
	const backend = Backend.makeFromCli(process.argv)
		.useAuthentication()
		.useRest(APIv1.rest)
		.useWebsocket(APIv1.websocket)
		.useStatistics()
		.useServices()
		.useForm()
		.setup();

	// Convert the raw form data into relative data to the sandbox root.
	async function formDataToSandbox(contextJob, inputs, formData) {
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
							await contextJob.moveTo(originalPath, relativePath);
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

	let commands = new Commands(pathlib.path("sandbox"), {
		services: backend.services.makeProvider("commands"),
	});
	await commands.initialize();

	backend.rest.handle("post", "/job/send", async (inputs) => {
		Exception.assertPrecondition(inputs.id in Jobs, "Job id is not known: {}", inputs.id);
		const schema = Jobs[inputs.id];
		const contextJob = await commands.allocate();
		const uid = contextJob.getUid();

		try {
			// Build the input data.
			const data = await formDataToSandbox(contextJob, schema.inputs, inputs.data);

			await commands.makeFromSchema(uid, schema, data);
			await commands.detach(uid);

			Log.info("Executing job {}", uid);
		} catch (error) {
			Log.error("Executing job {}", uid);
			await commands.remove(uid);
			throw error;
		}

		return {
			job: uid,
		};
	});

	backend.rest.handle("get", "/jobs", async (inputs) => {
		return {
			jobs: await commands.getAllInfo(),
			timestamp: Date.now(),
		};
	});

	backend.rest.handle("delete", "/job/{id}/cancel", async (inputs) => {
		await commands.kill(inputs.id);
	});

	backend.rest.handle("delete", "/job/{id}/delete", async (inputs) => {
		await commands.remove(inputs.id);
	});

	backend.rest.handle("post", "/files/{id}", async function (inputs) {
		try {
			const maxOrPaging = "paging" in inputs ? inputs.paging : 50;
			const contextJob = commands.getContext(inputs.id);
			const result = await contextJob.list(inputs.path, maxOrPaging, /*includeMetadata*/ true);

			return {
				data: result.data(),
				next: result.getNextPaging(),
			};
		} catch (e) {
			if (e instanceof FileNotFoundError) {
				throw this.httpError(404, "Not Found");
			}
			throw e;
		}
	});

	backend.rest.handle(
		"get",
		"/file/{id}",
		async function (inputs) {
			const contextJob = commands.getContext(inputs.id);
			try {
				const pathList = inputs.path.split("/");
				const metadata = await contextJob.metadata(pathList);
				if (metadata.size) {
					this.setHeader("Content-Length", metadata.size);
				}
				this.setHeader("Content-Disposition", 'attachment; filename="' + encodeURI(metadata.name) + '"');
				return await contextJob.read(pathList);
			} catch (e) {
				if (e instanceof FileNotFoundError) {
					throw this.httpError(404, "Not Found");
				}
				throw e;
			}
		},
		{
			timeoutS: 10 * 60, // 10min timeout
		},
	);

	backend.websocket.handle("/job/{id}", (context) => {
		const uid = context.getParam("id");
		commands.installCommandWebsocket(context, uid);
	});

	await backend.start();
})();
