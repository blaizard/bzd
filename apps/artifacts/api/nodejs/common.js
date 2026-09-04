import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import { HttpClient } from "#bzd/nodejs/core/http/client.js";
import { delayMs } from "#bzd/nodejs/utils/delay.js";
import pathlib from "#bzd/nodejs/utils/pathlib.js";
import { configDefaultNodeVolume, configRemotes, configToken } from "#bzd/apps/artifacts/api/nodejs/config.js";

const Exception = ExceptionFactory("artifacts", "api");
const Log = LogFactory("artifacts", "api");

Exception.assert(configRemotes().length > 0, "'remotes' from the API config cannot be empty.");

export class ArtifactsBase {
	constructor({
		uid = null,
		remotes = configRemotes(),
		volume = configDefaultNodeVolume(),
		token = process.env.BZD_NODE_TOKEN ?? configToken(),
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

	/// Run an async callback against each remote until one succeeds.
	///
	/// \param callback Async function invoked with the remote as argument.
	/// \param errorMessage The error message of the thrown error.
	/// \param retryForS The maximal number of seconds to retry before giving up.
	async tryRemotes(callback, errorMessage, retryForS = null) {
		const timestampStart = Date.now();
		for (;;) {
			for (const [remote, retry, nbRetries] of this.remotes()) {
				try {
					return await callback(remote);
				} catch (e) {
					this.logger.error(`Remote '${remote}' failed (attempt ${retry + 1}/${nbRetries + 1}): ${e}`);
				}
			}
			if (retryForS === null) {
				break;
			}
			const timestampElapsed = (Date.now() - timestampStart) / 1000;
			if (timestampElapsed > retryForS) {
				break;
			}
			await delayMs(Math.min(retryForS - timestampElapsed, 30) * 1000);
		}
		throw Exception.error(errorMessage);
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
