//! HTTP client wrapper over `reqwless` for bzd streams.
//!
//! This crate exposes a small `Request`/`ResponseInfo` API on top of
//! `reqwless`. A request is built with [`Request::new`], optionally customized
//! with [`Request::headers`] and [`Request::body`], and sent with
//! [`Request::send`]. The underlying HTTP implementation is an internal detail.

#![no_std]
#![no_main]

mod error;
pub mod url;

pub use url::{Scheme, Url};

use bzd::base::error::Error;
use bzd::base::stream::{EmbeddedStream, IOStream};
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
    method: Method,
    url: Url<'a>,
    headers: &'a [(&'a str, &'a str)],
    body: &'a [u8],
}

/// Metadata of a fully read HTTP response.
#[derive(Clone, Copy, Debug)]
pub struct ResponseInfo {
    /// HTTP status code.
    pub status: u16,
    /// Content length advertised by the server, if any.
    pub content_length: Option<usize>,
    /// Number of body bytes at the beginning of the buffer passed to [`Request::send`].
    pub body_length: usize,
}

impl<'a> Request<'a> {
    /// Build a request for `method` against `url`.
    ///
    /// The URL is parsed immediately, so an invalid URL fails here rather than
    /// when the request is sent.
    pub fn new(method: Method, url: &'a str) -> Result<Self, Error> {
        Ok(Self {
            method,
            url: Url::parse(url)?,
            headers: &[],
            body: &[],
        })
    }

    /// Set the additional headers.
    #[must_use]
    pub const fn headers(mut self, headers: &'a [(&'a str, &'a str)]) -> Self {
        self.headers = headers;
        self
    }

    /// Set the request body.
    #[must_use]
    pub const fn body(mut self, body: &'a [u8]) -> Self {
        self.body = body;
        self
    }

    /// HTTP method.
    pub const fn method(&self) -> Method {
        self.method
    }

    /// Parsed request URL.
    pub const fn url(&self) -> Url<'a> {
        self.url
    }

    /// Send the request over a bzd stream and read the full response body.
    ///
    /// The buffer is used as scratch space for the response headers and then holds the
    /// response body, available as `buffer[..body_length]`. It must be large enough to
    /// contain the whole body, otherwise `BufferTooSmall` is returned.
    pub async fn send<S>(self, stream: &mut S, buffer: &mut [u8]) -> Result<ResponseInfo, Error>
    where
        S: IOStream + ?Sized,
    {
        let mut connection = HttpConnection::Plain(EmbeddedStream::new(stream));
        let http_request = ReqwlessRequest::new(to_reqwless_method(self.method), self.url.path())
            .host(self.url.authority())
            .headers(self.headers)
            .body(self.body)
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
        type Chunk<'a>
            = &'a [u8]
        where
            Self: 'a;

        async fn read<'a>(
            &'a mut self,
            buffer: &'a mut [u8],
        ) -> Result<Self::Chunk<'a>, bzd::base::error::Error> {
            assert_eq!(buffer.is_empty(), false)?;
            let remaining = &self.input[self.input_offset..];
            let size = buffer.len().min(remaining.len());
            buffer[..size].copy_from_slice(&remaining[..size]);
            self.input_offset += size;
            Ok(&buffer[..size])
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
        let request =
            Request::new(Method::Get, "http://example.com/status")?.headers(&[("X-Test", "1")]);
        let response = request.send(&mut stream, &mut buffer).await?;
        assert_eq!(response.status, 200)?;
        assert_eq!(response.content_length, Some(5))?;
        assert_eq!(response.body_length, 5)?;
        assert_eq!(&buffer[..response.body_length], &b"hello"[..])?;
        assert_eq!(
            stream.output[..stream.output_length].starts_with(b"GET /status HTTP/1.1\r\n"),
            true
        )?;
        assert_eq!(
            stream.output[..stream.output_length]
                .windows(b"X-Test: 1\r\n".len())
                .any(|window| window == b"X-Test: 1\r\n"),
            true
        )?;
        Ok(())
    }

    #[test]
    async fn test_post() -> TestResult {
        let mut stream = MockStream::new(b"HTTP/1.1 204 No Content\r\n\r\n");
        let mut buffer = [0u8; 256];
        let request = Request::new(Method::Post, "http://example.com/echo")?.body(b"PING");
        let response = request.send(&mut stream, &mut buffer).await?;
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
        let request = Request::new(Method::Get, "http://example.com/")?;
        match request.send(&mut stream, &mut buffer).await {
            Err(error) => {
                assert_eq!(error.error_type(), ErrorType::Failure)?;
                assert_eq!(error.message(), "http: buffer too small")?;
            }
            Ok(_) => return Err(bzd::base::error::failure!("expected the request to fail")),
        }
        Ok(())
    }

    #[test]
    fn test_invalid_url() -> TestResult {
        match Request::new(Method::Get, "example.com") {
            Err(error) => assert_eq!(error.error_type(), ErrorType::Failure)?,
            Ok(_) => return Err(bzd::base::error::failure!("expected url parsing to fail")),
        }
        Ok(())
    }

    #[test]
    fn test_builder() -> TestResult {
        let request = Request::new(Method::Put, "https://example.com:8443/resource")?
            .headers(&[("Accept", "text/plain")])
            .body(b"payload");
        assert_eq!(request.method(), Method::Put)?;
        assert_eq!(request.url().port(), 8443)?;
        assert_eq!(request.url().path(), "/resource")?;
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
