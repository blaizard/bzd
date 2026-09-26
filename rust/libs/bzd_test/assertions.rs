#[macro_export]
macro_rules! assert_eq {
    ($left:expr, $right:expr) => {{
        let left = $left;
        let right = $right;
        if left != right {
            Err(::bzd_test::assert_failed(concat!(
                "assertion failed: '(left == right)'\n",
                "  left: ",
                stringify!($left),
                "\n",
                " right: ",
                stringify!($right),
                "\n"
            )))
        } else {
            Ok(())
        }
    }};
}
