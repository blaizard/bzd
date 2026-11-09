//! Minimal HTTP URL parser for `no_std` environments.

use bzd::base::error::Error;

/// HTTP URL schemes supported by the client.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Scheme {
    /// Plain HTTP.
    Http,
    /// HTTP over TLS.
    Https,
}

impl Scheme {
    /// Default port associated with the scheme.
    pub const fn default_port(&self) -> u16 {
        match self {
            Self::Http => 80,
            Self::Https => 443,
        }
    }

    /// Whether the scheme requires a TLS connection.
    pub const fn is_tls(&self) -> bool {
        matches!(self, Self::Https)
    }
}

/// Parsed HTTP URL.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Url<'a> {
    scheme: Scheme,
    authority: &'a str,
    host: &'a str,
    port: u16,
    path: &'a str,
}

impl<'a> Url<'a> {
    /// Parse an `http://` or `https://` URL.
    pub fn parse(input: &'a str) -> Result<Self, Error> {
        let (scheme, rest) = input
            .split_once("://")
            .ok_or_else(|| bzd::base::error::failure!("http: url is missing a scheme"))?;

        let scheme = match scheme {
            "http" => Scheme::Http,
            "https" => Scheme::Https,
            _ => return Err(bzd::base::error::failure!("http: unsupported url scheme")),
        };

        let (authority, path) = match rest.find('/') {
            Some(index) => (&rest[..index], &rest[index..]),
            None => (rest, "/"),
        };

        if authority.is_empty() {
            return Err(bzd::base::error::failure!("http: url is missing a host"));
        }

        let (host, port) = Self::parse_authority(authority, scheme)?;
        if host.is_empty() {
            return Err(bzd::base::error::failure!("http: url is missing a host"));
        }

        Ok(Self {
            scheme,
            authority,
            host,
            port,
            path,
        })
    }

    fn parse_authority(authority: &'a str, scheme: Scheme) -> Result<(&'a str, u16), Error> {
        if let Some(stripped) = authority.strip_prefix('[') {
            let end = stripped
                .find(']')
                .ok_or_else(|| bzd::base::error::failure!("http: invalid IPv6 host"))?;
            let host = &stripped[..end];
            let remainder = &stripped[end + 1..];
            let port = if remainder.is_empty() {
                scheme.default_port()
            } else {
                let port = remainder
                    .strip_prefix(':')
                    .ok_or_else(|| bzd::base::error::failure!("http: invalid url authority"))?;
                Self::parse_port(port)?
            };
            return Ok((host, port));
        }

        match authority.rsplit_once(':') {
            Some((host, port)) => Ok((host, Self::parse_port(port)?)),
            None => Ok((authority, scheme.default_port())),
        }
    }

    fn parse_port(port: &str) -> Result<u16, Error> {
        if port.is_empty() || !port.bytes().all(|byte| byte.is_ascii_digit()) {
            return Err(bzd::base::error::failure!("http: invalid url port"));
        }
        port.parse::<u16>()
            .map_err(|_| bzd::base::error::failure!("http: invalid url port"))
    }

    /// URL scheme.
    pub const fn scheme(&self) -> Scheme {
        self.scheme
    }

    /// Raw authority, as written in the URL (`host` or `host:port`).
    ///
    /// This is what should be used for the `Host` HTTP header.
    pub const fn authority(&self) -> &'a str {
        self.authority
    }

    /// Host without its optional port, used for DNS resolution and SNI.
    pub const fn host(&self) -> &'a str {
        self.host
    }

    /// Effective port, either explicit or the scheme default.
    pub const fn port(&self) -> u16 {
        self.port
    }

    /// Request path, always starting with `/`.
    pub const fn path(&self) -> &'a str {
        self.path
    }

    /// Whether the URL requires TLS.
    pub const fn is_tls(&self) -> bool {
        self.scheme.is_tls()
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    fn test_http() -> TestResult {
        let url = Url::parse("http://example.com/")?;
        assert_eq!(url.scheme(), Scheme::Http)?;
        assert_eq!(url.authority(), "example.com")?;
        assert_eq!(url.host(), "example.com")?;
        assert_eq!(url.port(), 80)?;
        assert_eq!(url.path(), "/")?;
        assert_eq!(url.is_tls(), false)?;
        Ok(())
    }

    #[test]
    fn test_https() -> TestResult {
        let url = Url::parse("https://www.google.com/humans.txt")?;
        assert_eq!(url.scheme(), Scheme::Https)?;
        assert_eq!(url.host(), "www.google.com")?;
        assert_eq!(url.port(), 443)?;
        assert_eq!(url.path(), "/humans.txt")?;
        assert_eq!(url.is_tls(), true)?;
        Ok(())
    }

    #[test]
    fn test_explicit_port() -> TestResult {
        let url = Url::parse("http://example.com:8080/status")?;
        assert_eq!(url.authority(), "example.com:8080")?;
        assert_eq!(url.host(), "example.com")?;
        assert_eq!(url.port(), 8080)?;
        assert_eq!(url.path(), "/status")?;
        Ok(())
    }

    #[test]
    fn test_default_path() -> TestResult {
        let url = Url::parse("https://example.com")?;
        assert_eq!(url.path(), "/")?;
        Ok(())
    }

    #[test]
    fn test_invalid() -> TestResult {
        assert_eq!(Url::parse("example.com").is_err(), true)?;
        assert_eq!(Url::parse("ftp://example.com").is_err(), true)?;
        assert_eq!(Url::parse("http://").is_err(), true)?;
        assert_eq!(Url::parse("http://example.com:abc/").is_err(), true)?;
        Ok(())
    }
}
