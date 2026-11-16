use embassy_executor::{Executor, Spawner};
use static_cell::StaticCell;

use composition::runExecutor;

static EXECUTOR: StaticCell<Executor> = StaticCell::new();

#[embassy_executor::task]
async fn main_task() {
    match runExecutor().await {
        Ok(()) => {
            if let Some(request) = rust_libs_power::power().deep_sleep_requested().await {
                if let Some(duration) = request.duration() {
                    println!("deep sleep for {duration:?}");
                    std::thread::sleep(duration);
                }
            }
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
