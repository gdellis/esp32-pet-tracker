#[cfg(test)]
mod tests {
    #[test]
    fn build_lib() {
        // Verify the library compiles - this is a compile-only test
        // In no_std context, we can't run actual tests without hardware
        // This test ensures the code at least compiles
        assert!(true);
    }
}
