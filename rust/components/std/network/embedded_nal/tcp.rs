use bzd::base::error::{failure, Error};
use bzd::base::stream::{IStream, OStream};
use core::net::SocketAddr;
use embedded_io_async::{Read, Write};
use embedded_nal_async::TcpConnect;
use rust_interfaces_network::BzdTcpInterface;
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

impl BzdTcpInterface for BzdComponentsStdNetworkNalTcp {
    type Stream<'a>
        = TcpStream<<Stack as TcpConnect>::Connection<'a>>
    where
        Self: 'a;

    async fn connect<'a>(&'a self, address: SocketAddr) -> Result<Self::Stream<'a>, Error> {
        let connection = self
            .stack
            .connect(address)
            .await
            .map_err(|_| failure("failed to connect TCP socket"))?;
        Ok(TcpStream::new(connection))
    }
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
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Error> {
        self.connection
            .read(buffer)
            .await
            .map_err(|_| bzd::base::error::failure("TCP read failed"))
    }
}

impl<C: Write> OStream for TcpStream<C> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
        self.connection
            .write(data)
            .await
            .map_err(|_| bzd::base::error::failure("TCP write failed"))
    }

    async fn flush(&mut self) -> Result<(), Error> {
        self.connection
            .flush()
            .await
            .map_err(|_| bzd::base::error::failure("TCP flush failed"))
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
        assert_eq!(stream.read(&mut buffer).await?, 5)?;
        assert_eq!(&buffer, b"hello")?;
        stream.write_all(b"world").await?;
        assert_eq!(&stream.connection.output, b"world\0\0\0")?;
        Ok(())
    }

    #[test]
    fn test_new() -> TestResult {
        let _tcp = BzdComponentsStdNetworkNalTcp::new();
        Ok(())
    }
}
