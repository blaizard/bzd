#![no_std]
#![feature(asm_experimental_arch)]

/// Enables the floating-point coprocessor.
///
/// `esp-hal` enables the `float-save-restore` feature by default, which makes
/// `xtensa-lx-rt` save the FP registers with `ssi` in its exception context.
/// That code assumes CP0 is already enabled, but CPENABLE is zero after reset.
/// The first interrupt then raises a `Coprocessor0 disabled` exception, which
/// turns into a double exception and recurses until the stack leaves DRAM.
///
/// TODO: Remove this workaround once `xtensa-lx-rt` enables CPENABLE while
/// saving the floating-point context, or `esp-hal` enables it during init.
pub fn enable() {
    unsafe {
        core::arch::asm!("wsr.cpenable {0}", "rsync", in(reg) 1u32, options(nostack));
    }
}
