import bcrypt from "bcryptjs";

export default class BCrypt {
	static async hash(data) {
		return await bcrypt.hash(data, 10);
	}

	static async compare(hash, data) {
		return await bcrypt.compare(data, hash);
	}
}
