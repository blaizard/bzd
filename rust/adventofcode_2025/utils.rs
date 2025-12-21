use std::env;
use std::fs::read_to_string;

pub fn file_to_vector(path: &str, delimiter: char, trim: bool) -> Vec<String> {
    read_to_string(path)
        .unwrap()
        .split(delimiter)
        .map(|s| {
            if trim {
                s.trim().to_string()
            } else {
                s.to_string()
            }
        })
        .filter(|line| !line.is_empty())
        .collect()
}

pub fn arg1_to_vector(delimiter: char, trim: bool) -> Vec<String> {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        panic!("Please provide a filename");
    }
    file_to_vector(&args[1], delimiter, trim)
}
