use super::LogValue;
use bzd_stream_impl::BzdOStreamInterface;
use error::Error;

/// Write the decimal representation of a value, most significant digit first,
/// with no intermediate buffer.
pub(crate) async fn write_unsigned(
    mut value: u64,
    out: &mut impl BzdOStreamInterface,
) -> Result<(), Error> {
    let mut divisor: u64 = 1;
    while divisor <= value / 10 {
        divisor *= 10;
    }
    loop {
        let digit = (value / divisor) as u8;
        out.write(&[b'0' + digit]).await?;
        value %= divisor;
        if divisor == 1 {
            break;
        }
        divisor /= 10;
    }
    Ok(())
}

macro_rules! impl_log_value_unsigned {
	($($ty:ty),* $(,)?) => {
		$(
			impl LogValue for $ty {
				async fn write(&self, out: &mut impl BzdOStreamInterface) -> Result<(), Error> {
					write_unsigned(*self as u64, out).await
				}
			}
		)*
	};
}

macro_rules! impl_log_value_signed {
	($($ty:ty),* $(,)?) => {
		$(
			impl LogValue for $ty {
				async fn write(&self, out: &mut impl BzdOStreamInterface) -> Result<(), Error> {
					if *self < 0
					{
						out.write(b"-").await?;
					}
					write_unsigned(self.unsigned_abs() as u64, out).await
				}
			}
		)*
	};
}

impl_log_value_unsigned!(u8, u16, u32, u64, usize);
impl_log_value_signed!(i8, i16, i32, i64, isize);
