"use strict";

import ExceptionFactory from "../exception.mjs";

const Exception = ExceptionFactory("api");

export default class API {
	constructor(schema, options) {
        this.options = Object.assign({
            /**
             * Authentication object to be used with this API.
             */
            authentication: null,
            /**
             * Version of the API to be used.
             */
            version: 1,
            /**
             * Channel to be used as a transportation mean for this API
             */
            channel: null,
            /**
             * Include additional plugins if any
             */
            plugins: []
        }, options);

        this.schema = schema;

        // Install all available plugins
        this.options.plugins.forEach((plugin) => {
            plugin.installAPI(this);
        });
	}

    /**
     * Add a schema to the existing schema.
     * Note this only update high level routes and do not deep merge.
     */
    addSchema(schema) {
        Object.assign(this.schema, schema);
    }

    /**
     * If authentication is supported
     */
    isAuthentication() {
        return (this.options.authentication !== null);
    }

	_makePath(endpoint) {
		return "/api/v" + this.options.version + endpoint;
	}

	/**
     * Ensure the requestis valid according to the API schema.
     */
	_sanityCheck(method, endpoint) {
		Exception.assert(endpoint in this.schema, "This endpoint is not defined: {}", endpoint);
		Exception.assert(method in this.schema[endpoint], "The method '{}' is not valid for endpoint '{}'", method, endpoint);
	}
}
