#![no_std]

pub use rust_example_minimal_composition::{
    BzdExampleHelloWorldContext, BzdExampleHelloWorldContextConstraint,
    BzdExampleHelloWorldContextConstraintTypes, BzdExampleHelloWorldInterface,
};

use core::time::Duration;
use interfaces_stream::BzdOStreamInterface;
use interfaces_timer::BzdTimerInterface;

pub struct BzdExampleHelloWorld<C>
where
    C: BzdExampleHelloWorldContextConstraint,
{
    pub context: BzdExampleHelloWorldContext<C>,
}

impl<C> BzdExampleHelloWorld<C>
where
    C: BzdExampleHelloWorldContextConstraint,
{
    pub fn new(context: BzdExampleHelloWorldContext<C>) -> Self {
        Self { context }
    }
}

impl<C> BzdExampleHelloWorldInterface for BzdExampleHelloWorld<C>
where
    C: BzdExampleHelloWorldContextConstraint,
{
    async fn run(&mut self) -> Result<(), bzd::base::error::Error> {
        for _ in 1..3 {
            self.context.out.lock().await.write(b".").await?;
            self.context
                .timer
                .lock()
                .await
                .delay(Duration::from_secs(1))
                .await?;
        }

        self.context
            .out
            .lock()
            .await
            .write(b"Hello, world!\n")
            .await?;

        Ok(())
    }
}
