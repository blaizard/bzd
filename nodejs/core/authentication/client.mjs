"use strict";

import ExceptionFactory from "../exception.mjs";

const Exception = ExceptionFactory("authentication", "client");

export default class AuthenticationClient {
    constructor(options, defaultOptions = {}) {
        this.options = Object.assign({
			/**
			 * Callback that will be triggered if the user attempt to access
			 * a route that requires authentication without it.
			 */
            unauthorizedCallback: null,
            /**
             * Callback to be called each time the authentication status changes.
             */
            onAuthentication: (/*isAuthenticated*/) => {}
        }, defaultOptions, options);
    }

    installAPI(api) {
        Exception.unreachable("'install' must be implemented.");
    }

    /**
     * Return true if the client is authenticated, false otherwise.
     */
    isAuthenticated() {
        Exception.unreachable("'isAuthenticated' must be implemented.");
    }

    /**
     * Attempt to refresh an authentication
     */
    async refreshAuthentication() {
        Exception.unreachable("'refreshAuthentication' must be implemented.");
    }

    /**
     * Attempt to login
     */
    async login(api, uid, password, persistent = false) {
        Exception.unreachable("'login' must be implemented.");
    }

    /**
     * Attempt to logout
     */
    async logout(api) {
        Exception.unreachable("'logout' must be implemented.");
    }

    /**
     * Set authentication information to the fetch request
     */
    async setAuthentication(fetchOptions) {
        Exception.unreachable("'setAuthentication' must be implemented.");
    }
};
