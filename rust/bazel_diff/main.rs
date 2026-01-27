use std::ffi::OsStr;
use std::io::{BufRead, BufReader};
use std::process::{Command, Stdio};

struct ExecuteResult {
    child: std::process::Child,
}

impl ExecuteResult {
    fn new(child: std::process::Child) -> Self {
        Self { child }
    }

    fn wait(&mut self) -> Result<(), String> {
        self.child
            .wait()
            .map_err(|e| format!("Failed to wait: {}", e))?;
        Ok(())
    }
}

struct LocalCommand<'lifetime, I, S>
where
    I: IntoIterator<Item = S>,
    S: AsRef<OsStr>,
{
    cmds: I,
    on_stdout: Option<Box<dyn FnMut() + 'lifetime>>,
}

impl<'lifetime, I, S> LocalCommand<'lifetime, I, S>
where
    I: IntoIterator<Item = S>,
    S: AsRef<OsStr>,
{
    fn new(cmds: I) -> Self {
        Self {
            cmds,
            on_stdout: None,
        }
    }

    fn stdout<F>(mut self, on_stdout: F) -> Self
    where
        F: FnMut() + 'lifetime,
    {
        self.on_stdout = Some(Box::new(on_stdout));
        self
    }

    fn spawn(self) -> Result<ExecuteResult, String> {
        let mut cmds_iter = self.cmds.into_iter();
        let cmds_first = cmds_iter.next().ok_or("No command provided".to_string())?;

        let mut command = Command::new(cmds_first);
        command.args(cmds_iter);

        let mut child = command
            .stdout(Stdio::piped())
            .stderr(Stdio::piped())
            .spawn()
            .map_err(|e| format!("Failed to execute command: {}", e))?;

        let stdout = child.stdout.take().ok_or("Failed to open stdout")?;
        let reader = BufReader::new(stdout);

        for line in reader.lines() {
            let line = line.map_err(|e| format!("Failed to read line: {}", e))?;
            println!("{}", line);
        }

        if let Some(mut callback) = self.on_stdout {
            callback()
        }

        Ok(ExecuteResult::new(child))
    }

    fn run(self) -> Result<ExecuteResult, String> {
        let mut result = self.spawn()?;
        result.wait()?;
        Ok(result)
    }
}

fn main() {
    let mut a = 1;
    let command = LocalCommand::new(["bazel", "--version"]).stdout(|| {
        a += 1;
        println!("Received stdout from bazel command");
    });

    let result = command.run();

    println!("a={}", a);

    match result {
        Ok(_) => {}
        Err(e) => eprintln!("Error: {}", e),
    }

    /*match local_command(["bazel", "query", "--output", "label_kind", "//rust/..."]) {
        Ok(result) => println!("Success: {}", results),
        Err(e) => eprintln!("Error: {}", e),
    }*/
}
