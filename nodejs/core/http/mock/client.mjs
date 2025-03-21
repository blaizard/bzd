import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import { HttpClientFactory } from "#bzd/nodejs/core/http/client.mjs";

const Exception = ExceptionFactory("http", "client", "mock");

export function makeMockHttpClientFactory(callback) {
	class MockHttpClientFactory extends HttpClientFactory {
		constructor(url, optionsOrCallback = {}) {
			super(url, optionsOrCallback);
			this.HttpClientClass = makeHttpClient(callback);
		}
	}

	return MockHttpClientFactory;
}

export function makeHttpClient(callback) {
	class MockHttpClient {
		static async request(url, options = {}) {
			return await callback(url, options);
		}

		static async get(url, options = {}) {
			options["method"] = "get";
			return await MockHttpClient.request(url, options);
		}

		static async post(url, options = {}) {
			options["method"] = "post";
			return await MockHttpClient.request(url, options);
		}
	}

	return MockHttpClient;
}
