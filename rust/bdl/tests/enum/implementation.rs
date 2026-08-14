#![no_std]
#![no_main]

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use rust_bdl_tests_enum_interface::BzdComponentsEspUartDevice;

    #[test]
    fn test_enum_variants() -> TestResult {
        assert_eq!(BzdComponentsEspUartDevice::Uart0 as u8, 0)?;
        assert_eq!(BzdComponentsEspUartDevice::Uart1 as u8, 1)?;
        assert_eq!(
            BzdComponentsEspUartDevice::Uart0 == BzdComponentsEspUartDevice::Uart0,
            true
        )?;
        assert_eq!(
            BzdComponentsEspUartDevice::Uart0 == BzdComponentsEspUartDevice::Uart1,
            false
        )?;
        Ok(())
    }
}
