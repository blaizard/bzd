import ChildProcess from "child_process";

ChildProcess.execFileSync(process.execPath, ["../node_modules/.bin/prettier", ...process.argv.slice(2)], {
	stdio: "inherit",
});
