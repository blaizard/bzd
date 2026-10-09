struct CriticalSection;

critical_section::set_impl!(CriticalSection);

unsafe impl critical_section::Impl for CriticalSection {
    unsafe fn acquire() -> critical_section::RawRestoreState {
        Default::default()
    }

    unsafe fn release(_: critical_section::RawRestoreState) {}
}
