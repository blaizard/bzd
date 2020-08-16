import StorageDockerV2 from "bzd/db/storage/docker_v2.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";

const Exception = ExceptionFactory("plugins", "docker");

export default {
	storage(params) {
		switch (params["docker.type"]) {
		case "v2":
			return new StorageDockerV2(params["docker.url"]);
		case "gcr":
			return StorageDockerV2.makeFromGcr(params["docker.key"], params["docker.service"]);
		default:
			Exception.unreachable("Unsupported Docker type '{}'", params["docker.type"]);
		}
	}
};
