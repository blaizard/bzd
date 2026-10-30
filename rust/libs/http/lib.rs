//! HTTP client wrapper over `reqwless` for bzd streams.
//!
//! This crate exposes a small `Request`/`ResponseInfo` API on top of
//! `reqwless`. The underlying HTTP implementation is an internal detail.

#![no_std]
#![no_main]

mod error;

use bzd::base::error::Error;
use bzd::base::stream::{EmbeddedIOStream, IOStream};
use error::map_error;
use reqwless::client::HttpConnection;
use reqwless::request::{Method as ReqwlessMethod, Request as ReqwlessRequest, RequestBuilder};

/// HTTP request methods supported by the wrapper.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Method {
    /// GET.
    Get,
    /// PUT.
    Put,
    /// POST.
    Post,
    /// DELETE.
    Delete,
    /// HEAD.
    Head,
    /// PATCH.
    Patch,
    /// OPTIONS.
    Options,
    /// CONNECT.
    Connect,
    /// TRACE.
    Trace,
}

/// An HTTP request.
pub struct Request<'a> {
    /// HTTP method.
    pub method: Method,
    /// Value of the `Host` header.
    pub host: &'a str,
    /// Request path.
    pub path: &'a str,
    /// Additional headers.
    pub headers: &'a [(&'a str, &'a str)],
    /// Request body.
    pub body: &'a [u8],
}

/// Metadata of a fully read HTTP response.
#[derive(Clone, Copy, Debug)]
pub struct ResponseInfo {
    /// HTTP status code.
    pub status: u16,
    /// Content length advertised by the server, if any.
    pub content_length: Option<usize>,
    /// Number of body bytes at the beginning of the buffer passed to [`request`].
    pub body_length: usize,
}

/// Send an HTTP request over a bzd stream and read the full response body.
///
/// The buffer is used as scratch space for the response headers and then holds the
/// response body, available as `buffer[..body_length]`. It must be large enough to
/// contain the whole body, otherwise `BufferTooSmall` is returned.
pub async fn request<S>(
    stream: &mut S,
    request: Request<'_>,
    buffer: &mut [u8],
) -> Result<ResponseInfo, Error>
where
    S: IOStream + ?Sized,
{
    let mut connection = HttpConnection::Plain(EmbeddedIOStream::new(stream));
    let http_request = ReqwlessRequest::new(to_reqwless_method(request.method), request.path)
        .host(request.host)
        .headers(request.headers)
        .body(request.body)
        .build();
    let response = connection
        .send(http_request, buffer)
        .await
        .map_err(map_error)?;
    let status = response.status.0;
    let content_length = response.content_length;
    let body_length = response
        .body()
        .read_to_end()
        .await
        .map_err(map_error)?
        .len();
    Ok(ResponseInfo {
        status,
        content_length,
        body_length,
    })
}

fn to_reqwless_method(method: Method) -> ReqwlessMethod {
    match method {
        Method::Get => ReqwlessMethod::GET,
        Method::Put => ReqwlessMethod::PUT,
        Method::Post => ReqwlessMethod::POST,
        Method::Delete => ReqwlessMethod::DELETE,
        Method::Head => ReqwlessMethod::HEAD,
        Method::Patch => ReqwlessMethod::PATCH,
        Method::Options => ReqwlessMethod::OPTIONS,
        Method::Connect => ReqwlessMethod::CONNECT,
        Method::Trace => ReqwlessMethod::TRACE,
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;
    use crate::error::map_error;
    use bzd::base::error::ErrorType;
    use bzd::base::stream::{IStream, OStream};

    struct MockStream {
        input: &'static [u8],
        input_offset: usize,
        output: [u8; 256],
        output_length: usize,
    }

    impl MockStream {
        fn new(input: &'static [u8]) -> Self {
            Self {
                input,
                input_offset: 0,
                output: [0; 256],
                output_length: 0,
            }
        }
    }

    impl IStream for MockStream {
        async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, bzd::base::error::Error> {
            assert_eq!(buffer.is_empty(), false)?;
            let remaining = &self.input[self.input_offset..];
            let size = buffer.len().min(remaining.len());
            buffer[..size].copy_from_slice(&remaining[..size]);
            self.input_offset += size;
            Ok(size)
        }
    }

    impl OStream for MockStream {
        async fn write(&mut self, data: &[u8]) -> Result<usize, bzd::base::error::Error> {
            assert_eq!(data.is_empty(), false)?;
            let size = data.len().min(self.output.len() - self.output_length);
            self.output[self.output_length..self.output_length + size]
                .copy_from_slice(&data[..size]);
            self.output_length += size;
            Ok(size)
        }
    }

    #[test]
    async fn test_get() -> TestResult {
        let mut stream = MockStream::new(b"HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nhello");
        let mut buffer = [0u8; 256];
        let response = request(
            &mut stream,
            Request {
                method: Method::Get,
                host: "example.com",
                path: "/status",
                headers: &[],
                body: &[],
            },
            &mut buffer,
        )
        .await?;
        assert_eq!(response.status, 200)?;
        assert_eq!(response.content_length, Some(5))?;
        assert_eq!(response.body_length, 5)?;
        assert_eq!(&buffer[..response.body_length], &b"hello"[..])?;
        assert_eq!(
            stream.output[..stream.output_length].starts_with(b"GET /status HTTP/1.1\r\n"),
            true
        )?;
        Ok(())
    }

    #[test]
    async fn test_post() -> TestResult {
        let mut stream = MockStream::new(b"HTTP/1.1 204 No Content\r\n\r\n");
        let mut buffer = [0u8; 256];
        let response = request(
            &mut stream,
            Request {
                method: Method::Post,
                host: "example.com",
                path: "/echo",
                headers: &[],
                body: b"PING",
            },
            &mut buffer,
        )
        .await?;
        assert_eq!(response.status, 204)?;
        assert_eq!(response.body_length, 0)?;
        assert_eq!(
            stream.output[..stream.output_length].starts_with(b"POST /echo HTTP/1.1\r\n"),
            true
        )?;
        assert_eq!(
            stream.output[..stream.output_length].ends_with(b"\r\n\r\nPING"),
            true
        )?;
        Ok(())
    }

    #[test]
    async fn test_body_buffer_too_small() -> TestResult {
        let mut stream = MockStream::new(b"HTTP/1.1 200 OK\r\nContent-Length: 64\r\n\r\n");
        let mut buffer = [0u8; 32];
        match request(
            &mut stream,
            Request {
                method: Method::Get,
                host: "example.com",
                path: "/",
                headers: &[],
                body: &[],
            },
            &mut buffer,
        )
        .await
        {
            Err(error) => {
                assert_eq!(error.error_type(), ErrorType::Failure)?;
                assert_eq!(error.message(), "http: buffer too small")?;
            }
            Ok(_) => return Err(bzd::base::error::failure("expected the request to fail")),
        }
        Ok(())
    }

    #[test]
    fn test_map_error() -> TestResult {
        let error = map_error(reqwless::Error::BufferTooSmall);
        assert_eq!(error.error_type(), ErrorType::Failure)?;
        let error = map_error(reqwless::Error::ConnectionAborted);
        assert_eq!(error.error_type(), ErrorType::Eof)?;
        Ok(())
    }
}
