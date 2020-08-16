import StorageDisk from "bzd/db/storage/disk.mjs";

export default {
	storage(params) {
		return new StorageDisk(params["fs.root"]);
	}
};
