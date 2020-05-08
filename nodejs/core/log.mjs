"use strict";

import Format from "./format.mjs";

class Performance {
	constructor() {
		this.timeStart = Date.now();
	}

	toString() {
		const timeMs = (Date.now() - this.timeStart);
		return ((timeMs > 1000) ? (timeMs / 1000 + "s") : (timeMs + "ms"));
	}
}

class Logger {
	constructor(config) {
		this.config = Object.assign({
			level: Logger.levels.info.level
		}, config);
	}

	static get levels() {
		return {
			// eslint-disable-next-line no-console
			debug: {level: 4, text: "debug", fct: console.log},
			// eslint-disable-next-line no-console
			info: {level: 3, text: "info", fct: console.info},
			// eslint-disable-next-line no-console
			warning: {level: 2, text: "warning", fct: console.warn},
			// eslint-disable-next-line no-console
			error: {level: 1, text: "error", fct: console.error}
		};
	}

	// Configuration options
	setLevel(level) {
		this.config.level = Logger.levels[level].level;
	}

	// Support traces
	print(level, topics, str = "", ...args) {
		const log = Logger.levels[level];
		if (log.level <= this.config.level) {
			const date = new Date().toISOString();
			let message = Format("[{}] [{}] ", date.replace("Z", "").replace("T", " "), log.text);
			if (topics) {
				message += Format("[{}] ", topics.join("::"));
			}
			message += Format(String(str), ...args);
			log.fct(message);
		}
	}
}

// Singleton
let logger = new Logger({
	level: Logger.levels.info.level
});

const custom = (config, ...msgs) => {
	logger.print(config.level, config.topics, ...msgs);
};

const LoggerFactory = (...topics) => {
	return {
		debug: (...msgs) => custom({level: "debug", topics: topics}, ...msgs),
		info: (...msgs) => custom({level: "info", topics: topics}, ...msgs),
		warning: (...msgs) => custom({level: "warning", topics: topics}, ...msgs),
		error: (...msgs) => custom({level: "error", topics: topics}, ...msgs),
		custom: (...args) => custom(...args),
		setMinLevel(level) {
			logger.setLevel(level);
		},
		mute() {
			logger.setLevel("error");
		},
		performance() {
			return new Performance();
		}
	};
};

export default LoggerFactory;
