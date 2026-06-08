use embassy_executor::{Executor, Spawner};
use static_cell::StaticCell;

unsafe extern "Rust" {
    fn run_embassy(spawner: Spawner);
}

static EXECUTOR: StaticCell<Executor> = StaticCell::new();

pub fn exit(code: i32) -> ! {
    std::process::exit(code)
}

#[unsafe(no_mangle)]
fn main() {
    let executor = EXECUTOR.init(Executor::new());
    executor.run(|spawner| unsafe { run_embassy(spawner) });
}
