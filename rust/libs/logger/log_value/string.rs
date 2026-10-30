use super::LogValue;
use bzd::base::error::Error;
use bzd::base::stream::OStream;

impl LogValue for str {
    async fn write_to(&self, out: &mut impl OStream) -> Result<(), Error> {
        out.write_all(self.as_bytes()).await
    }
}
