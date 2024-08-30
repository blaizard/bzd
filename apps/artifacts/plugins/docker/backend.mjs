import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import StorageDockerV2 from "#bzd/nodejs/db/storage/docker_v2.mjs";
import PluginBase from "#bzd/apps/artifacts/backend/plugin.mjs";

import DockerV2Proxy from "./docker_v2_proxy.mjs";

const Exception = ExceptionFactory("plugins", "docker");

export default class Plugin extends PluginBase {
	constructor(volume, options, provider, endpoints) {
		super(volume, options, provider, endpoints);

		provider.addStartProcess(async () => {
			let storage = null;
			switch (options["docker.type"]) {
				case "v2":
					storage = await StorageDockerV2.make(options["docker.url"]);
					break;
				case "gcr":
					storage = await StorageDockerV2.makeFromGcr(options["docker.key"], options["docker.service"]);
					break;
				default:
					Exception.unreachable("Unsupported Docker type '{}'", options["docker.type"]);
			}
			this.setStorage(storage);
		});

		if (options["docker.proxy"] == true) {
			this.proxy = null;
			provider.addStartProcess(async () => {
				this.proxy = DockerV2Proxy.makeFromStorageDockerV2(
					options["docker.proxy.url"],
					options["docker.proxy.port"],
					this.getStorage(),
				);
				await this.proxy.start();
			});

			provider.addStopProcess(async () => {
				await this.proxy.stop();
				this.proxy = null;
			});
		}
	}
}
