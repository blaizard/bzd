"use strict";

import ExceptionFactory from "../exception.mjs";
import Jwt from "jsonwebtoken";

const Exception = ExceptionFactory("authentication", "server");

export default class AuthenticationServer {
    constructor(options) {
        this.options = Object.assign({
            privateKey: null,
            publicKey: null,
            /**
             * Time in seconds after which the access token will expire.
             */
            tokenAccessExpiresIn: 15 * 60,
            /**
             * Time in seconds after which the refresh token will expire.
             */
            tokenRefreshExpiresIn: 7 * 24 * 60 * 60,
            /**
             * Callback to verify once identiy, mathing uid and password pair.
             */
            verifyIdentityCallback: null
        }, options);
    }

    async _generateToken(data, expiresIn) {
        Exception.assert(this.options.privateKey, "The private key is not set.");
        return new Promise((resolve, reject) => {
            Jwt.sign(data, this.options.privateKey, { expiresIn: expiresIn }, (e, token) => {
                if (e) {
                    reject(e);
                }
                else {
                    resolve({
                        token: token,
                        expiresIn: expiresIn
                    });
                }
            });
        });
    }

    async verifyToken(token, verifyCallback = () => true) {
        try {
            const data = await this.readToken(token);
            return await verifyCallback(data);
        }
        catch (e) {
            return false;
        }
    }

    async verifyIdentity(uid, password) {
        Exception.assert(this.options.verifyIdentityCallback, "verifyIdentityCallback is not set.");
        return await this.options.verifyIdentityCallback(uid, password);
    }

    async generateAccessToken(data) {
        return await this._generateToken(data, this.options.tokenAccessExpiresIn);
    }

    async generateRefreshToken(data) {
        return await this._generateToken(data, this.options.tokenRefreshExpiresIn);
    }

    async readToken(token) {
        Exception.assert(this.options.publicKey || this.options.privateKey, "A key must be set.");
        return new Promise((resolve, reject) => {
            Jwt.verify(token, this.options.publicKey || this.options.privateKey, (e, data) => {
                if (e) {
                    reject(e);
                }
                else {
                    resolve(data);
                }
            });
        });
    }
}
