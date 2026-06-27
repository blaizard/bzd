import { ExceptionFactory } from "../exception.js";
import LogFactory from "../log.js";
import Endpoints from "#bzd/nodejs/core/http/endpoints.js";
import { WebsocketClient as WebsocketClientImpl } from "#bzd/nodejs/core/http/client.js";

import Base from "./base.js";

const Exception = ExceptionFactory("websocket", "client");
const Log = LogFactory("websocket", "client");

export default class WebsocketClient extends Base {
	constructor(schema, options) {
		super(schema, options);
		this.endpoints = new Endpoints(Object.keys(schema));
	}

	/// Register and connect to a websocket
	async handle(endpoint, callback) {
		const updatedEndpoint = this.endpoints.match(endpoint);
		this._sanityCheck(updatedEndpoint);
		return await WebsocketClientImpl.handle(this.getEndpoint(endpoint), callback);
	}
}
