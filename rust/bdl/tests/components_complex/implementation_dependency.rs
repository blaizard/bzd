#![no_std]
#![no_main]

use critical_section as _;
use rust_bdl_tests_components_complex_dependency::{
    BzdTestManagerContext, BzdTestManagerContextConstraintTypes, BzdTestManagerInterface,
    BzdTestUserContext, BzdTestUserInterface,
};

#[allow(dead_code)]
type ManagerContext = BzdTestManagerContext<BzdTestManagerContextConstraintTypes<User>>;

#[allow(dead_code)]
struct User {
    context: BzdTestUserContext,
}

impl BzdTestUserInterface for User {
    async fn greet(&mut self) -> Result<&'static str, bzd::base::error::Error> {
        Ok(self.context.username)
    }
}

#[allow(dead_code)]
struct Manager {
    context: ManagerContext,
}

impl BzdTestManagerInterface for Manager {
    async fn run(&mut self) -> Result<(), bzd::base::error::Error> {
        let mut user = self.context.user.lock().await;
        let _ = user.greet().await?;
        Ok(())
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;
    use component::{Component, StaticComponent};

    #[test]
    fn test_component() -> TestResult {
        static USER: StaticComponent<Component<User>> = StaticComponent::new();
        let user = USER.get_or_init(|| {
            Component::new(User {
                context: BzdTestUserContext { username: "Alex" },
            })
        });
        let mut manager = Manager {
            context: ManagerContext::new(user),
        };
        let _future = manager.run();
        Ok(())
    }
}
