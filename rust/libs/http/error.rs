use bzd::base::error::{self, Error};

/// Convert a reqwless error into a bzd error.
pub(crate) fn map_error(error: reqwless::Error) -> Error {
    match error {
        reqwless::Error::Network(kind) => map_network_error(kind),
        reqwless::Error::ConnectionAborted => error::eof("http: connection aborted"),
        reqwless::Error::Codec => error::data("http: codec error"),
        reqwless::Error::InvalidUrl(_) => error::data("http: invalid url"),
        reqwless::Error::BufferTooSmall => error::failure("http: buffer too small"),
        reqwless::Error::AlreadySent => error::failure("http: request already sent"),
        reqwless::Error::IncorrectBodyWritten => error::failure("http: incorrect body length"),
        reqwless::Error::Dns => error::failure("http: dns error"),
    }
}

fn map_network_error(kind: embedded_io_async::ErrorKind) -> Error {
    match kind {
        embedded_io_async::ErrorKind::TimedOut => error::timeout("http: network timeout"),
        embedded_io_async::ErrorKind::InvalidData => error::data("http: network data error"),
        _ => error::failure("http: network error"),
    }
}
