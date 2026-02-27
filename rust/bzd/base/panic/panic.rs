#![no_std]

use core::fmt;

pub struct PanicPrint<'a> {
    pub info: &'a core::panic::PanicInfo<'a>,
}

const RESET: &str = "\x1b[0m";
const RED: &str = "\x1b[31m";

impl<'a> fmt::Display for PanicPrint<'a> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}{}{}", RED, self.info, RESET)
    }
}
