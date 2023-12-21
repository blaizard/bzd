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

function consoleProcessor(date, level, topics, str = "", ...args) {
	let message = Format("[{}] [{}] ", date.toISOString().replace("Z", "").replace("T", " "), level);
	if (topics) {
		message += Format("[{}] ", topics.join("::"));
	}
	message += Format(String(str), ...args);
	({
		error: console.error,
		warning: console.warn,
		info: console.info,
		debug: console.log,
		trace: console.trace,
	})[level](message);
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
		for (const processor of Object.values(this.processors)) {
			if (logLevel <= processor.level) {
				processor.process(date, level, topics, str, ...args);
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
