import Url from "url";

import LogFactory from "#bzd/nodejs/core/log.mjs";
import HttpProxy from "#bzd/nodejs/core/http/proxy.mjs";

const Log = LogFactory("proxy", "docker-v2");

export default class DockerV2Proxy {
	constructor(url, port, proxiedUrl, getAuthentication) {
		this.url = url;
		this.port = port;
		this.proxiedUrl = proxiedUrl;
		this.getAuthentication = getAuthentication;
		this.proxy = null;
		this.server = null;

		this._urlParsed = new Url.URL(this.url);

		this.proxy = new HttpProxy(this.proxiedUrl, this.port, {
			timeoutMs: 84600 * 1000,
			// See https://docs.docker.com/registry/deploying/#importantrequired-http-headers
			xForward: true,
			beforeRequest: async (request, response) => {
				if (request.url.startsWith("/v2/token")) {
					const url = new Url.URL(request.url, this.proxiedUrl);
					const auth = await this.getAuthentication(url.searchParams.get("scope"));
					response.writeHead(200, { "Content-Type": "application/javascript" });
					response.end(JSON.stringify(auth));
				}
			},
			afterRequest: async (request, response) => {
				// Redirect authentication request to this proxy
				if (response.statusCode == 401 && "www-authenticate" in response.headers) {
					response.headers["www-authenticate"] =
						'Bearer realm="' + this.url + '/v2/token",service="' + this._getServiceFromUrl() + '"';
				}

				// Update location with the correct URL
				if ("location" in response.headers) {
					response.headers.location = this._updateUrlFromResponse(response.headers.location);
				}

				// This prevents the client to switch protocol if needed
				delete response["alt-svc"];

				// See https://docs.docker.com/registry/deploying/#importantrequired-http-headers
				response.headers["docker-distribution-api-version"] = "registry/2.0";
			},
		});
	}

	/// Make a proxy from a Docker Storage object.
	static makeFromStorageDockerV2(url, port, dockerStorage) {
		return new DockerV2Proxy(url, port, dockerStorage.url, async (scope) => {
			return await dockerStorage.options.authentication.call(dockerStorage, scope);
		});
	}

	/// Convert a URL from the proxy to the host
	_updateUrlFromResponse(url) {
		let parseUrl = new Url.URL(url);
		parseUrl.protocol = this._urlParsed.protocol;
		parseUrl.port = this._urlParsed.port;
		parseUrl.username = this._urlParsed.username;
		parseUrl.password = this._urlParsed.password;
		parseUrl.hostname = this._urlParsed.hostname;
		return Url.format(parseUrl);
	}

	_getServiceFromUrl() {
		return Url.format({
			hostname: this._urlParsed.hostname,
			port: this._urlParsed.port,
		});
	}

	async start() {
		await this.proxy.start();
		Log.info("Deployed Docker proxy at '{}' on port {}", this.url, this.port);
	}

	async stop() {
		await this.proxy.stop();
	}
}
