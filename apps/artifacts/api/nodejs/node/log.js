import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import LogFactory from "#bzd/nodejs/core/log.js";
import { timestampUs } from "#bzd/nodejs/utils/timestamp.js";

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
		/// Pending flush, or null when no flush is scheduled.
		this.flushPromise = null;

		/// Register as a log backend at 'info' level: receives info, warning and error entries.
		this.logger.addBackend("artifacts", (...args) => this.processor(...args), "info");

		/// Flush the buffered entries periodically.
		this.timer = setInterval(() => this.flush(), this.flushIntervalS * 1000);
		this.timer.unref();
	}

	processor(_date, level, topics, message) {
		this.buffer.push({
			timestampUs: timestampUs(),
			value: {
				[this.name]: {
					topic: (topics ?? []).join("::"),
					level: level,
					message: message,
				},
			},
		});
		if (this.buffer.length >= this.maxBufferSize / 2) {
			this.flush();
		}
	}

	/// Flush the buffered logs.
	///
	/// This is only a trigger: publish() drains the whole buffer. If a flush is
	/// already pending, coalesce this call into it so a slow remote cannot stack
	/// an unbounded chain of publishes. New logs wait for the next flush.
	flush() {
		if (this.flushPromise === null) {
			this.flushPromise = this.publish().finally(() => {
				this.flushPromise = null;
			});
		}
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
