#![no_std]

extern crate alloc;

use alloc::boxed::Box;
use alloc::ffi::CString;
use bzd::base::error::Error;
use bzd::base::stream::{IStream, OStream};
use core::mem::MaybeUninit;
use core::net::SocketAddr;
use embassy_sync::once_lock::OnceLock;
use embedded_io_async::{ErrorKind, ErrorType, Read, Write};
use interfaces_tcp::BzdNetworkTcpClientInterface;
use mbedtls_rs::{
    AuthMode, Certificate, ClientSessionConfig, Session, SessionConfig, SessionError, Tls,
    TlsVersion, X509,
};
use rand_chacha::ChaCha20Rng;
use rand_core::SeedableRng;
#[allow(unused_imports)]
pub use rust_components_esp_network_tls_interface::{
    BzdComponentsEspNetworkTlsContext, BzdComponentsEspNetworkTlsContextConstraint,
    BzdComponentsEspNetworkTlsContextConstraintTypes, BzdComponentsEspNetworkTlsInterface,
};
use rust_interfaces_tls::BzdNetworkTlsClientInterface;

static TLS: OnceLock<Result<&'static Tls<'static>, mbedtls_rs::TlsError>> = OnceLock::new();

const DRAM2_HEAP_SIZE: usize = 64 * 1024;

// Extra heap region in DRAM2, which is otherwise unused by the linker script.
#[unsafe(link_section = ".dram2_uninit")]
static mut DRAM2_HEAP: MaybeUninit<[u8; DRAM2_HEAP_SIZE]> = MaybeUninit::uninit();

fn setup_tls_heap() {
    // SAFETY: DRAM2 is reserved for the heap by the linker script and lives for
    // the whole program.
    unsafe {
        esp_alloc::HEAP.add_region(esp_alloc::HeapRegion::new(
            core::ptr::addr_of_mut!(DRAM2_HEAP).cast::<u8>(),
            DRAM2_HEAP_SIZE,
            esp_alloc::MemoryCapability::Internal.into(),
        ));
    }
}

fn tls() -> Result<&'static Tls<'static>, Error> {
    let result = TLS.get_or_init(|| {
        setup_tls_heap();
        let rng = esp_hal::rng::Rng::new();
        let mut seed = [0u8; 32];
        for chunk in seed.chunks_mut(4) {
            chunk.copy_from_slice(&rng.random().to_le_bytes());
        }
        let rng = Box::leak(Box::new(ChaCha20Rng::from_seed(seed)));
        Tls::new(rng).map(|tls| &*Box::leak(Box::new(tls)))
    });
    match result {
        Ok(tls) => Ok(*tls),
        Err(error) => Err(bzd::base::error::failure!("mbedtls init failed: {error:?}")),
    }
}

/// Bidirectional TLS stream, exposed as a bzd stream.
pub struct TlsStream<C> {
    connection: C,
}

impl<C> TlsStream<C> {
    fn new(connection: C) -> Self {
        Self { connection }
    }
}

impl<C: Read<Error = SessionError>> IStream for TlsStream<C> {
    type Chunk<'a>
        = &'a [u8]
    where
        Self: 'a;

    async fn read<'a>(&'a mut self, buffer: &'a mut [u8]) -> Result<Self::Chunk<'a>, Error> {
        let size = self
            .connection
            .read(buffer)
            .await
            .map_err(map_session_error)?;
        Ok(&buffer[..size])
    }
}

impl<C: Write<Error = SessionError>> OStream for TlsStream<C> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
        self.connection.write(data).await.map_err(map_session_error)
    }

    async fn flush(&mut self) -> Result<(), Error> {
        self.connection.flush().await.map_err(map_session_error)
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

#[doc(hidden)]
#[derive(Debug)]
pub struct TransportError(bzd::base::error::Error);

impl core::fmt::Display for TransportError {
    fn fmt(&self, formatter: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        self.0.fmt(formatter)
    }
}

impl core::error::Error for TransportError {}

impl embedded_io_async::Error for TransportError {
    fn kind(&self) -> ErrorKind {
        match self.0.error_type() {
            bzd::base::error::ErrorType::Timeout => ErrorKind::TimedOut,
            bzd::base::error::ErrorType::Data => ErrorKind::InvalidData,
            bzd::base::error::ErrorType::Eof => ErrorKind::BrokenPipe,
            _ => ErrorKind::Other,
        }
    }
}

impl<S: IStream + OStream> ErrorType for Transport<S> {
    type Error = TransportError;
}

impl<S: IStream + OStream> Read for Transport<S> {
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Self::Error> {
        self.stream.read_into(buffer).await.map_err(TransportError)
    }
}

impl<S: IStream + OStream> Write for Transport<S> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Self::Error> {
        self.stream.write(data).await.map_err(TransportError)
    }

    async fn flush(&mut self) -> Result<(), Self::Error> {
        self.stream.flush().await.map_err(TransportError)
    }
}

fn map_session_error(error: SessionError) -> Error {
    match error {
        SessionError::Io(ErrorKind::TimedOut) => bzd::base::error::timeout!("tls: {error}"),
        SessionError::Io(ErrorKind::InvalidData) => bzd::base::error::data!("tls: {error}"),
        SessionError::Io(ErrorKind::ConnectionReset) => bzd::base::error::eof!("tls: {error}"),
        _ => bzd::base::error::failure!("tls: {error}"),
    }
}

async fn connect<S>(
    tls: &'static Tls<'static>,
    transport: S,
    server_name: &str,
    ca_certificate: &str,
) -> Result<TlsStream<Session<'static, Transport<S>>>, Error>
where
    S: IStream + OStream + 'static,
{
    if server_name.is_empty() {
        return Err(bzd::base::error::failure!(
            "TLS server name must not be empty"
        ));
    }

    let ca_certificate = CString::new(ca_certificate)
        .map_err(|_| bzd::base::error::data!("TLS CA certificate contains a nul byte"))?;
    let ca = Certificate::new(X509::PEM(&ca_certificate))
        .map_err(|error| bzd::base::error::data!("invalid TLS CA certificate: {error}"))?;
    let config = ClientSessionConfig {
        ca_chain: Some(ca),
        auth_mode: AuthMode::Required,
        min_version: TlsVersion::Tls1_2,
        ..ClientSessionConfig::new()
    };
    let mut session = Session::new(
        tls.reference(),
        Transport::new(transport),
        &SessionConfig::Client(config),
    )
    .map_err(|error| bzd::base::error::failure!("tls session creation failed: {error}"))?;

    let server_name = CString::new(server_name)
        .map_err(|_| bzd::base::error::data!("TLS server name contains a nul byte"))?;
    session
        .set_server_name(&server_name)
        .map_err(|error| bzd::base::error::failure!("tls server name failed: {error}"))?;
    session
        .connect()
        .await
        .map_err(|error| bzd::base::error::failure!("tls handshake failed: {error}"))?;

    Ok(TlsStream::new(session))
}

pub struct BzdComponentsEspNetworkTls<C>
where
    C: BzdComponentsEspNetworkTlsContextConstraint,
{
    context: BzdComponentsEspNetworkTlsContext<C>,
}

impl<C> BzdComponentsEspNetworkTls<C>
where
    C: BzdComponentsEspNetworkTlsContextConstraint,
{
    pub fn new(context: BzdComponentsEspNetworkTlsContext<C>) -> Self {
        Self { context }
    }
}

impl<C> BzdNetworkTlsClientInterface for BzdComponentsEspNetworkTls<C>
where
    C: BzdComponentsEspNetworkTlsContextConstraint,
{
    type Stream =
        TlsStream<Session<'static, Transport<<C::Tcp as BzdNetworkTcpClientInterface>::Stream>>>;

    async fn connect(&self, address: SocketAddr, server_name: &str) -> Result<Self::Stream, Error> {
        let transport = self.context.tcp.lock().await.connect(address).await?;
        let tls = tls()?;
        connect(tls, transport, server_name, self.context.ca_certificate).await
    }
}

impl<C> BzdComponentsEspNetworkTlsInterface for BzdComponentsEspNetworkTls<C> where
    C: BzdComponentsEspNetworkTlsContextConstraint
{
}
