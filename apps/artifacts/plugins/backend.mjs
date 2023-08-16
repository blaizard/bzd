import Docker from "./docker/backend.mjs";
import Fs from "./fs/backend.mjs";
import Webdav from "./webdav/backend.mjs";

export default {
	fs: Fs,
	docker: Docker,
	webdav: Webdav,
};
