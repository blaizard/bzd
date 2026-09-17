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
    async fn write(&mut self, data: &[u8]) {
        let mut stdout = io::stdout();
        let _ = stdout.write_all(data);
        let _ = stdout.flush();
    }
}
