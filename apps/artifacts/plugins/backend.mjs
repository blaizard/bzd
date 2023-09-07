import Docker from "./docker/backend.mjs";
import Fs from "./fs/backend.mjs";
import Webdav from "./webdav/backend.mjs";
import Bzd from "./bzd/backend.mjs";

export default {
	fs: Fs,
	docker: Docker,
	webdav: Webdav,
	bzd: Bzd,
};
