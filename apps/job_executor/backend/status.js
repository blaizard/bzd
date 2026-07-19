// Copy of nodejs/utils/run.js with some addition.
const StatusJobExecutor = Object.freeze({
	idle: "idle",
	running: "running",
	failed: "failed",
	completed: "completed",
	cancelled: "cancelled",
	// Addition.
	terminated: "terminated",
});

export default StatusJobExecutor;
