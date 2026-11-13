#![no_std]
#![no_main]

//! Extracts one JPEG frame (SOI through EOI) from a byte stream.

use bzd::base::error::Error;

/// Incremental JPEG frame extractor.
pub struct JpegFramer<'a> {
    out: &'a mut [u8],
    offset: usize,
    complete: bool,
    state: State,
}

enum State {
    /// Looking for `FF D8`; `previous` carries the last byte across chunks.
    Scan {
        previous: u8,
    },
    /// Expecting the `FF` that starts a marker.
    MarkerStart,
    /// Reading the marker code byte.
    MarkerCode,
    LengthHigh {
        is_sos: bool,
    },
    LengthLow {
        is_sos: bool,
        length: usize,
    },
    Skip {
        remaining: usize,
        then_entropy: bool,
    },
    /// Entropy-coded scan data.
    Entropy,
    EntropyEscape,
}

impl<'a> JpegFramer<'a> {
    pub fn new(out: &'a mut [u8]) -> Self {
        Self {
            out,
            offset: 0,
            complete: false,
            state: State::Scan { previous: 0 },
        }
    }

    /// Length of the extracted frame, once complete.
    pub fn length(&self) -> Option<usize> {
        self.complete.then_some(self.offset)
    }

    /// Feed bytes and return how many were consumed.
    pub fn feed(&mut self, bytes: &[u8]) -> Result<usize, Error> {
        for (index, byte) in bytes.iter().copied().enumerate() {
            if self.complete {
                return Ok(index);
            }
            match &mut self.state {
                State::Scan { previous } => {
                    if *previous == 0xFF && byte == 0xD8 {
                        self.push(0xFF)?;
                        self.push(0xD8)?;
                        self.state = State::MarkerStart;
                    } else {
                        *previous = byte;
                    }
                }
                _ => {
                    self.push(byte)?;
                    self.step(byte)?;
                    if self.complete {
                        return Ok(index + 1);
                    }
                }
            }
        }
        Ok(bytes.len())
    }

    fn push(&mut self, byte: u8) -> Result<(), Error> {
        if self.offset >= self.out.len() {
            return Err(bzd::base::error::failure!(
                "OV3660: JPEG frame exceeds {} bytes",
                self.out.len()
            ));
        }
        self.out[self.offset] = byte;
        self.offset += 1;
        Ok(())
    }

    fn step(&mut self, byte: u8) -> Result<(), Error> {
        match self.state {
            State::Scan { .. } => {}
            State::MarkerStart => {
                if byte == 0xFF {
                    self.state = State::MarkerCode;
                }
            }
            State::MarkerCode => self.marker_code(byte)?,
            State::LengthHigh { is_sos } => {
                self.state = State::LengthLow {
                    is_sos,
                    length: (byte as usize) << 8,
                };
            }
            State::LengthLow { is_sos, length } => {
                let length = length | byte as usize;
                if length < 2 {
                    return Err(bzd::base::error::failure!(
                        "OV3660: invalid JPEG segment length"
                    ));
                }
                let remaining = length - 2;
                self.state = if remaining == 0 {
                    if is_sos {
                        State::Entropy
                    } else {
                        State::MarkerStart
                    }
                } else {
                    State::Skip {
                        remaining,
                        then_entropy: is_sos,
                    }
                };
            }
            State::Skip {
                remaining,
                then_entropy,
            } => {
                let remaining = remaining - 1;
                self.state = if remaining == 0 {
                    if then_entropy {
                        State::Entropy
                    } else {
                        State::MarkerStart
                    }
                } else {
                    State::Skip {
                        remaining,
                        then_entropy,
                    }
                };
            }
            State::Entropy => {
                if byte == 0xFF {
                    self.state = State::EntropyEscape;
                }
            }
            State::EntropyEscape => match byte {
                0x00 | 0xD0..=0xD7 => self.state = State::Entropy,
                0xFF => self.state = State::EntropyEscape,
                _ => self.marker_code(byte)?,
            },
        }
        Ok(())
    }

    fn marker_code(&mut self, code: u8) -> Result<(), Error> {
        match code {
            0xD9 => self.complete = true,
            0xDA => self.state = State::LengthHigh { is_sos: true },
            0xD0..=0xD7 | 0x01 | 0xD8 => self.state = State::MarkerStart,
            _ => self.state = State::LengthHigh { is_sos: false },
        }
        Ok(())
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    const FRAME: &[u8] = &[
        0xFF, 0xD8, // SOI
        0xFF, 0xE0, 0x00, 0x04, 0xAA, 0xBB, // APP0 with a 2-byte payload
        0xFF, 0xDB, 0x00, 0x04, 0xFF, 0xD9, // DQT payload containing FF D9
        0xFF, 0xDA, 0x00, 0x04, 0x00, 0x00, // SOS header
        0x11, 0xFF, 0x00, 0x22, // entropy data with an escaped FF
        0xFF, 0xD0, // restart marker
        0x33, // more entropy data
        0xFF, 0xD9, // EOI
    ];

    #[test]
    fn extracts_a_whole_frame_from_one_chunk() -> TestResult {
        let mut out = [0u8; 128];
        let mut framer = JpegFramer::new(&mut out);
        let consumed = framer.feed(FRAME)?;
        assert_eq!(consumed, FRAME.len())?;
        assert_eq!(framer.length(), Some(FRAME.len()))?;
        assert_eq!(&out[..FRAME.len()], FRAME)?;
        Ok(())
    }

    #[test]
    fn handles_chunk_boundaries() -> TestResult {
        let mut out = [0u8; 128];
        let mut framer = JpegFramer::new(&mut out);
        let mut consumed = 0;
        for chunk in FRAME.chunks(1) {
            consumed += framer.feed(chunk)?;
        }
        assert_eq!(consumed, FRAME.len())?;
        assert_eq!(framer.length(), Some(FRAME.len()))?;
        assert_eq!(&out[..FRAME.len()], FRAME)?;
        Ok(())
    }

    #[test]
    fn skips_bytes_before_soi() -> TestResult {
        let mut input = [0x00, 0x11, 0x22];
        let mut out = [0u8; 128];
        let mut framer = JpegFramer::new(&mut out);
        assert_eq!(framer.feed(&input)?, 3)?;
        input.copy_from_slice(&FRAME[..3]);
        assert_eq!(framer.feed(&input)?, 3)?;
        Ok(())
    }

    #[test]
    fn rejects_a_frame_that_does_not_fit() -> TestResult {
        let mut out = [0u8; 4];
        let mut framer = JpegFramer::new(&mut out);
        assert!(framer.feed(FRAME).is_err());
        Ok(())
    }
}
