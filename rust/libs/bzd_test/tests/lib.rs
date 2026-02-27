#![no_std]
#![no_main]

mod extra_file;

#[derive(Debug)]
pub struct TestError {
    pub file: &'static str,
    pub line: u32,
    pub message: &'static str,
}

pub type TestResult = Result<(), TestError>;

#[cfg(test)]
macro_rules! bzd_assert_eq {
    ($left:expr, $right:expr) => {{
        let left = $left;
        let right = $right;
        if left != right {
            Err(TestError {
                file: file!(),
                line: line!(),
                message: concat!(
                    "assertion failed: '(left == right)'\n",
                    "  left: ",
                    stringify!($left),
                    "\n",
                    " right: ",
                    stringify!($right),
                    "\n"
                ),
            })
        } else {
            Ok(())
        }
    }};
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use bzd_test::tests::get_tests;
    use create::TestError;

    #[test]
    fn test_success() {
        bzd_assert_eq!(1, 1).unwrap();

        assert_eq!(1, 1);
    }

    #[test]
    #[ignore]
    fn test_fail() {
        assert_eq!(1, 2);
    }

    #[test]
    fn test_nb_tests() {
        let tests = get_tests();
        assert_eq!(tests.len(), 4);
    }
}
