import Format from "./format.mjs";

class Performance {
	constructor() {
		this.timeStart = Date.now();
	}

	toString() {
		const timeMs = Date.now() - this.timeStart;
		return timeMs > 1000 ? timeMs / 1000 + "s" : timeMs + "ms";
	}
}

function consoleProcessor(date, level, topics, message) {
	let content = Format("[{}] [{}] ", date.toISOString().replace("Z", "").replace("T", " "), level);
	if (topics) {
		content += Format("[{}] ", topics.join("::"));
	}
	content += message;
	({
		error: console.error,
		warning: console.warn,
		info: console.info,
		debug: console.log,
		trace: console.trace,
	})[level](content);
}

class Logger {
	constructor() {
		this.processors = {};
		this.addBackend("console", consoleProcessor);
	}

	static get levels() {
		return {
			trace: 5,
			debug: 4,
			info: 3,
			warning: 4,
			error: 1,
		};
	}

	get config() {
		return Object.fromEntries(
			Object.entries(this.processors).map(([name, data]) => {
				return [
					name,
					{
						setLevel: (level) => {
							data.level = Logger.levels[level];
						},
						mute: (level) => {
							data.level = Logger.levels["error"];
						},
					},
				];
			}),
		);
	}

	// Support traces
	process(level, topics, str = "", ...args) {
		const date = new Date();
		const logLevel = Logger.levels[level];
		let message = null;
		for (const processor of Object.values(this.processors)) {
			if (logLevel <= processor.level) {
				if (message === null) {
					message = Format(String(str), ...args);
				}
				processor.process(date, level, topics, message);
			}
		}
	}

	/// Add a new backend to the logger.
	addBackend(name, processor, level = "info") {
		this.processors[name] = {
			level: Logger.levels[level],
			process: processor,
		};
	}
}

// Singleton
let logger = new Logger({
	level: Logger.levels.info.level,
});

const custom = (config, ...msgs) => {
	logger.process(config.level, config.topics, ...msgs);
};

const LoggerFactory = (...topics) => {
	return {
		trace: (...msgs) => custom({ level: "trace", topics: topics }, ...msgs),
		debug: (...msgs) => custom({ level: "debug", topics: topics }, ...msgs),
		info: (...msgs) => custom({ level: "info", topics: topics }, ...msgs),
		warning: (...msgs) => custom({ level: "warning", topics: topics }, ...msgs),
		error: (...msgs) => custom({ level: "error", topics: topics }, ...msgs),
		custom: (...args) => custom(...args),
		get config() {
			return logger.config;
		},
		performance() {
			return new Performance();
		},
		addBackend(name, level, processor) {
			logger.addBackend(name, level, processor);
		},
	};
};

export default LoggerFactory;
