use std::env;
use std::fs;

struct Config {
    query: String,
    file_path: String,
}

impl Config {
    fn build<T: Iterator<Item = String>>(args: T) -> Result<Config, String> {
        let args: Vec<String> = args.collect();

        if args.len() != 3 {
            return Err(format!(
                "Wrong number of arguments, provided {}; Usage: grep <query> <file_path>",
                args.len() - 1
            ));
        }
        Ok(Config {
            query: args[1].clone(),
            file_path: args[2].clone(),
        })
    }
}

fn run(config: Config) -> Result<(), String> {
    let contents = fs::read_to_string(&config.file_path)
        .map_err(|e| format!("Error reading file '{}': {}", &config.file_path, e))?;

    for line in lib::search(&config.query, &contents) {
        println!("{}", line);
    }

    Ok(())
}

fn main() {
    let config = Config::build(env::args()).expect("Parsing argument failed");

    println!(
        "Searching for '{}' in file '{}'",
        config.query, config.file_path
    );

    run(config).expect("Application error");
}
