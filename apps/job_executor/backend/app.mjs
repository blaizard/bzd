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
		.useLoggerMemory()
		.setup();

	let commands = new Commands(pathlib.path("sandbox"), {
		services: backend.services.makeProvider("commands"),
	});
	await commands.initialize();

	backend.rest.handle("post", "/job/send", async function () {
		const contextJob = await commands.allocate();
		const uid = contextJob.getUid();

		try {
			const inputs = await this.processForm(async (key, file, info) => {
				const path = ["inputs", info.name];
				await contextJob.write(path, file);
				return path.join("/");
			});
			Exception.assertPrecondition(inputs.type in Jobs, "Job type is not known: {}", inputs.type);

			// Build the input data.
			await commands.makeFromSchema(contextJob, Jobs[inputs.type], inputs);
			await commands.schedule(uid, { type: "queue" });

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

	backend.rest.handle("post", "/job/{id}/start", async (inputs) => {
		const info = await commands.schedule(inputs.id, { type: "immediately" });
		return {
			info: info,
		};
	});

	backend.rest.handle("delete", "/job/{id}/cancel", async (inputs) => {
		const info = await commands.terminate(inputs.id);
		return {
			info: info,
		};
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
