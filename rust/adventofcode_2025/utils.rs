use std::env;
use std::fs::read_to_string;

pub fn file_to_vector(path: &str) -> Vec<String> {
    read_to_string(path)
        .unwrap()
        .lines()
        .filter(|line| !line.is_empty())
        .map(String::from)
        .collect()
}

pub fn arg1_to_vector() -> Vec<String> {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        panic!("Please provide a filename");
    }
    file_to_vector(&args[1])
}
