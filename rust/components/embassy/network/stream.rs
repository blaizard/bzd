use bzd::base::error::Error;
use bzd::base::stream::{IStream, OStream};
use embedded_io_async::{Read, Write};

pub struct TcpStream<C> {
    connection: C,
}

impl<C> TcpStream<C> {
    pub fn new(connection: C) -> Self {
        Self { connection }
    }
}

impl<C: Read> IStream for TcpStream<C> {
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Error> {
        self.connection
            .read(buffer)
            .await
            .map_err(|error| bzd::base::error::failure!("TCP read failed: {error}"))
    }
}

impl<C: Write> OStream for TcpStream<C> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
        self.connection
            .write(data)
            .await
            .map_err(|error| bzd::base::error::failure!("TCP write failed: {error}"))
    }

    async fn flush(&mut self) -> Result<(), Error> {
        self.connection
            .flush()
            .await
            .map_err(|error| bzd::base::error::failure!("TCP flush failed: {error}"))
    }
}
