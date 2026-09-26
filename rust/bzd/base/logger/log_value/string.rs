use super::LogValue;
use bzd_stream_impl::BzdOStreamInterface;
use error::Error;

impl LogValue for str {
    async fn write(&self, out: &mut impl BzdOStreamInterface) -> Result<(), Error> {
        out.write(self.as_bytes()).await
    }
}
