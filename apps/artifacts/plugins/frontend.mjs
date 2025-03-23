import Docker from "./docker/frontend.mjs";
import Storage from "./storage/frontend.mjs";
import Nodes from "./nodes/frontend.mjs";

export default {
	docker: Docker,
	storage: Storage,
	nodes: Nodes,
};
