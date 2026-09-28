#![no_std]

pub use rust_example_minimal_composition::{
    BzdExampleHelloWorldContext, BzdExampleHelloWorldContextConstraint,
    BzdExampleHelloWorldContextConstraintTypes, BzdExampleHelloWorldInterface,
};

use interfaces_stream::BzdOStreamInterface;

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
        self.context.out.write(b"Hello, world!\n").await?;
        Ok(())
    }
}
