import StorageDisk from "#bzd/nodejs/db/storage/disk.mjs";

export default {
	async storage(params) {
		return await StorageDisk.make(params["fs.root"], {
			mustExists: true,
		});
	},
};
