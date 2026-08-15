export default class HttpServerContextCommon {
	constructor() {
		// List of trusted proxies, only for these peers the "x-forwarded-for" header is honored.
		const trustedProxies = (process.env.BZD_TRUSTED_PROXIES ?? "")
			.split(",")
			.map((s) => s.trim())
			.filter(Boolean);
		this.trustedProxies_ = new Set(trustedProxies.map((ip) => this._normalizeIp(ip)));
	}

	/// Normalize an IP address, in particular removing the IPv4-mapped IPv6 prefix.
	_normalizeIp(ip) {
		return ip.startsWith("::ffff:") ? ip.slice("::ffff:".length) : ip;
	}

	/// Get the client IP address.
	getIp() {
		// The peer is the source of the connection as seen by the server, it cannot be spoofed.
		const peer = this._normalizeIp(this.request.socket?.remoteAddress ?? this.request.connection?.remoteAddress ?? "");
		if (peer === "") {
			return null;
		}
		// Only honor the "x-forwarded-for" header when the immediate peer is a trusted proxy,
		// otherwise a client could spoof or rotate the IP to bypass per-IP rate limiting.
		if (this.trustedProxies_.has(peer)) {
			const forwarded = this.getHeader("x-forwarded-for", null);
			if (forwarded !== null) {
				// The proxy appends the trusted address last, the first entry is client-spoofable.
				const parts = forwarded
					.split(",")
					.map((part) => part.trim())
					.filter((part) => part.length > 0 && !/\s/.test(part))
					.map((part) => this._normalizeIp(part));
				// Return the rightmost entry that is not itself a trusted proxy.
				for (let i = parts.length - 1; i >= 0; --i) {
					if (!this.trustedProxies_.has(parts[i])) {
						return parts[i];
					}
				}
			}
		}
		return peer;
	}
}
