unsafe extern "Rust" {
    fn run_executor() -> bool;
}

#[unsafe(no_mangle)]
fn main() {
    println!("Main started!");
    let _ = unsafe { run_executor() };
    println!("Main stopped!");

    std::process::exit(0);
}
