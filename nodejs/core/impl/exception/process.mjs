export default function uncaughtExceptionHandler(ExceptionFactory) {
	process.on("uncaughtException", (e) => {
		ExceptionFactory.fromError(e).print("Exception: uncaughtException");
		process.exit(1);
	});
	process.on("unhandledRejection", (reason, promise) => {
		ExceptionFactory.fromError(reason).print("Exception: unhandledRejection, promise: '{}'", promise);
		process.exit(2);
	});
}
