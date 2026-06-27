import Docker from "./docker/frontend.js";
import Storage from "./storage/frontend.js";
import Nodes from "./nodes/frontend.js";

export default {
	docker: Docker,
	storage: Storage,
	nodes: Nodes,
};
