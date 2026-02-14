pub fn search<'a>(query: &'a str, contents: &'a str) -> impl Iterator<Item = &'a str> {
    contents.lines().filter(move |line| line.contains(query))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_search() {
        let query = "duct";
        let contents = "this is the first line\na duct tape\nanother line with duct\nlast line";
        let result = search(query, contents);
        assert_eq!(
            result.collect::<Vec<_>>(),
            vec!["a duct tape", "another line with duct"]
        );
    }
}
