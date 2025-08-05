import { ExceptionFactory } from "../exception.mjs";
import LogFactory from "../log.mjs";
import Endpoints from "#bzd/nodejs/core/http/endpoints.mjs";

import Base from "./base.mjs";

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

		return new Promise((resolve, reject) => {
			let socket = new WebSocket(this.getEndpoint(endpoint));
			socket.onopen = () => {
				resolve(socket);
			};
			socket.onmessage = async (event) => {
				await callback(event.data);
			};
			socket.onclose = (object) => {
				Exception.assert(
					object.code == 1000,
					"Endpoint '{}' disconnected with code {} and reason: {}.",
					endpoint,
					object.code,
					object.reason,
				);
			};
			socket.onerror = (error) => {
				reject(error);
			};
		});
	}
}
