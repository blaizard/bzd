use bzd::base::stream::OStream;
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

impl OStream for BzdComponentsStdOut {
    async fn write(&mut self, data: &[u8]) -> Result<usize, bzd::base::error::Error> {
        let mut stdout = io::stdout();
        stdout
            .write(data)
            .map_err(|_| bzd::base::error::failure("failed to write to stdout"))
    }

    async fn flush(&mut self) -> Result<(), bzd::base::error::Error> {
        let mut stdout = io::stdout();
        stdout
            .flush()
            .map_err(|_| bzd::base::error::failure("failed to flush stdout"))
    }
}
