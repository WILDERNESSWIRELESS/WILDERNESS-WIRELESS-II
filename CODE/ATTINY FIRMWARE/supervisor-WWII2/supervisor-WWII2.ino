#include <SoftwareSerial.h>
#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management
#include <avr/wdt.h>      // Watchdog timer

#define VOK_PIN       0
#define VLO_PIN       1
#define SUP_PIN       3 
#define SHD_PIN       4 
#define WAK_PIN       2 
#define ON_THRESH  3600
#define OFF_THRESH 3400

int f_wdt = 0;
long batteryVoltage = 0;
int serverUp = 0;

void setup() {
  
  pinMode(VOK_PIN, OUTPUT);
  digitalWrite(VOK_PIN, LOW);
  
  pinMode(VLO_PIN, OUTPUT);
  digitalWrite(VLO_PIN, LOW);
  
  pinMode(SUP_PIN, INPUT);
  
  pinMode(WAK_PIN, OUTPUT);
  digitalWrite(WAK_PIN, LOW);
  
  pinMode(SHD_PIN, OUTPUT);
  digitalWrite(SHD_PIN, HIGH);
  
  enterSleep();
}

void loop() {
  wdt_reset();
  batteryVoltage = readVcc();
  serverUp = digitalRead(SUP_PIN);
  
  if (batteryVoltage <= OFF_THRESH ) {
    //SIGNAL RPI TO POWEROFF
    digitalWrite(SHD_PIN, LOW);
    delay(7000);
    // Turn HIGH SIDE SWITCH OFF
    digitalWrite(WAK_PIN, LOW);
  }

 if(batteryVoltage >= ON_THRESH){
  // TURN HIGH SIDE SWITCH ON
  digitalWrite(SHD_PIN, HIGH);
  digitalWrite(WAK_PIN, HIGH);
 }
 
  enterSleep();
}

void enterSleep(void) {
  noInterrupts ();       // timed sequence coming up
  // pat the dog
  wdt_reset();
  // clear various "reset" flags
  MCUSR = 0;
  // allow changes, disable reset, clear existing interrupt
  WDTCR = bit (WDCE) | bit (WDE) | bit (WDIF);
  // set interrupt mode and an interval (WDE must be changed from 1 to 0 here)
  WDTCR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable ();       // ready to sleep
  interrupts ();         // interrupts are required now
  sleep_mode ();          // sleep
  sleep_disable ();      // precaution
}

ISR(WDT_vect) {
  heartbeat();
}

void heartbeat() {
  
  // Message Voltage >= THRESH
  
  if(batteryVoltage >= ON_THRESH){
      digitalWrite(VOK_PIN, HIGH);
      delay(500);
      digitalWrite(VOK_PIN, LOW);
      delay(100);
  }
  
  // Message Voltage <= THRESH
  
  if(batteryVoltage <= OFF_THRESH){
      digitalWrite(VLO_PIN, HIGH);
      delay(500);
      digitalWrite(VLO_PIN, LOW);
      delay(500);
  }
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high << 8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

