#![no_std]

use esp_hal::gpio::{Input, InputConfig, Level, Output, OutputConfig, Pull};

pub struct Gpio {
    button: Input<'static>,
}

impl Gpio {
    pub fn new(button_pin: impl esp_hal::gpio::InputPin + 'static) -> Self {
        let config = InputConfig::default().with_pull(Pull::Up);
        let button = Input::new(button_pin, config);
        Self { button }
    }

    pub fn is_button_pressed(&self) -> bool {
        self.button.is_low()
    }
}

pub struct Led {
    led: Output<'static>,
}

impl Led {
    pub fn new(led_pin: impl esp_hal::gpio::OutputPin + 'static) -> Self {
        let led = Output::new(led_pin, Level::High, OutputConfig::default());
        Self { led }
    }

    pub fn on(&mut self) {
        self.led.set_low();
    }

    pub fn off(&mut self) {
        self.led.set_high();
    }

    pub fn toggle(&mut self) {
        self.led.toggle();
    }
}
