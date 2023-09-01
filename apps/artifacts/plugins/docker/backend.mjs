import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import StorageDockerV2 from "#bzd/nodejs/db/storage/docker_v2.mjs";

import DockerV2Proxy from "./docker_v2_proxy.mjs";

const Exception = ExceptionFactory("plugins", "docker");

export default {
	async storage(params) {
		switch (params["docker.type"]) {
			case "v2":
				return await StorageDockerV2.make(params["docker.url"]);
			case "gcr":
				return await StorageDockerV2.makeFromGcr(params["docker.key"], params["docker.service"]);
			default:
				Exception.unreachable("Unsupported Docker type '{}'", params["docker.type"]);
		}
	},
	services: {
		proxy: {
			async is(params) {
				return params["docker.proxy"] == true;
			},
			async start(params, context) {
				const proxy = DockerV2Proxy.makeFromStorageDockerV2(
					params["docker.proxy.url"],
					params["docker.proxy.port"],
					await context.getVolume(),
				);
				await proxy.start();
				return proxy;
			},
			async stop(proxy) {
				await proxy.stop();
			},
		},
	},
};
