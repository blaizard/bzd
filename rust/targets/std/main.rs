unsafe extern "Rust" {
    fn run_executor() -> bool;
}

#[unsafe(no_mangle)]
fn main() -> i32 {
    println!("Main started!");
    let _ = unsafe { run_executor() };
    println!("Main stopped!");

    0
}
