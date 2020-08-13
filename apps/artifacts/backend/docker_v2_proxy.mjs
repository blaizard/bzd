import Http from "http";
import HttpProxy from "http-proxy";
import LogFactory from "bzd/core/log.mjs";
import Url from "url";

const Log = LogFactory("docker-v2-proxy");

export default class DockerV2Proxy {
	constructor(port, dockerStorage) {
		this.port = port;
		this.url = dockerStorage.url;
		this.dockerStorage = dockerStorage;
		this.proxy = null;
	}

	async start() {
		this.proxy = HttpProxy.createProxyServer({});

		this.proxy.on("proxyRes", (proxyRes /*, req, res*/) => {
			if (proxyRes.statusCode == 401 && "www-authenticate" in proxyRes.headers) {
				proxyRes.headers["www-authenticate"] = "Bearer realm=\"http://127.0.0.1:5050/v2/token\",service=\"127.0.0.1:5050\"";
			}
		});

		let server = Http.createServer(async (req, res) => {
			if (req.url.startsWith("/v2/token")) {
				const queryObject = Url.parse(req.url, true).query;
				const auth = await this.dockerStorage.options.authentication.call(this.dockerStorage, queryObject.scope);
				res.writeHead(200, { "Content-Type": "application/javascript" });
				res.end(JSON.stringify(auth));
			}
			else {
				this.proxy.web(req, res, {
					target: this.url,
					changeOrigin: true
				});
			}
		});

		Log.info("Docker proxy deployed at port {}", this.port);
		server.listen(this.port);
	}

	async close() {
		this.proxy.close();
	}
}
