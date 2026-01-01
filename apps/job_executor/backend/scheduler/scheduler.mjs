import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import { Status } from "#bzd/nodejs/utils/run.mjs";
import ServicesProvider from "#bzd/nodejs/core/services/provider.mjs";

const Exception = ExceptionFactory("scheduler");
const Log = LogFactory("scheduler");

export default class Scheduler {
	constructor(executors, options) {
		this.executors = executors;
		this.options = Object.assign(
			{
				services: new ServicesProvider(),
			},
			options,
		);
		this.infos = {};

		this.options.services.addTimeTriggeredProcess(
			"scheduler.monitoring",
			async () => {
				const uids = Object.keys(this.executors);
				const infoList = await Promise.all(uids.map((uid) => this.executors[uid].getInfo()));
				const infos = Object.fromEntries(infoList.map((info, index) => [uids[index], info]));
				this.statuses = Object.fromEntries(Object.entries(infos).map(([uid, info]) => [uid, info.status]));

				// Loop through the executors ready to be scheduled.
				for (const [uid, status] of Object.entries(this.statuses)) {
					if (status == Status.idle && this.isReadyToBeExecuted(uid)) {
						await this._execute(uid);
						this.statuses[uid] = Status.running;
					}
				}

				return this.statuses;
			},
			{
				periodS: 5,
			},
		);
	}

	/// Schedule a new workload.
	///
	/// Scheduler options are:
	/// - immediately
	/// - queued
	/// - periodically <time>
	async schedule(uid, type) {
		const executor = this._getExecutor(uid);
		await executor.updateInfo({
			scheduler: {
				type: type,
			},
		});
	}

	/// Check if this uid is ready to be executed.
	isReadyToBeExecuted(uid) {
		return true;
	}

	_getExecutor(uid) {
		Exception.assertPrecondition(uid in this.executors, "Undefined job id '{}'.", uid);
		return this.executors[uid];
	}

	async _execute(uid) {
		const executor = this._getExecutor(uid);
		const info = await executor.getInfo();
		Exception.assertPrecondition(info.status == Status.idle, "The job '{uid}' has already started", uid);
		Exception.assert("args" in info, "The job '{}' is missing args: {:j}", uid, info);
		await executor.execute(info.args);
	}

	async terminate(uid, { force = false } = {}) {
		const executor = this._getExecutor(uid);
		const info = await executor.getInfo();
		Exception.assertPrecondition(force || info.status == Status.running, "The job '{}' is not running.", uid);
		if (executor.kill) {
			await executor.kill();
		}
	}
}
