use bzd::base::error::Error;
use bzd::base::stream::{IStream, OStream};
use core::net::SocketAddr;
use embedded_io_async::{Read, Write};
use embedded_nal_async::TcpConnect;
use interfaces_tcp::BzdNetworkTcpClientInterface;
use std_embedded_nal_async::Stack;

pub struct BzdComponentsStdNetworkNalTcp {
    stack: Stack,
}

impl BzdComponentsStdNetworkNalTcp {
    pub fn new() -> Self {
        Self {
            stack: Stack::default(),
        }
    }
}

impl Default for BzdComponentsStdNetworkNalTcp {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdNetworkTcpClientInterface for BzdComponentsStdNetworkNalTcp {
    type Stream = TcpStream<<Stack as TcpConnect>::Connection<'static>>;

    async fn connect(&self, address: SocketAddr) -> Result<Self::Stream, Error> {
        let connection = self
            .stack
            .connect(address)
            .await
            .map_err(|error| map_error("failed to connect TCP socket", error))?;
        Ok(TcpStream::new(connection))
    }
}

fn map_error<E: core::fmt::Display>(context: &str, error: E) -> Error {
    bzd::base::error::failure!("{context}: {error}")
}

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
            .map_err(|error| map_error("TCP read failed", error))?;
        Ok(&buffer[..size])
    }
}

impl<C: Write> OStream for TcpStream<C> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
        self.connection
            .write(data)
            .await
            .map_err(|error| map_error("TCP write failed", error))
    }

    async fn flush(&mut self) -> Result<(), Error> {
        self.connection
            .flush()
            .await
            .map_err(|error| map_error("TCP flush failed", error))
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    struct MockConnection {
        input: &'static [u8],
        output: [u8; 8],
        length: usize,
    }

    impl MockConnection {
        fn new(input: &'static [u8]) -> Self {
            Self {
                input,
                output: [0; 8],
                length: 0,
            }
        }
    }

    impl embedded_io_async::ErrorType for MockConnection {
        type Error = std::io::Error;
    }

    impl embedded_io_async::Read for MockConnection {
        async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, std::io::Error> {
            let size = buffer.len().min(self.input.len());
            buffer[..size].copy_from_slice(&self.input[..size]);
            self.input = &self.input[size..];
            Ok(size)
        }
    }

    impl embedded_io_async::Write for MockConnection {
        async fn write(&mut self, data: &[u8]) -> Result<usize, std::io::Error> {
            let size = data.len().min(self.output.len() - self.length);
            self.output[self.length..self.length + size].copy_from_slice(&data[..size]);
            self.length += size;
            Ok(size)
        }

        async fn flush(&mut self) -> Result<(), std::io::Error> {
            Ok(())
        }
    }

    #[test]
    async fn test_stream_roundtrip() -> TestResult {
        let mut stream = TcpStream::new(MockConnection::new(b"hello"));
        let mut buffer = [0u8; 5];
        let chunk = stream.read(&mut buffer).await?;
        assert_eq!(chunk, &b"hello"[..])?;
        stream.write_all(b"world").await?;
        assert_eq!(&stream.connection.output, b"world\0\0\0")?;
        Ok(())
    }

    #[test]
    fn test_new() -> TestResult {
        let _tcp = BzdComponentsStdNetworkNalTcp::new();
        Ok(())
    }

    #[test]
    fn test_map_error() -> TestResult {
        let error = map_error(
            "TCP read failed",
            std::io::Error::new(std::io::ErrorKind::ConnectionReset, "connection reset"),
        );
        assert_eq!(error.message(), "TCP read failed: connection reset")?;
        Ok(())
    }
}
