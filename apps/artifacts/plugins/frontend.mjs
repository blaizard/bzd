import Docker from "./docker/frontend.mjs";
import Fs from "./fs/frontend.mjs";
import Webdav from "./webdav/frontend.mjs";
import Bzd from "./bzd/frontend.mjs";

export default {
	fs: Fs,
	docker: Docker,
	webdav: Webdav,
	bzd: Bzd,
};
