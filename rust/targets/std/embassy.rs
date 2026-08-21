use embassy_executor::{Executor, Spawner};
use static_cell::StaticCell;

use composition::executor;

static EXECUTOR: StaticCell<Executor> = StaticCell::new();

#[embassy_executor::task]
async fn main_task() {
    let result = executor().await;
    println!("executor() completed with {result}");
    exit(0);
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
