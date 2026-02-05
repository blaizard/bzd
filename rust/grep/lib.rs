pub fn search<'a>(query: &str, contents: &'a str) -> Vec<&'a str> {
    let mut results = Vec::new();
    for line in contents.lines() {
        if line.contains(query) {
            results.push(line);
        }
    }
    results
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_search() {
        let query = "duct";
        let contents = "this is the first line\na duct tape\nanother line with duct\nlast line";
        let result = search(query, contents);
        assert_eq!(result, vec!["a duct tape", "another line with duct"]);
    }
}
