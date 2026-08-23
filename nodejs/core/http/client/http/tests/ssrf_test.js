import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import { assertUrlSafe } from "#bzd/nodejs/core/http/client/ssrf.js";

const Exception = ExceptionFactory("test", "http", "ssrf");

describe("SSRF guard", () => {
	describe("rejects blocked targets", () => {
		const blockedUrls = [
			"unix:///var/run/docker.sock",
			"file:///etc/passwd",
			"http://127.0.0.1:8081/",
			"http://localhost/",
			"http://169.254.169.254/latest/meta-data/",
			"http://10.0.0.1/",
			"http://172.16.0.1/",
			"http://192.168.1.1/",
			"http://0.0.0.0/",
			"http://[::1]/",
			"http://[fc00::1]/",
			"http://[fe80::1]/",
			"http://[::ffff:127.0.0.1]/",
		];
		for (const url of blockedUrls) {
			it("rejects '" + url + "'", async () => {
				await Exception.assertThrows(async () => {
					await assertUrlSafe(url);
				});
			});
		}
	});

	it("accepts a public address", async () => {
		await assertUrlSafe("http://8.8.8.8/");
	});

	it("accepts an allow-listed address with a port", async () => {
		await assertUrlSafe("http://127.0.0.1:8081/", { allowList: ["127.0.0.1:8081"] });
	});

	it("accepts an allow-listed hostname with a port", async () => {
		await assertUrlSafe("http://internal.registry.local:8080/x", { allowList: ["internal.registry.local:8080"] });
	});
});
