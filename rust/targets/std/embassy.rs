use embassy_executor::{Executor, Spawner};
use static_cell::StaticCell;

use composition::runExecutor;

static EXECUTOR: StaticCell<Executor> = StaticCell::new();

#[embassy_executor::task]
async fn main_task() {
    match runExecutor().await {
        Ok(()) => {
            println!("executor() completed successfully");
            exit(0);
        }
        Err(e) => {
            println!("executor() failed: {e}");
            exit(1);
        }
    }
}

pub fn exit(code: i32) -> ! {
    std::process::exit(code)
}

fn run_executor(spawner: Spawner) {
    spawner.spawn(main_task().unwrap());
}

fn main() {
    let executor = EXECUTOR.init(Executor::new());
    executor.run(|spawner| run_executor(spawner));
}
