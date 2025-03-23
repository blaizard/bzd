import Docker from "./docker/backend.mjs";
import Storage from "./storage/backend.mjs";
import Nodes from "./nodes/backend.mjs";

export default {
	docker: Docker,
	storage: Storage,
	nodes: Nodes,
};
