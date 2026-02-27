#![no_std]
#![no_main]

use main as _;

#[cfg(test)]
#[bzd_test::test]
mod tests {
    #[test]
    fn test_dummy() {
        esp_println::println!("Init!");

        let expected = 1;
        let actual = 2;
        assert_eq!(expected, actual);
    }
}
