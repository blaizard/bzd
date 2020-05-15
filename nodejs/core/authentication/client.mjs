"use strict";

import ExceptionFactory from "../exception.mjs";
import ExceptionFetch from "../impl/fetch/exception.mjs";

const Exception = ExceptionFactory("authentication", "client");

export default class AuthenticationClient {
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
    }

    isAuthenticated() {
        return (this.token !== null);
    }

    invalidToken() {
        this.token = null;
    }

    setToken(token) {
        this.token = token;
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
        if (this.options.refreshTokenCallback) {
            console.log("refreshTokenCallback");

            try {
                const result = await this.options.refreshTokenCallback();
                console.log(result);
                if (result) {
                    Exception.assert("token" in result, "Missing token.");
                    this.setToken(result.token);
                    return true;
                }
            }
            catch (e) {
                if (e.code != 403/*Forbidden*/) {
                    throw e;
                }
            }
        }

        this.invalidToken();

        if (this.options.unauthorizedCallback) {
            await this.options.unauthorizedCallback();
        }

        return false;
    }

    /**
     * This function must be called on a code path where authentication is required.
     */
    async assertAuthentication() {
        if (!this.token) {
            console.log("tryRefreshToken");
            await this.tryRefreshToken();
        }
    }
};
