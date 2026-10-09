use child::{BzdComponentsChild, BzdComponentsChildContext, BzdComponentsPersonInterface};
use component::{CompositionComponent, Lifecycle, StaticComponent};
use critical_section as _;
use parent::{
    BzdComponentsParent, BzdComponentsParentContext, BzdComponentsParentContextConstraintTypes,
};

// ---- Registry

struct PersonEntryChild {
    component: CompositionComponent<BzdComponentsChild>,
}

impl PersonEntryChild {
    fn get() -> &'static Self {
        static COMPONENT: StaticComponent<PersonEntryChild> = StaticComponent::new();
        COMPONENT.get_or_init(|| Self {
            component: CompositionComponent::new(BzdComponentsChild::new(
                BzdComponentsChildContext {
                    age: 28,
                    username: "Alex",
                },
            )),
        })
    }
}

#[allow(unused_variables)]
impl Lifecycle for PersonEntryChild {
    async fn init(&self) -> Result<(), bzd::base::error::Error> {
        self.component
            .acquire(async |component| {
                println!("[child] init");
                Ok(())
            })
            .await?;
        Ok(())
    }

    async fn shutdown(&self) -> Result<(), bzd::base::error::Error> {
        self.component
            .release(async |component| {
                println!("[child] shutdown");
                Ok(())
            })
            .await?;
        Ok(())
    }
}

struct PersonEntryParent {
    component: CompositionComponent<
        BzdComponentsParent<BzdComponentsParentContextConstraintTypes<BzdComponentsChild>>,
    >,
}

impl PersonEntryParent {
    fn get() -> &'static Self {
        static COMPONENT: StaticComponent<PersonEntryParent> = StaticComponent::new();
        COMPONENT.get_or_init(|| Self {
            component: CompositionComponent::new(BzdComponentsParent::<
                BzdComponentsParentContextConstraintTypes<BzdComponentsChild>,
            >::new(BzdComponentsParentContext::<
                BzdComponentsParentContextConstraintTypes<BzdComponentsChild>,
            >::new(
                "Bob",
                42,
                [PersonEntryChild::get().component.handle()],
            ))),
        })
    }
}

#[allow(unused_variables)]
impl Lifecycle for PersonEntryParent {
    async fn init(&self) -> Result<(), bzd::base::error::Error> {
        self.component
            .acquire(async |component| {
                println!("[parent] init");
                Ok(())
            })
            .await?;
        Ok(())
    }

    async fn shutdown(&self) -> Result<(), bzd::base::error::Error> {
        self.component
            .release(async |component| {
                println!("[parent] shutdown");
                Ok(())
            })
            .await?;
        Ok(())
    }
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
    let person_child = PersonEntryChild::get();
    let person_parent = PersonEntryParent::get();

    block_on(person_child.init())?;
    block_on(person_parent.init())?;

    block_on(async { person_child.component.lock().await.print_info().await })?;
    block_on(async { person_child.component.lock().await.print_info().await })?;
    block_on(async { person_parent.component.lock().await.print_info().await })?;

    block_on(person_child.shutdown())?;
    block_on(person_parent.shutdown())?;

    Ok(())
}
