import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import { HttpClient } from "#bzd/nodejs/core/http/client.js";
import pathlib from "#bzd/nodejs/utils/pathlib.js";
import config from "#bzd/apps/artifacts/api/config.json" with { type: "json" };

const Exception = ExceptionFactory("artifacts", "api");
const Log = LogFactory("artifacts", "api");

Exception.assert(config.remotes.length > 0, "'remotes' from the API config cannot be empty.");

export class ArtifactsBase {
	constructor({
		uid = null,
		remotes = config.remotes,
		volume = config.defaultNodeVolume,
		token = process.env.BZD_NODE_TOKEN ?? config.token,
		logger = Log,
		httpClient = HttpClient,
	} = {}) {
		this.uid = uid;
		this.remoteSources = remotes;
		this.volume = volume;
		this.token = token;
		this.logger = logger;
		this.remote = null;
		this.httpClient = httpClient;
	}

	/// Lazily iterate over remote sources, retrying the current remote (if any).
	///
	/// \yields [remote, retry, nbRetries] arrays.
	*remotes() {
		if (this.remote) {
			for (let i = 0; i < 3; ++i) {
				yield [this.remote, i, 2];
			}
		}
		for (const remote of this.remoteSources) {
			this.remote = remote;
			yield [this.remote, 0, 0];
		}
		this.remote = null;
	}

	/// Convert a path string to a normalized storage key (array of path segments).
	static pathToKey(path) {
		return pathlib.path(path).normalize.parts;
	}

	/// Build a list of human-readable attribute tokens for `toString()`.
	repr_() {
		const content = [];
		if (this.uid !== null) {
			content.push(`uid=${this.uid}`);
		}
		content.push(`remotes=${this.remoteSources.join(",")}`);
		if (this.volume !== null) {
			content.push(`volume=${this.volume}`);
		}
		if (this.token !== null) {
			content.push("token=xxx");
		}
		return content;
	}

	/// Human-readable representation; token is always masked as `xxx` when set.
	toString() {
		return `<${this.constructor.name} ${this.repr_().join(" ")}>`;
	}
}
