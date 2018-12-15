//
#define F_CPU 1000000UL  //CPU freq. set to default 1 MHz, also done in program settings
#include <avr/io.h> //pin and register macros
#include <util/delay.h> //delay function delays for the correct time
#include <avr/interrupt.h> //interrupt macros, pin change interrupts are one way to awake the Tiny85 from sleep
#include <avr/sleep.h> //Will be used to dramatically save battery life

volatile int OperatingMode = 0; // interrupt value to denote state of LED
int ButtonPin = 4;  //Using Pin 3 as Button Press Input Pin, but labeled PB4 on AtTiny85
int LEDPin = 0;
unsigned long previousTime = 0;   // Records the last time stamp that the LED changed state
unsigned long currentTime = 0; // Records the time of the current LED State Change
int LEDState = LOW;

void setup() {
  DDRB  = 0b00000001;    // PB0 is set as an output
  PORTB = 0b11101110;
  cli(); // disable interrupts outside of loop

  GIMSK = 0b00100000;    // turns on pin change interrupts
  PCMSK = 0b00010000;    // turn on interrupts on pin PB4 (Pin 3, my button input pin)
  ADCSRA &= ~_BV(ADEN);  // ADC off
  sei();                 // enables interrupts


}

void loop() {
  unsigned long currentTime = millis();
  switch (OperatingMode) {
    case 0: //Off
      analogWrite(LEDPin, 0);
      set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Most Power Savings of the 5 Sleep Modes
      cli();
      sleep_enable();
      sei();
      sleep_cpu();
      break;
    case 1: //Brightest   Change order for better,  Natural feel?
      analogWrite(LEDPin, 255);
      sleep_disable();
      break;
    case 2: //Intermediate Brightest
      analogWrite(LEDPin, 128);
      break;
    case 3: //Least Bright
      analogWrite(LEDPin, 64);
      break;
    case 4: //Flashing @ 2 Hz
      if (currentTime - previousTime >= 250) { // for 2 Hz need a 250 ms delay between switches, to give a 500 ms period
        previousTime = currentTime; // Update with LED State Change
        if (LEDState == LOW) {
          LEDState = HIGH;
        } else {
          LEDState = LOW;
        }
        digitalWrite(LEDPin, LEDState);
      }

      break;
  }
}

// Interrupt Function, source: https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/
ISR(PCINT0_vect) {
  if (digitalRead(ButtonPin)) {
    OperatingMode = OperatingMode + 1;
    if (OperatingMode > 4) {
      OperatingMode = 0;
    }
  }
}


// Code Sources:
//https://embeddedthoughts.com/2016/06/06/attiny85-introduction-to-pin-change-and-timer-interrupts/
//https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/
//https://playground.arduino.cc/Learning/arduinoSleepCode
//https://www.instructables.com/id/ATTiny-Port-Manipulation/
//https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay


