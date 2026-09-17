#![no_std]
#![no_main]

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
        let _ = self.context.user.greet().await?;
        Ok(())
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;
    use component::LocalStatic;

    #[test]
    fn test_component() -> TestResult {
        static USER: LocalStatic<User> = LocalStatic::new();
        let user = USER.get_mut_or_init(|| User {
            context: BzdTestUserContext { username: "Alex" },
        });
        let mut manager = Manager {
            context: ManagerContext::new(user),
        };
        let _future = manager.run();
        Ok(())
    }
}
