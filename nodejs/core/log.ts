import Format from "#bzd/nodejs/core/format.js";
// @ts-expect-error: rate_limit is a plain JS module without type declarations.
import RateLimiter from "#bzd/nodejs/core/rate_limit.js";

class Performance {
    timeStart: number;

    constructor() {
        this.timeStart = Date.now();
    }

    toString() {
        const timeMs = Date.now() - this.timeStart;
        return timeMs > 1000 ? timeMs / 1000 + "s" : timeMs + "ms";
    }
}

function consoleProcessor(date: Date, level: string, topics: string[] | null, message: string) {
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
    } as Record<string, any>)[level](content);
}

class Logger {
    processors: Record<string, any> = {};
    rateLimiter_: RateLimiter | null = null;
    skippedCount_: Map<string, number> = new Map();

    constructor(_options?: any) {
        this.addBackend("console", consoleProcessor);
    }

    // Configure the rate limiter used to group occurrences of the same log.
    setRateLimiter(cache: any, options?: any) {
        const rateLimitOptions = options ?? {};
        this.rateLimiter_ = new RateLimiter(cache, {
            bucket: "log",
            threshold: rateLimitOptions.threshold ?? 2,
            windowMs: rateLimitOptions.windowMs ?? 5000,
            clock: rateLimitOptions.clock ?? { getTimeMs: () => Date.now() },
        });
    }

    static get levels(): Record<string, number> {
        return {
            trace: 5,
            debug: 4,
            info: 3,
            warning: 2,
            error: 1,
        };
    }

    get config() {
        return Object.fromEntries(
            Object.entries(this.processors).map(([name, data]) => {
                return [
                    name,
                    {
                        setLevel: (level: string) => {
                            data.level = Logger.levels[level];
                        },
                        mute: (_level?: string) => {
                            data.level = Logger.levels["error"];
                        },
                    },
                ];
            }),
        );
    }

    // Build the identity of a log, used to group occurrences of the same log.
    static makeKey(level: string, topics: string[] | undefined, str: any): string {
        return [level, ...(topics ?? []), String(str)].join("\0");
    }

    // Process the message through all configured backends.
    process(level: string, topics?: string[], str: any = "", ...args: any[]) {
        const date = new Date();
        const logLevel = Logger.levels[level];
        let message: string | null = null;
        for (const processor of Object.values(this.processors)) {
            if (logLevel <= processor.level) {
                if (message === null) {
                    message = Format(String(str), ...args);
                    if (this.rateLimiter_ !== null) {
                        const key = Logger.makeKey(level, topics, message);
                        // Rate limiting: only print a given log a few times per window and count the suppressed ones.
                        if (this.rateLimiter_.isOverLimit(key)) {
                            this.skippedCount_.set(key, (this.skippedCount_.get(key) ?? 0) + 1);
                            return;
                        }
                        void this.rateLimiter_.record(key);
                        const skippedCount = this.skippedCount_.get(key) ?? 0;
                        if (skippedCount > 0) {
                            message = Format("[skipped {} logs] ", skippedCount) + message;
                            this.skippedCount_.delete(key);
                        }
                    }
                }
                processor.process(date, level, topics, message);
            }
        }
    }

    /// Add a new backend to the logger.
    addBackend(name: string, processor: any, level: string = "info") {
        this.processors[name] = {
            level: Logger.levels[level],
            process: processor,
        };
    }
}

// Singleton
let logger = new Logger({
    level: Logger.levels.info,
});

const custom = (config: any, ...msgs: any[]) => {
    logger.process(config.level, config.topics, ...msgs);
};

const LoggerFactory = (...topics: string[]) => {
    return {
        trace: (...msgs: any[]) => custom({ level: "trace", topics: topics }, ...msgs),
        debug: (...msgs: any[]) => custom({ level: "debug", topics: topics }, ...msgs),
        info: (...msgs: any[]) => custom({ level: "info", topics: topics }, ...msgs),
        warning: (...msgs: any[]) => custom({ level: "warning", topics: topics }, ...msgs),
        error: (...msgs: any[]) => custom({ level: "error", topics: topics }, ...msgs),
        custom: (...args: [any, ...any[]]) => custom(...args),
        get config() {
            return logger.config;
        },
        performance() {
            return new Performance();
        },
        addBackend(name: string, level: any, processor?: any) {
            logger.addBackend(name, level, processor);
        },
        /// Configure the rate limiter of the global logger.
        setRateLimiter(cache: any, options?: any) {
            logger.setRateLimiter(cache, options);
        },
    };
};

export default LoggerFactory;