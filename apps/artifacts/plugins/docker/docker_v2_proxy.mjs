import Http from "http";
import HttpProxy from "http-proxy";
import LogFactory from "bzd/core/log.mjs";
import ExceptionFactory from "bzd/core/exception.mjs";
import Url from "url";

const Log = LogFactory("docker-v2-proxy");
const Exception = ExceptionFactory("docker-v2-proxy");

export default class DockerV2Proxy {
	constructor(url, port, proxiedUrl, getAuthentication) {
		this.url = url;
		this.port = port;
		this.proxiedUrl = proxiedUrl;
		this.getAuthentication = getAuthentication;
		this.proxy = null;
		this.server = null;

		this._urlParsed = new Url.URL(this.url);
	}

	/**
	 * Make a proxy from a Docker Storage object.
	 */
	static makeFromStorageDockerV2(url, port, dockerStorage) {
		return new DockerV2Proxy(url, port, dockerStorage.url, async (scope) => {
			return await dockerStorage.options.authentication.call(dockerStorage, scope);
		});
	}

	/**
	 * Test function to sniff and display insights from the HTTP requests
	 */
	_sniffProxyRes(proxyRes, req) {
		let proxyResData = "";
		proxyRes.on("data", (chunk) => {
			proxyResData += chunk;
		});
		proxyRes.on("end", () => {
			const snifferData = {
				request: {
					data: req.body,
					headers: req.headers,
					url: req.url,
					method: req.method
				},
				response: {
					data: proxyResData,
					headers: proxyRes.headers,
					statusCode: proxyRes.statusCode
				}
			};
			console.log(snifferData);
		});
	}

	/**
	 * Convert a URL from the proxy to the host
	 */
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
			port: this._urlParsed.port
		});
	}

	async start() {
		this.proxy = HttpProxy.createProxyServer({});

		this.proxy.on("proxyRes", (proxyRes, req /*, res*/) => {
			// Redirect authentication request to this proxy
			if (proxyRes.statusCode == 401 && "www-authenticate" in proxyRes.headers) {
				proxyRes.headers["www-authenticate"] =
					"Bearer realm=\"" + this.url + "/v2/token\",service=\"" + this._getServiceFromUrl() + "\"";
			}

			// Update location with the correct URL
			if ("location" in proxyRes.headers) {
				proxyRes.headers.location = this._updateUrlFromResponse(proxyRes.headers.location);
			}

			this._sniffProxyRes(proxyRes, req);
		});

		this.proxy.on("error", (e) => {
			Exception.print("{}", Exception.fromError(e));
		});

		this.server = Http.createServer(async (req, res) => {
			if (req.url.startsWith("/v2/token")) {
				const queryObject = Url.parse(req.url, true).query;
				const auth = await this.getAuthentication(queryObject.scope);
				res.writeHead(200, { "Content-Type": "application/javascript" });
				res.end(JSON.stringify(auth));
			}
			else {
				this.proxy.web(req, res, {
					target: this.proxiedUrl,
					changeOrigin: true
				});
			}
		});

		Log.info("Deployed Docker proxy at '{}' on port {}", this.url, this.port);
		this.server.listen(this.port);
	}

	_closeServer() {
		return new Promise((resolve) => {
			this.server.close(() => {
				resolve();
			});
		});
	}

	async stop() {
		this.proxy.close();
		await this._closeServer();
	}
}
