import { lookup } from "dns/promises";
import { isIP } from "net";

import ExceptionFactory from "#bzd/nodejs/core/exception.js";

const Exception = ExceptionFactory("http", "client", "ssrf");

// IPv4 networks that must never be accessed, as (base, prefix) pairs.
const IPV4_BLOCKED_RANGES = [
	[0x00000000, 8], // 0.0.0.0/8
	[0x0a000000, 8], // 10.0.0.0/8 (RFC 1918)
	[0x64400000, 10], // 100.64.0.0/10 (CGNAT)
	[0x7f000000, 8], // 127.0.0.0/8 (loopback)
	[0xa9fe0000, 16], // 169.254.0.0/16 (link-local)
	[0xac100000, 12], // 172.16.0.0/12 (RFC 1918)
	[0xc0000000, 24], // 192.0.0.0/24 (IETF protocol assignments)
	[0xc0a80000, 16], // 192.168.0.0/16 (RFC 1918)
	[0xc6120000, 15], // 198.18.0.0/15 (benchmarking)
	[0xe0000000, 4], // 224.0.0.0/4 (multicast)
	[0xf0000000, 4], // 240.0.0.0/4 (reserved)
	[0xffffffff, 32], // 255.255.255.255
];

// IPv6 networks that must never be accessed, as (base, prefix) pairs.
const IPV6_BLOCKED_RANGES = [
	[0x0n, 128], // ::
	[0x1n, 128], // ::1 (loopback)
	[0xfc000000000000000000000000000000n, 7], // fc00::/7 (unique local)
	[0xfe800000000000000000000000000000n, 10], // fe80::/10 (link-local)
	[0xff000000000000000000000000000000n, 8], // ff00::/8 (multicast)
	[0x20010db8000000000000000000000000n, 32], // 2001:db8::/32 (documentation)
];

function ipv4ToInt(address) {
	const octets = address.split(".");
	if (octets.length !== 4) {
		return null;
	}
	const parts = octets.map(Number);
	if (parts.some((octet) => !Number.isInteger(octet) || octet < 0 || octet > 255)) {
		return null;
	}
	return ((parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3]) >>> 0;
}

function isIpv4Blocked(value) {
	for (const [base, prefix] of IPV4_BLOCKED_RANGES) {
		const mask = (~0 << (32 - prefix)) >>> 0;
		if ((value & mask) === (base & mask)) {
			return true;
		}
	}
	return false;
}

function ipv6ToBigInt(address) {
	const index = address.indexOf("::");
	if (index < 0) {
		const groups = address.split(":");
		if (groups.length !== 8 || groups.some((group) => !/^[0-9a-fA-F]{1,4}$/.test(group))) {
			return null;
		}
		let value = 0n;
		for (const group of groups) {
			value = (value << 16n) | BigInt(parseInt(group, 16));
		}
		return value;
	}
	const head = address.slice(0, index).split(":").filter(Boolean);
	const tail = address
		.slice(index + 2)
		.split(":")
		.filter(Boolean);
	const groups = [...head, ...tail];
	if (groups.some((group) => !/^[0-9a-fA-F]{1,4}$/.test(group)) || groups.length > 7) {
		return null;
	}
	const missing = 8 - groups.length;
	const expanded = [...head, ...Array(missing).fill("0"), ...tail];
	let value = 0n;
	for (const group of expanded) {
		value = (value << 16n) | BigInt(parseInt(group, 16));
	}
	return value;
}

function isIpv6Blocked(value) {
	for (const [base, prefix] of IPV6_BLOCKED_RANGES) {
		const mask = ((1n << 128n) - 1n) ^ ((1n << BigInt(128 - prefix)) - 1n);
		if ((value & mask) === (base & mask)) {
			return true;
		}
	}
	return false;
}

/// Normalize an IPv4-mapped IPv6 address (::ffff:a.b.c.d or ::ffff:xxxx:xxxx) to its IPv4 form.
function normalizeIpv4MappedIpv6(address) {
	const lower = address.toLowerCase();
	if (!lower.startsWith("::ffff:")) {
		return null;
	}
	const tail = address.slice(7);
	if (tail.includes(".")) {
		const value = ipv4ToInt(tail);
		return value === null ? null : tail;
	}
	const parts = tail.split(":");
	if (parts.length !== 2 || parts.some((part) => !/^[0-9a-fA-F]{1,4}$/.test(part))) {
		return null;
	}
	const value = (parseInt(parts[0], 16) << 16) | parseInt(parts[1], 16);
	return [(value >>> 24) & 0xff, (value >>> 16) & 0xff, (value >>> 8) & 0xff, value & 0xff].join(".");
}

function isBlockedAddress(address) {
	if (isIP(address) === 4) {
		const value = ipv4ToInt(address);
		return value !== null && isIpv4Blocked(value);
	}
	if (isIP(address) === 6) {
		const ipv4 = normalizeIpv4MappedIpv6(address);
		if (ipv4 !== null) {
			const value = ipv4ToInt(ipv4);
			return value !== null && isIpv4Blocked(value);
		}
		const value = ipv6ToBigInt(address);
		return value !== null && isIpv6Blocked(value);
	}
	return false;
}

function parseAllowListEntry(entry) {
	try {
		const url = new URL("http://" + entry);
		return {
			hostname: url.hostname,
			port: url.port,
		};
	} catch (e) {
		return null;
	}
}

function isHostnameAllowed(url, allowList) {
	for (const entry of allowList) {
		const parsed = parseAllowListEntry(entry);
		if (parsed === null) {
			continue;
		}
		if (parsed.hostname !== url.hostname) {
			continue;
		}
		if (parsed.port && parsed.port !== url.port) {
			continue;
		}
		return true;
	}
	return false;
}

function isAddressAllowed(address, allowList) {
	const normalized = normalizeIpv4MappedIpv6(address) ?? address;
	for (const entry of allowList) {
		const parsed = parseAllowListEntry(entry);
		if (parsed === null) {
			continue;
		}
		const host = parsed.hostname.replace(/^\[|\]$/g, "");
		if (isIP(host) && host === normalized) {
			return true;
		}
	}
	return false;
}

/// Assert that the given url is safe to be fetched by the server.
///
/// \param urlString The url to validate.
/// \param allowList A list of hosts (optionally with a port) that are explicitly allowed,
///        even if they resolve to an internal address.
export async function assertUrlSafe(urlString, { allowList = [] } = {}) {
	let url;
	try {
		url = new URL(urlString);
	} catch (e) {
		Exception.errorPrecondition("The url '{}' is malformed: {}", urlString, e.message);
	}
	Exception.assertPrecondition(
		url.protocol === "http:" || url.protocol === "https:",
		"The url '{}' must use the http or https scheme, got '{}'.",
		urlString,
		url.protocol,
	);

	// An explicit allow-list entry short-circuits before any resolution happens.
	if (isHostnameAllowed(url, allowList)) {
		return;
	}

	const hostname = url.hostname.replace(/^\[|\]$/g, "");
	let addresses;
	try {
		addresses = (await lookup(hostname, { all: true, verbatim: true })).map((entry) => entry.address);
	} catch (e) {
		Exception.errorPrecondition("The url '{}' cannot be resolved: {}", urlString, e.message);
	}
	Exception.assertPrecondition(addresses.length > 0, "The url '{}' does not resolve to any address.", urlString);

	for (const address of addresses) {
		if (isAddressAllowed(address, allowList)) {
			continue;
		}
		Exception.assertPrecondition(
			!isBlockedAddress(address),
			"The url '{}' resolves to the blocked address '{}'.",
			urlString,
			address,
		);
	}
}
