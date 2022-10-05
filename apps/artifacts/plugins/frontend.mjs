import Fs from "./fs/frontend.mjs";
import Docker from "./docker/frontend.mjs";
import Webdav from "./webdav/frontend.mjs";

export default {
	fs: Fs,
	docker: Docker,
	webdav: Webdav,
};
