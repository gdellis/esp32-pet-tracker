#![no_std]
#![no_main]

use esp_hal::clock::CpuClock;
use esp_hal::main;
use esp_hal::time::{Duration, Instant};

mod lib;

use lib::{Gpio, Led};

#[panic_handler]
fn panic(_: &core::panic::PanicInfo) -> ! {
    loop {}
}

esp_bootloader_esp_idf::esp_app_desc!();

#[main]
fn main() -> ! {
    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
    let peripherals = esp_hal::init(config);

    let mut led = Led::new(peripherals.GPIO48);
    let gpio = Gpio::new(peripherals.GPIO0);

    led.off();
    let mut led_state = false;

    loop {
        if gpio.is_button_pressed() {
            led_state = !led_state;
            if led_state {
                led.on();
            } else {
                led.off();
            }
        }

        let delay_start = Instant::now();
        while delay_start.elapsed() < Duration::from_millis(50) {}
    }
}
