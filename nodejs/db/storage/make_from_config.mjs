import StorageDisk from "#bzd/nodejs/db/storage/disk.mjs";
import StorageGoogleCloudStorage from "#bzd/nodejs/db/storage/google_cloud_storage.mjs";
import StorageGoogleDrive from "#bzd/nodejs/db/storage/google_drive.mjs";
import StorageDockerV2 from "#bzd/nodejs/db/storage/docker_v2.mjs";
import StorageWebdav from "#bzd/nodejs/db/storage/webdav.mjs";
import StorageMemory from "#bzd/nodejs/db/storage/memory.mjs";
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import Validation from "#bzd/nodejs/core/validation.mjs";

const Exception = ExceptionFactory("db", "storage", "make-from-config");

function validateConfig(schema, config) {
	const validation = new Validation(schema);
	const result = validation.validate(config, { output: "result", all: true });
	Exception.assertResult(result);
}

export default async function makeFromConfig(config) {
	switch (config.type) {
		case "disk":
			validateConfig(
				{
					type: "mandatory",
					path: "mandatory",
					options: "",
				},
				config,
			);
			return await StorageDisk.make(config.path, config.options);
		case "googleCloudStorage":
			validateConfig(
				{
					type: "mandatory",
					bucketName: "mandatory",
					options: "",
				},
				config,
			);
			return await StorageGoogleCloudStorage.make(config.bucketName, config.options);
		case "googleDrive":
			validateConfig(
				{
					type: "mandatory",
					folderId: "mandatory",
					options: "",
				},
				config,
			);
			return await StorageGoogleDrive.make(config.folderId, config.options);
		case "memory":
			validateConfig(
				{
					type: "mandatory",
					data: "mandatory",
					options: "",
				},
				config,
			);
			return await StorageMemory.make(config.data, config.options);
		case "dockerv2":
			validateConfig(
				{
					type: "mandatory",
					url: "mandatory",
					options: "",
				},
				config,
			);
			return await StorageDockerV2.make(config.url, config.options);
		case "webdav":
			validateConfig(
				{
					type: "mandatory",
					url: "mandatory",
					options: "",
				},
				config,
			);
			return await StorageWebdav.make(config.url, config.options);
		default:
			Exception.unreachable("Unsupported storage type: '{}'.", config.type);
	}
}
