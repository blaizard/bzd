import Docker from "./docker/frontend.mjs";
import Fs from "./fs/frontend.mjs";
import Webdav from "./webdav/frontend.mjs";
import Nodes from "./nodes/frontend.mjs";

export default {
	fs: Fs,
	docker: Docker,
	webdav: Webdav,
	nodes: Nodes,
};
