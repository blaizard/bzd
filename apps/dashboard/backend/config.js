"use strict";

const PersistenceDisk = require("../../../nodejs/require/persistence/disk.js");

/**
 * Store all configuration for the dashboard
 */
module.exports = class Config extends PersistenceDisk {
	constructor() {
		super(".");
	}
};
