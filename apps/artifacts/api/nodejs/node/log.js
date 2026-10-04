import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";

const Exception = ExceptionFactory("artifacts", "api");
const Log = LogFactory("artifacts", "api");

/// A log backend that publishes log entries to an artifacts node.
export class LoggerBackendNode {
	constructor({ node = null, name = null, flushIntervalS = 1, maxBufferSize = 1000, logger = Log } = {}) {
		Exception.assert(node, "A node must be provided.");
		Exception.assert(name, "A name must be provided.");
		this.node = node;
		this.name = name;
		this.flushIntervalS = flushIntervalS;
		this.maxBufferSize = maxBufferSize;
		this.logger = logger;
		this.buffer = [];
		/// Chain of pending publishes, serialized to avoid overlapping requests.
		this.flushPromise = Promise.resolve();

		/// Register as a log backend at 'trace' level: receives all log entries (all levels including trace).
		this.logger.addBackend("artifacts", (...args) => this.processor(...args), "info");

		/// Flush the buffered entries periodically.
		this.timer = setInterval(() => this.flush(), this.flushIntervalS * 1000);
		this.timer.unref();
	}

	processor(date, level, topics, message) {
		this.buffer.push({
			timestampUs: date.getTime() * 1000,
			value: {
				[this.name]: {
					topic: (topics ?? []).join("::"),
					level: level,
					message: message,
					source: "",
				},
			},
		});
		if (this.buffer.length >= this.maxBufferSize / 2) {
			this.flush();
		}
	}

	/// Publish all buffered entries in a single bulk request.
	async flush() {
		/// Serialize the publishes so that concurrent flush requests never overlap.
		this.flushPromise = this.flushPromise.then(() => this.publish());
		return this.flushPromise;
	}

	/// Publish the current buffer, re-buffering the entries if the remote is unreachable.
	async publish() {
		if (this.buffer.length === 0) {
			return;
		}
		const entries = this.buffer;
		this.buffer = [];
		try {
			await this.node.publishBulk({ path: ["log"] }, (publish) => {
				for (const entry of entries) {
					publish({ timestampUs: entry.timestampUs, value: entry.value });
				}
			});
		} catch (e) {
			console.error("Unable to publish logs to artifacts: ", e.message);

			/// Keep the entries, including those logged while publishing, for a later retry.
			this.buffer = [...entries, ...this.buffer].slice(-this.maxBufferSize);
		}
	}

	close() {
		clearInterval(this.timer);
	}
}
