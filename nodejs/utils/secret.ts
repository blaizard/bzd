import { spawnSync } from "child_process";
import * as fs from "fs";
import * as os from "os";
import * as path from "path";

import { age } from "#bzd/nodejs/utils/config.js";
import { pathFromRLocation } from "#bzd/nodejs/utils/runfiles.js";

const AGE_PATH = pathFromRLocation(age);

/// Decrypt the given payload.
///
/// \param payload The payload to decrypt.
/// \param keyFile The key file to be used.
/// \return the decrypted payload.
export function decrypt(payload: string, keyFile?: string): string {
	const secret = JSON.parse(payload).secret;
	const keyFilePath = resolveKeyFile(keyFile);
	const result = spawnSync(AGE_PATH, ["--decrypt", "--identity", keyFilePath], {
		input: secret,
		encoding: "utf8",
	});
	if (result.status !== 0) {
		throw new Error(result.stderr);
	}
	return result.stdout;
}

function resolveKeyFile(keyFile?: string): string {
	if (keyFile) {
		return keyFile;
	}
	const envKeyFile = process.env.BZD_KEY_FILE;
	if (envKeyFile && fs.existsSync(envKeyFile)) {
		return envKeyFile;
	}
	const homeKeyFile = path.join(os.homedir(), ".bzd", "key.txt");
	if (fs.existsSync(homeKeyFile)) {
		return homeKeyFile;
	}
	throw new Error(
		"No key file found, searched in:\n - Environment variable 'BZD_KEY_FILE'.\n - Path at '" +
			homeKeyFile +
			"'.",
	);
}
