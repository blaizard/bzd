"use strict";

import Http from "http";
import Https from "https";
import ExceptionFactory from "../../exception.js";

const Exception = ExceptionFactory("fetch", "node.http");

export default async function request(url, method, headers, body, options) {
    console.log("Calling HTTPPPP");
}
