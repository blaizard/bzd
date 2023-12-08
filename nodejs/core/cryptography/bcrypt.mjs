import bcrypt from "bcryptjs";

export default class BCrypt {
	static async hash(data) {
		return new Promise((resolve, reject) => {
			bcrypt.hash(data, 10, (err, hash) => {
				if (err || !hash) {
					reject(err || "no hash generated");
				} else {
					resolve(hash);
				}
			});
		});
	}

	static async compare(hash, data) {
		return await bcrypt.compare(data, hash);
	}
}
