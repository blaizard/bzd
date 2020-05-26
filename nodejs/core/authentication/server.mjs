"use strict";

import ExceptionFactory from "../exception.mjs";

const Exception = ExceptionFactory("authentication", "server");

export default class AuthenticationServer {
    constructor(options, defaultOptions = {}) {
        this.options = Object.assign({
            /**
             * Callback to verify once identiy, mathing uid and password pair.
             */
            verifyIdentityCallback: null
        }, defaultOptions, options);
    }

    installAPI(api, web) {
        Exception.unreachable("'install' must be implemented.");
    }

    async verify(request, callback = (uid) => true) {
        Exception.unreachable("'verify' must be implemented.");
    }

    async verifyIdentity(uid, password) {
        Exception.assert(this.options.verifyIdentityCallback, "verifyIdentityCallback is not set.");
        return await this.options.verifyIdentityCallback(uid, password);
    }
}
