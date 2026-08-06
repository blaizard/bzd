use interface::MyInterface;

#[allow(dead_code)]
struct Calculator;

impl MyInterface for Calculator {
    fn add(&self, a: i32, b: i32) -> i32 {
        a + b
    }
}
