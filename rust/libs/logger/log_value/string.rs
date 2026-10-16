use super::LogValue;
use bzd::base::error::Error;
use interfaces_stream::BzdOStreamInterface;

impl LogValue for str {
    async fn write(&self, out: &mut impl BzdOStreamInterface) -> Result<(), Error> {
        out.write(self.as_bytes()).await
    }
}
