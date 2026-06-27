import Docker from "./docker/backend.js";
import Storage from "./storage/backend.js";
import Nodes from "./nodes/backend.js";

export default {
	docker: Docker,
	storage: Storage,
	nodes: Nodes,
};
