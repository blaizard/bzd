// Provide the critical section implementation on std platforms, where no
// hardware critical section is available.
use critical_section as _;

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
