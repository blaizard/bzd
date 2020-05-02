import { spawn } from "child_process";

let ls = spawn(process.execPath, ["./node_modules/.bin/eslint", ...process.argv.slice(2)]);

ls.stdout.on("data", (data) => {
    process.stdout.write(data.toString());
});

ls.stderr.on("data", (data) => {
    process.stderr.write(data.toString());
});

ls.on("exit", (code) => {
    process.exit(code);
});
