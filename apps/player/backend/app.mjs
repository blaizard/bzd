import { Command } from "commander/esm.mjs";

import Path from "path";
import API from "bzd/core/api/server.mjs";
import APIv1 from "../api.v1.json";
import HttpServer from "bzd/core/http/server.mjs";
import LogFactory from "bzd/core/log.mjs";
import FileSystem from "bzd/core/filesystem.mjs";
import StorageDisk from "bzd/db/storage/disk.mjs";
import { spawn } from "child_process";
import { Readable } from "stream";
import Scenario from "../lib/scenario.mjs";
import Permissions from "bzd/db/storage/permissions.mjs";
import { WebSocketServer } from "ws";
import Terminal from "./terminal.mjs";

const Log = LogFactory("backend");

const program = new Command();
program
	.version("1.0.0", "-v, --version")
	.usage("[OPTIONS]...")
	.option(
		"-p, --port <number>",
		"Port to be used to serve the application, can also be set with the environemnt variable BZD_PORT.",
		8080,
		parseInt
	)
	.option("-s, --static <path>", "Directory to static serve.", ".")
	.parse(process.argv);

function pathToPathList(path) {
	return path.split("/").filter(Boolean);
}

(async () => {
	// Read arguments
	const PORT = process.env.BZD_PORT || program.opts().port;
	const PATH_STATIC = program.opts().static;
	const PATH_SCENARIO = program.args[0];
	//const PATH_CACHE = Path.join(Path.dirname(PATH_SCENARIO), ".player", "cache");

	const raw = await FileSystem.readFile(PATH_SCENARIO);
	const scenarioData = JSON.parse(raw);
	const scenario = new Scenario(scenarioData);

	const PATH_STORAGE = Path.join(Path.dirname(PATH_SCENARIO), ".player", scenario.name);
	const storage = new StorageDisk(PATH_STORAGE);

	// websocket ----------------

	const wss = new WebSocketServer({ port: 9999 });

	wss.on("connection", (ws) => {
		let terminal = new Terminal(PATH_STORAGE);
		terminal.on("data", (data) => {
			ws.send(data);
		});
		terminal.on("exit", () => {
			ws.close();
		});
		ws.on("message", async (event) => {
			const input = JSON.parse(event.toString());
			switch (input.type) {
			case "init":
				await terminal.init(input.value);
				break;
			case "stream":
				await terminal.write(input.value);
				break;
			default:
				Log.error("Unsupported data type '{}' for terminal.", input.type);
			}
		});
	});

	// -----------------

	// Set-up the web server
	let web = new HttpServer(PORT);
	// Install the APIs
	let api = new API(APIv1, {
		channel: web,
	});

	api.handle("get", "/scenario", async () => {
		return scenario.data;
	});

	api.handle("get", "/file/content", async (inputs) => {
		return await storage.read(pathToPathList(inputs.path));
	});

	api.handle("post", "/file/content", async (inputs) => {
		await storage.writeFromChunk(pathToPathList(inputs.path), inputs.content || "");
	});

	api.handle("post", "/file/executable", async (inputs) => {
		await storage.setPermission(
			pathToPathList(inputs.path),
			new Permissions({
				read: true,
				write: true,
				executable: true,
			})
		);
	});

	api.handle("get", "/file/list", async (inputs) => {
		const pathList = pathToPathList(inputs.path);
		const maxOrPaging = "paging" in inputs ? inputs.paging : 50;

		const result = await storage.list(pathList, maxOrPaging, /*includeMetadata*/ true);

		return {
			data: result.data(),
			next: result.getNextPaging(),
		};
	});

	api.handle("post", "/exec", async (inputs) => {
		const cmd = inputs.cmds.shift();
		const args = inputs.cmds;
		let process = spawn(cmd, args, {
			cwd: PATH_STORAGE,
			timeout: 60 * 1000, // 1 min
		});
		const stream = new Readable({
			read() {},
		});
		process.stdout.on("data", (data) => {
			stream.push(data);
		});
		process.stderr.on("data", (data) => {
			stream.push(data);
		});
		process.on("close", () => {
			stream.push(null);
		});
		return stream;
	});

	Log.info("Application started");
	web.addStaticRoute("/", PATH_STATIC, "index.html");
	web.start();
})();
