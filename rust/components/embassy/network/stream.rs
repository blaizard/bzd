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
    type Chunk<'a>
        = &'a [u8]
    where
        Self: 'a;

    async fn read<'a>(&'a mut self, buffer: &'a mut [u8]) -> Result<Self::Chunk<'a>, Error> {
        let size = self
            .connection
            .read(buffer)
            .await
            .map_err(|error| bzd::base::error::failure!("TCP read failed: {error}"))?;
        Ok(&buffer[..size])
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
