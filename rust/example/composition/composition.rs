use child::{BzdComponentsChild, BzdComponentsChildContext, BzdComponentsPersonInterface};
use component::{Lifecycle, LocalStatic, Wrapper};
use parent::{
    BzdComponentsParent, BzdComponentsParentContext, BzdComponentsParentContextConstraintTypes,
};

// ---- Registry

struct PersonEntryChild {
    instance: BzdComponentsChild,
}

impl Lifecycle for PersonEntryChild {
    async fn init(&mut self) -> Result<(), bzd::base::error::Error> {
        println!("[child] init");
        Ok(())
    }

    async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error> {
        println!("[child] shutdown");
        Ok(())
    }
}

// One registry function per composition entry, it creates and returns mutable
// access to the component wrapper.
fn registry_person_child() -> &'static mut Wrapper<PersonEntryChild> {
    static COMPONENT: LocalStatic<Wrapper<PersonEntryChild>> = LocalStatic::new();
    COMPONENT.get_mut_or_init(|| {
        Wrapper::new(PersonEntryChild {
            instance: BzdComponentsChild::new(BzdComponentsChildContext {
                age: 28,
                username: "Alex",
            }),
        })
    })
}

struct PersonEntryParent {
    instance: BzdComponentsParent<BzdComponentsParentContextConstraintTypes<BzdComponentsChild>>,
}

impl Lifecycle for PersonEntryParent {
    async fn init(&mut self) -> Result<(), bzd::base::error::Error> {
        println!("[parent] init");
        Ok(())
    }

    async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error> {
        println!("[parent] shutdown");
        Ok(())
    }
}

fn registry_person_parent() -> &'static mut Wrapper<PersonEntryParent> {
    static COMPONENT: LocalStatic<Wrapper<PersonEntryParent>> = LocalStatic::new();
    COMPONENT.get_mut_or_init(|| {
        let child = &mut registry_person_child().instance;
        Wrapper::new(PersonEntryParent {
            instance: BzdComponentsParent::new(BzdComponentsParentContext::<
                BzdComponentsParentContextConstraintTypes<BzdComponentsChild>,
            >::new("Bob", 42, [child])),
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
    let person_child = registry_person_child();
    let person_parent = registry_person_parent();

    block_on(person_child.init())?;
    block_on(person_parent.init())?;

    block_on(person_child.instance.print_info())?;
    block_on(person_child.instance.print_info())?;
    block_on(person_parent.instance.print_info())?;

    block_on(person_child.shutdown())?;
    block_on(person_parent.shutdown())?;

    Ok(())
}
