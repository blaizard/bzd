use interfaces_stream::BzdOStreamInterface;
use std::io::{self, Write};

pub struct BzdComponentsStdOut {}

impl BzdComponentsStdOut {
    pub fn new() -> Self {
        Self {}
    }
}

impl Default for BzdComponentsStdOut {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdOStreamInterface for BzdComponentsStdOut {
    async fn write(&mut self, data: &[u8]) -> Result<(), bzd::base::error::Error> {
        let mut stdout = io::stdout();
        if stdout.write_all(data).is_err() {
            return Err(bzd::base::error::failure("failed to write to stdout"));
        }
        if stdout.flush().is_err() {
            return Err(bzd::base::error::failure("failed to flush stdout"));
        }
        Ok(())
    }
}
