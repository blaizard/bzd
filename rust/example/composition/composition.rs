use component::{Lifecycle, LocalStatic, Wrapper};
use user::*;

// ---- Registry

struct UserEntryAlpha {
    instance: BzdComponentsUser,
}

impl Lifecycle for UserEntryAlpha {
    async fn init(&mut self) -> Result<(), bzd::base::error::Error> {
        println!("[alex] init");
        Ok(())
    }

    async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error> {
        println!("[alex] shutdown");
        Ok(())
    }
}

// One registry function per composition entry, it creates and returns mutable
// access to the component wrapper.
fn registry_user_alpha() -> &'static mut Wrapper<UserEntryAlpha> {
    static COMPONENT: LocalStatic<Wrapper<UserEntryAlpha>> = LocalStatic::new();
    COMPONENT.get_mut_or_init(|| {
        Wrapper::new(UserEntryAlpha {
            instance: BzdComponentsUser::new(BzdComponentsUserContext {
                age: 28,
                username: "Alex",
            }),
        })
    })
}

struct UserEntryBeta {
    instance: BzdComponentsUser,
}

impl Lifecycle for UserEntryBeta {
    async fn init(&mut self) -> Result<(), bzd::base::error::Error> {
        println!("[bob] init");
        Ok(())
    }

    async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error> {
        println!("[bob] shutdown");
        Ok(())
    }
}

fn registry_user_beta() -> &'static mut Wrapper<UserEntryBeta> {
    static COMPONENT: LocalStatic<Wrapper<UserEntryBeta>> = LocalStatic::new();
    COMPONENT.get_mut_or_init(|| {
        Wrapper::new(UserEntryBeta {
            instance: BzdComponentsUser::new(BzdComponentsUserContext {
                age: 42,
                username: "Bob",
            }),
        })
    })
}

fn block_on<F: core::future::Future>(future: F) -> F::Output {
    use core::task::{Context, Poll, RawWaker, RawWakerVTable, Waker};

    fn clone(_: *const ()) -> RawWaker {
        RawWaker::new(core::ptr::null(), &VTABLE)
    }
    fn noop(_: *const ()) {}
    static VTABLE: RawWakerVTable = RawWakerVTable::new(clone, noop, noop, noop);

    let mut future = core::pin::pin!(future);
    // Safety: The waker never dereferences its data pointer.
    let waker = unsafe { Waker::from_raw(RawWaker::new(core::ptr::null(), &VTABLE)) };
    let mut context = Context::from_waker(&waker);
    loop {
        if let Poll::Ready(result) = future.as_mut().poll(&mut context) {
            return result;
        }
    }
}

fn main() -> Result<(), bzd::base::error::Error> {
    let user_alpha = registry_user_alpha();
    let user_beta = registry_user_beta();

    block_on(user_alpha.init())?;
    block_on(user_beta.init())?;

    block_on(user_alpha.instance.print_info())?;
    block_on(user_alpha.instance.print_info())?;
    block_on(user_beta.instance.print_info())?;

    block_on(user_alpha.shutdown())?;
    block_on(user_beta.shutdown())?;

    Ok(())
}
