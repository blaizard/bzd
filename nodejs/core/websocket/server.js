import { ExceptionFactory } from "../exception.js";
import LogFactory from "../log.js";

import Base from "./base.js";

const Exception = ExceptionFactory("websocket", "server");
const Log = LogFactory("websocket", "server");

export default class WebsocketServer extends Base {
	constructor(schema, options) {
		super(schema, options);
	}

	/// Register a websocket
	handle(endpoint, callback) {
		Exception.assert(this.options.channel, "Channel is missing");
		this._sanityCheck(endpoint);

		this.options.channel.addRouteWebsocket(this.getEndpoint(endpoint), (ws) => {
			callback(ws);
		});
	}
}
