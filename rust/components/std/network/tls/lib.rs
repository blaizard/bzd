#![no_main]
#![allow(async_fn_in_trait)]

use bzd::base::error::Error;
use bzd::base::stream::embedded_io_async::{ErrorType, Read, Write};
use bzd::base::stream::{EmbeddedError, IStream, OStream};
use core::mem::ManuallyDrop;
use core::net::SocketAddr;
use embedded_tls::pki::CertVerifier;
use embedded_tls::{
    Aes128GcmSha256, Certificate, CryptoProvider, CryptoRngCore, TlsClock, TlsConfig,
    TlsConnection, TlsContext, TlsError, TlsVerifier,
};
use interfaces_tcp::BzdNetworkTcpClientInterface;
use rand_core::{CryptoRng, RngCore};
use rust_components_std_network_tls_interface::{
    BzdComponentsStdNetworkTlsContext, BzdComponentsStdNetworkTlsContextConstraint,
    BzdComponentsStdNetworkTlsInterface,
};
use rust_interfaces_tls::BzdNetworkTlsClientInterface;

const READ_BUFFER_SIZE: usize = 16 * 1024;
const WRITE_BUFFER_SIZE: usize = 16 * 1024;
const CERTIFICATE_SIZE: usize = 8 * 1024;

/// Bidirectional TLS stream, exposed as a bzd stream.
pub struct TlsStream<C> {
    connection: C,
}

impl<C> TlsStream<C> {
    fn new(connection: C) -> Self {
        Self { connection }
    }
}

impl<C: Read<Error = TlsError>> IStream for TlsStream<C> {
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Error> {
        self.connection.read(buffer).await.map_err(map_tls_error)
    }
}

impl<C: Write<Error = TlsError>> OStream for TlsStream<C> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
        self.connection.write(data).await.map_err(map_tls_error)
    }

    async fn flush(&mut self) -> Result<(), Error> {
        self.connection.flush().await.map_err(map_tls_error)
    }
}

/// Owns a TLS connection together with its record buffers.
#[doc(hidden)]
pub struct OwnedConnection<S: Read + Write + 'static> {
    connection: ManuallyDrop<TlsConnection<'static, S, Aes128GcmSha256>>,
    read_buffer: *mut [u8],
    write_buffer: *mut [u8],
}

impl<S: Read + Write + 'static> OwnedConnection<S> {
    fn new(transport: S) -> Self {
        let read_buffer = Box::into_raw(vec![0u8; READ_BUFFER_SIZE].into_boxed_slice());
        let write_buffer = Box::into_raw(vec![0u8; WRITE_BUFFER_SIZE].into_boxed_slice());
        // SAFETY: both buffers are owned by this object and stay allocated until Drop,
        // which drops the connection before releasing the buffers.
        let read = unsafe { &mut *read_buffer };
        let write = unsafe { &mut *write_buffer };
        Self {
            connection: ManuallyDrop::new(TlsConnection::new(transport, read, write)),
            read_buffer,
            write_buffer,
        }
    }

    async fn open<P>(&mut self, context: TlsContext<'_, P>) -> Result<(), TlsError>
    where
        P: CryptoProvider<CipherSuite = Aes128GcmSha256>,
    {
        TlsConnection::open(&mut self.connection, context).await
    }
}

impl<S: Read + Write + 'static> ErrorType for OwnedConnection<S> {
    type Error = TlsError;
}

impl<S: Read + Write + 'static> Read for OwnedConnection<S> {
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Self::Error> {
        TlsConnection::read(&mut self.connection, buffer).await
    }
}

impl<S: Read + Write + 'static> Write for OwnedConnection<S> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Self::Error> {
        TlsConnection::write(&mut self.connection, data).await
    }

    async fn flush(&mut self) -> Result<(), Self::Error> {
        TlsConnection::flush(&mut self.connection).await
    }
}

impl<S: Read + Write + 'static> Drop for OwnedConnection<S> {
    fn drop(&mut self) {
        // SAFETY: the connection holds the only references to the buffers and is
        // dropped before them. Neither field is used after this point.
        unsafe {
            ManuallyDrop::drop(&mut self.connection);
            drop(Box::from_raw(self.read_buffer));
            drop(Box::from_raw(self.write_buffer));
        }
    }
}

/// Adapter turning a bzd stream into an owning embedded-io stream.
#[doc(hidden)]
pub struct Transport<S> {
    stream: S,
}

impl<S> Transport<S> {
    fn new(stream: S) -> Self {
        Self { stream }
    }
}

impl<S: IStream + OStream> ErrorType for Transport<S> {
    type Error = EmbeddedError;
}

impl<S: IStream + OStream> Read for Transport<S> {
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Self::Error> {
        self.stream.read(buffer).await.map_err(EmbeddedError::new)
    }
}

impl<S: IStream + OStream> Write for Transport<S> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Self::Error> {
        self.stream.write(data).await.map_err(EmbeddedError::new)
    }

    async fn flush(&mut self) -> Result<(), Self::Error> {
        self.stream.flush().await.map_err(EmbeddedError::new)
    }
}

/// Crypto provider backed by the operating system RNG and the system clock.
struct Provider<'a, R> {
    rng: R,
    verifier: CertVerifier<'a, Aes128GcmSha256, SystemClock, CERTIFICATE_SIZE>,
}

impl<'a, R> Provider<'a, R>
where
    R: CryptoRngCore,
{
    fn new(rng: R, ca: &'a [u8]) -> Self {
        Self {
            rng,
            verifier: CertVerifier::new(Certificate::X509(ca)),
        }
    }
}

impl<R: CryptoRngCore> CryptoProvider for Provider<'_, R> {
    type CipherSuite = Aes128GcmSha256;
    type Signature = [u8; 0];

    fn rng(&mut self) -> impl CryptoRngCore {
        &mut self.rng
    }

    fn verifier(&mut self) -> Result<&mut impl TlsVerifier<Self::CipherSuite>, TlsError> {
        Ok(&mut self.verifier)
    }
}

struct OsRng;

impl RngCore for OsRng {
    fn next_u32(&mut self) -> u32 {
        let mut bytes = [0u8; 4];
        self.fill_bytes(&mut bytes);
        u32::from_le_bytes(bytes)
    }

    fn next_u64(&mut self) -> u64 {
        let mut bytes = [0u8; 8];
        self.fill_bytes(&mut bytes);
        u64::from_le_bytes(bytes)
    }

    fn fill_bytes(&mut self, dest: &mut [u8]) {
        getrandom::getrandom(dest).expect("failed to generate random bytes");
    }

    fn try_fill_bytes(&mut self, dest: &mut [u8]) -> Result<(), rand_core::Error> {
        getrandom::getrandom(dest)
            .map_err(|_| rand_core::Error::from(core::num::NonZeroU32::new(1).unwrap()))
    }
}

impl CryptoRng for OsRng {}

struct SystemClock;

impl TlsClock for SystemClock {
    fn now() -> Option<u64> {
        std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .ok()
            .map(|duration| duration.as_secs())
    }
}

fn pem_to_der(pem: &str) -> Result<Vec<u8>, Error> {
    let (label, der) = pem_rfc7468::decode_vec(pem.as_bytes())
        .map_err(|error| bzd::base::error::data!("invalid TLS CA certificate PEM: {error}"))?;
    if label != "CERTIFICATE" {
        return Err(bzd::base::error::data!(
            "unexpected TLS CA certificate PEM label: {label}"
        ));
    }
    Ok(der)
}

fn map_tls_error(error: TlsError) -> Error {
    match error {
        TlsError::ConnectionClosed => bzd::base::error::eof!("tls: connection closed"),
        TlsError::InvalidCertificate
        | TlsError::InvalidCertificateEntry
        | TlsError::InvalidCertificateRequest
        | TlsError::InvalidSignature
        | TlsError::InvalidSignatureScheme
        | TlsError::DecodeError
        | TlsError::ParseError(_) => bzd::base::error::data!("tls: {error}"),
        TlsError::Io(_) => bzd::base::error::failure!("tls io: {error}"),
        _ => bzd::base::error::failure!("tls: {error}"),
    }
}

async fn connect<S>(
    transport: S,
    server_name: &str,
    ca_certificate: &str,
) -> Result<TlsStream<OwnedConnection<Transport<S>>>, Error>
where
    S: IStream + OStream + 'static,
{
    if server_name.is_empty() {
        return Err(bzd::base::error::failure!(
            "TLS server name must not be empty"
        ));
    }
    let ca = pem_to_der(ca_certificate)?;
    let config = TlsConfig::new()
        .with_server_name(server_name)
        .enable_rsa_signatures();
    let provider = Provider::new(OsRng, &ca);
    let mut connection = OwnedConnection::new(Transport::new(transport));
    connection
        .open(TlsContext::new(&config, provider))
        .await
        .map_err(map_tls_error)?;
    Ok(TlsStream::new(connection))
}

pub struct BzdComponentsStdNetworkTls<C>
where
    C: BzdComponentsStdNetworkTlsContextConstraint,
{
    context: BzdComponentsStdNetworkTlsContext<C>,
}

impl<C> BzdComponentsStdNetworkTls<C>
where
    C: BzdComponentsStdNetworkTlsContextConstraint,
{
    pub fn new(context: BzdComponentsStdNetworkTlsContext<C>) -> Self {
        Self { context }
    }
}

impl<C> BzdNetworkTlsClientInterface for BzdComponentsStdNetworkTls<C>
where
    C: BzdComponentsStdNetworkTlsContextConstraint,
{
    type Stream =
        TlsStream<OwnedConnection<Transport<<C::Tcp as BzdNetworkTcpClientInterface>::Stream>>>;

    async fn connect(&self, address: SocketAddr, server_name: &str) -> Result<Self::Stream, Error> {
        let transport = self.context.tcp.lock().await.connect(address).await?;
        connect(transport, server_name, self.context.ca_certificate).await
    }
}

impl<C> BzdComponentsStdNetworkTlsInterface for BzdComponentsStdNetworkTls<C> where
    C: BzdComponentsStdNetworkTlsContextConstraint
{
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;
    use bzd::base::error::ErrorType as BzdErrorType;

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

    impl ErrorType for MockConnection {
        type Error = TlsError;
    }

    impl Read for MockConnection {
        async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Self::Error> {
            let size = buffer.len().min(self.input.len());
            buffer[..size].copy_from_slice(&self.input[..size]);
            self.input = &self.input[size..];
            Ok(size)
        }
    }

    impl Write for MockConnection {
        async fn write(&mut self, data: &[u8]) -> Result<usize, Self::Error> {
            let size = data.len().min(self.output.len() - self.length);
            self.output[self.length..self.length + size].copy_from_slice(&data[..size]);
            self.length += size;
            Ok(size)
        }

        async fn flush(&mut self) -> Result<(), Self::Error> {
            Ok(())
        }
    }

    struct MockStream {
        input: &'static [u8],
        output: [u8; 8],
        length: usize,
    }

    impl MockStream {
        fn new(input: &'static [u8]) -> Self {
            Self {
                input,
                output: [0; 8],
                length: 0,
            }
        }
    }

    impl IStream for MockStream {
        async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Error> {
            let size = buffer.len().min(self.input.len());
            buffer[..size].copy_from_slice(&self.input[..size]);
            self.input = &self.input[size..];
            Ok(size)
        }
    }

    impl OStream for MockStream {
        async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
            let size = data.len().min(self.output.len() - self.length);
            self.output[self.length..self.length + size].copy_from_slice(&data[..size]);
            self.length += size;
            Ok(size)
        }

        async fn flush(&mut self) -> Result<(), Error> {
            Ok(())
        }
    }

    #[test]
    async fn test_tls_stream_delegates() -> TestResult {
        let mut stream = TlsStream::new(MockConnection::new(b"hello"));
        let mut buffer = [0u8; 5];
        assert_eq!(stream.read(&mut buffer).await?, 5)?;
        assert_eq!(&buffer, b"hello")?;
        stream.write_all(b"world").await?;
        assert_eq!(&stream.connection.output, b"world\0\0\0")?;
        Ok(())
    }

    #[test]
    async fn test_transport_delegates() -> TestResult {
        let mut transport = Transport::new(MockStream::new(b"hello"));
        let mut buffer = [0u8; 5];
        assert_eq!(
            Read::read(&mut transport, &mut buffer)
                .await
                .map_err(EmbeddedError::into_error)?,
            5
        )?;
        assert_eq!(&buffer, b"hello")?;
        Write::write_all(&mut transport, b"world")
            .await
            .map_err(EmbeddedError::into_error)?;
        assert_eq!(&transport.stream.output, b"world\0\0\0")?;
        Ok(())
    }

    #[test]
    fn test_pem_to_der_decodes_certificate() -> TestResult {
        let pem = "-----BEGIN CERTIFICATE-----\nMAA=\n-----END CERTIFICATE-----\n";
        assert_eq!(pem_to_der(pem)?, vec![0x30, 0x00])?;
        Ok(())
    }

    #[test]
    fn test_pem_to_der_rejects_invalid_input() -> TestResult {
        let error = pem_to_der("not a certificate").unwrap_err();
        assert_eq!(error.error_type(), BzdErrorType::Data)?;
        Ok(())
    }

    #[test]
    fn test_pem_to_der_rejects_unexpected_label() -> TestResult {
        let pem = "-----BEGIN PRIVATE KEY-----\nMAA=\n-----END PRIVATE KEY-----\n";
        let error = pem_to_der(pem).unwrap_err();
        assert_eq!(error.error_type(), BzdErrorType::Data)?;
        Ok(())
    }

    #[test]
    async fn test_connect_rejects_empty_server_name() -> TestResult {
        match connect(MockStream::new(b""), "", "").await {
            Err(error) => assert_eq!(error.error_type(), BzdErrorType::Failure)?,
            Ok(_) => return Err(bzd::base::error::failure!("expected connect to fail")),
        }
        Ok(())
    }

    #[test]
    fn test_map_tls_error() -> TestResult {
        assert_eq!(
            map_tls_error(TlsError::ConnectionClosed).error_type(),
            BzdErrorType::Eof
        )?;
        assert_eq!(
            map_tls_error(TlsError::InvalidCertificate).error_type(),
            BzdErrorType::Data
        )?;
        assert_eq!(
            map_tls_error(TlsError::InternalError).error_type(),
            BzdErrorType::Failure
        )?;
        Ok(())
    }
}
