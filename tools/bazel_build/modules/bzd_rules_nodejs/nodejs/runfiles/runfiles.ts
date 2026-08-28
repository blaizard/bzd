import * as fs from "fs";
import * as path from "path";

export function pathFromRLocation(rlocation: string): string {

	// Directory strategy.
	const maybeDirectory = process.env.RUNFILES_DIR;
	if (maybeDirectory) {
		const candidate = path.join(maybeDirectory, rlocation);
		if (fs.existsSync(candidate)) {
			return candidate;
		}
	}

	// Manifest strategy (Windows / --enable_runfiles=false).
	const maybeManifestFile = process.env.RUNFILES_MANIFEST_FILE;
	if (maybeManifestFile && fs.existsSync(maybeManifestFile)) {
		const lines = fs.readFileSync(maybeManifestFile, "utf8").split("\n");
		for (const line of lines) {
			const index = line.indexOf(" ");
			if (index == -1) {
				continue;
			}
			if (line.substring(0, index) === rlocation) {
				return line.substring(index + 1);
			}
		}
	}

	throw new Error(`runfile not found: ${rlocation}.`);
}
