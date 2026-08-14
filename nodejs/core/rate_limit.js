/// Sliding-window rate limiter backed by a Cache2 collection.
///
/// Each key maps to a list of timestamps for the sliding window. The collection TTL equals
/// the window so Cache2's garbage collector bounds the memory used. Keys are stored raw.
export default class RateLimiter {
	constructor(cache, config) {
		this.config = Object.assign(
			{
				/// Maximum number of requests allowed within a window.
				threshold: 5,
				/// Size of the sliding window in milliseconds.
				windowMs: 15 * 60 * 1000,
				/// Clock used to get the current time, defaults to the system clock.
				clock: { getTimeMs: () => Date.now() },
			},
			config,
		);
		this.cache = cache;
		// Register the collection so `set`/`getInstant` pass Cache2's collection assertion;
		// the fetch function is never invoked as entries are always set manually.
		cache.register(this.config.bucket, () => ({ timestamps: [] }), { timeoutMs: this.config.windowMs });
	}

	/// Return whether the given key is currently over the limit, without recording a new request.
	isOverLimit(key) {
		const now = this.config.clock.getTimeMs();
		const entry = this.cache.getInstant(this.config.bucket, key, null);
		if (entry === null) {
			return false;
		}
		const timestamps = (entry.timestamps ?? []).filter((timestamp) => timestamp > now - this.config.windowMs);
		return timestamps.length >= this.config.threshold;
	}

	/// Record a new request for the given key.
	///
	/// \return true if the request is allowed, false if it is over the limit.
	async record(key) {
		const now = this.config.clock.getTimeMs();
		const entry = this.cache.getInstant(this.config.bucket, key, { timestamps: [] });
		const timestamps = (entry.timestamps ?? []).filter((timestamp) => timestamp > now - this.config.windowMs);
		timestamps.push(now);
		// Cap the number of timestamps kept so the stored entry remains bounded.
		if (timestamps.length > this.config.threshold) {
			timestamps.splice(0, timestamps.length - this.config.threshold);
		}
		this.cache.set(this.config.bucket, key, { timestamps });
		return timestamps.length < this.config.threshold;
	}

	/// Reset the rate limit for the given key.
	///
	/// Cache2 has no per-key delete, so an empty entry is stored instead; it expires after
	/// the window and is reclaimed by the garbage collector.
	reset(key) {
		this.cache.set(this.config.bucket, key, { timestamps: [] });
	}
}
