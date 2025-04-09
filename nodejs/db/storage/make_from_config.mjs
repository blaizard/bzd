import StorageDisk from "#bzd/nodejs/db/storage/disk.mjs";
import StorageGoogleCloudStorage from "#bzd/nodejs/db/storage/google_cloud_storage.mjs";
import StorageGoogleDrive from "#bzd/nodejs/db/storage/google_drive.mjs";
import StorageDockerV2 from "#bzd/nodejs/db/storage/docker_v2.mjs";
import StorageWebdav from "#bzd/nodejs/db/storage/webdav.mjs";
import StorageMemory from "#bzd/nodejs/db/storage/memory.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Validation from "#bzd/nodejs/core/validation.mjs";

const Log = LogFactory("db", "storage", "make-from-config");
const Exception = ExceptionFactory("db", "storage", "make-from-config");

function validateConfig(schema, config) {
	const validation = new Validation(schema);
	const result = validation.validate(config, { output: "result", all: true });
	Exception.assertResult(result);
}

export default async function makeFromConfig(config) {
	const commonConfig = {
		type: "mandatory",
		write: "",
		tests: "",
		options: "",
	};
	let storage = null;
	switch (config.type) {
		case "disk":
			validateConfig(
				Object.assign(
					{
						path: "mandatory",
					},
					commonConfig,
				),
				config,
			);
			storage = await StorageDisk.make(
				config.path,
				Object.assign(
					{
						write: config.write,
					},
					config.options,
				),
			);
			break;
		case "googleCloudStorage":
			validateConfig(
				Object.assign(
					{
						bucketName: "mandatory",
					},
					commonConfig,
				),
				config,
			);
			storage = await StorageGoogleCloudStorage.make(
				config.bucketName,
				Object.assign(
					{
						write: config.write,
					},
					config.options,
				),
			);
			break;
		case "googleDrive":
			validateConfig(
				Object.assign(
					{
						folderId: "mandatory",
					},
					commonConfig,
				),
				config,
			);
			storage = await StorageGoogleDrive.make(
				config.folderId,
				Object.assign(
					{
						write: config.write,
					},
					config.options,
				),
			);
			break;
		case "memory":
			validateConfig(
				Object.assign(
					{
						data: "mandatory",
					},
					commonConfig,
				),
				config,
			);
			storage = await StorageMemory.make(
				config.data,
				Object.assign(
					{
						write: config.write,
					},
					config.options,
				),
			);
			break;
		case "dockerv2":
			validateConfig(
				Object.assign(
					{
						url: "mandatory",
					},
					commonConfig,
				),
				config,
			);
			storage = await StorageDockerV2.make(
				config.url,
				Object.assign(
					{
						write: config.write,
					},
					config.options,
				),
			);
			break;
		case "webdav":
			validateConfig(
				Object.assign(
					{
						url: "mandatory",
					},
					commonConfig,
				),
				config,
			);
			storage = await StorageWebdav.make(
				config.url,
				Object.assign(
					{
						write: config.write,
					},
					config.options,
				),
			);
			break;
		default:
			Exception.unreachable("Unsupported storage type: '{}'.", config.type);
	}

	Exception.assert(storage !== null, "Storage cannot be null.");
	if (config.tests) {
		const root = config.tests === true ? "validation_tests" : config.tests;
		Log.info("Running validation tests on '{}' storage at /{}.", config.type, root);
		await storage.runValidationTests(root);
	}

	return storage;
}
