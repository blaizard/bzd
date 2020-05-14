"use strict";

import ExceptionFactory from "bzd/core/exception.mjs";

const Exception = ExceptionFactory("authentication");

export default class Authentication {
    constructor(options) {
        this.options = Object.assign({
			/**
			 * Callback that will be triggered if the user attempt to access
			 * a route that requires authentication without it.
			 */
            unauthorizedCallback: null,
            /**
             * Callback to refresh an invalid token.
             * Returns a dictionary with the token and refresh_token keys.
             * Returns null in case the refresh got invalidated.
             */
            refreshTokenCallback: null
        }, options);

        // Token to be used during this session.
        this.token = null;
        this.tokenRefresh = null;
    }

    isAuthenticated() {
        return (this.token !== null);
    }

    invalidToken() {
        this.token = null;
        this.tokenRefresh = null;
    }

    setToken(token, tokenRefresh) {
        this.token = token;
        this.tokenRefresh = tokenRefresh;
    }

    getToken() {
        Exception.assert(this.isAuthenticated(), "Token is not set.");
        return this.token;
    }

    /**
     * Try to refresh token, this should be called after an unauthorized request.
     */
    async tryRefreshToken() {

        // Try to refresh the token
        if (this.tokenRefresh && this.options.refreshTokenCallback) {
            const result = await this.options.refreshTokenCallback(this.tokenRefresh);
            if (result) {
                Exception.assert("token" in result, "Missing token.");
                Exception.assert("refresh_token" in result, "Missing refresh token.");
                this.setToken(this.token, this.tokenRefresh);
                return;
            }
        }

        this.invalidToken();

        if (this.options.unauthorizedCallback) {
            await this.options.unauthorizedCallback();
        }
        Exception.unreachable("This route requires authentication.");
    }

    /**
     * This function must be called on a code path where authentication is required.
     */
    async assertAuthentication() {
        if (!this.token) {
            await this.tryRefreshToken();
        }
    }
};
