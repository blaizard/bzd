import StorageDockerV2 from "bzd/db/storage/docker_v2.mjs";

export default {
	storage(params) {
		switch (params["docker.preset"]) {
		case "gcr":
			return StorageDockerV2.makeFromGcr(params["docker.key"], params["docker.service"]);
		default:
			return new StorageDockerV2(params["docker.url"]);
		}
	}
};
