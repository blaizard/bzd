export default class HttpServerContextCommon {
	/// Get the client IP address.
	getIp() {
		const forwarded = this.getHeader("x-forwarded-for", null);
		if (forwarded !== null) {
			// The proxy appends the trusted address last, the first entry is client-spoofable.
			const parts = forwarded
				.split(",")
				.map((part) => part.trim())
				.filter((part) => part.length > 0 && !/\s/.test(part));
			if (parts.length > 0) {
				return parts[parts.length - 1];
			}
		}
		const remote = this.request.socket?.remoteAddress ?? this.request.connection?.remoteAddress ?? null;
		if (remote === null) {
			return null;
		}
		return remote.startsWith("::ffff:") ? remote.slice("::ffff:".length) : remote;
	}
}
