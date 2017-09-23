#include <SoftwareSerial.h>
#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management
#include <avr/wdt.h>      // Watchdog timer

#define VOK_PIN       0
#define VLO_PIN       1
#define SUP_PIN       3
#define SHD_PIN       4
#define WAK_PIN       2
#define ON_THRESH  3650
#define OFF_THRESH 3500

int f_wdt = 0;
long batteryVoltage = 0;
bool serverUp = 0;
bool 

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
  
  // RESET WATCHDOG TIMER
  wdt_reset();

  // CHECK BATTERY VOLTAGE
  batteryVoltage = readVcc();

  // WRITE TO STATUS LEDS
  updateStatus();
  
  // CHECK TO SEE IF RPI IS UP
  serverUp = digitalRead(SUP_PIN);
  
  // IF VOLTAGE IS LOW, START SHUTDOWN PROCEDURE
  if (batteryVoltage <= OFF_THRESH ) {
    
    //IF RPI is UP, SHUTDOWN
    if(serverUp == TRUE){
        digitalWrite(SHD_PIN, LOW);
        delay(7000);
    }
    
    // IF RPI is DOWN, REMOVE POWER
    if(serverUp == FALSE){
        digitalWrite(WAK_PIN, LOW);
    }
    
  }
  
  // IF BATTERY VOLTAGE IS GOOD, START STARTUP PROCEDURE
  if(batteryVoltage >= ON_THRESH){
    
    // BRING THE SHUTDOWN LINE HIGH
    digitalWrite(SHD_PIN, HIGH);

    // APPLY POWER TO RPI
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
  // do nothing
}

void updateStatus() {
  
  // Message Voltage >= THRESH
  
  if(batteryVoltage >= ON_THRESH){
      digitalWrite(VOK_PIN, HIGH);
      digitalWrite(VLO_PIN, LOW);
  }
  
  // Message Voltage <= THRESH
  
  if(batteryVoltage <= OFF_THRESH){
      digitalWrite(VLO_PIN, HIGH);
      digitalWrite(VOK_PIN, LOW);
  }

  // Message Voltage between ON_ and OFF_ THRESH 

  if(batteryVoltage < ON_THRESH && batteryVoltage > OFF_THRESH){
    digitalWrite(VOK_PIN, HIGH);
    digitalWrite(VLO_PIN, HIGH);
  }
  
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference

  ADMUX = _BV(MUX3) | _BV(MUX2);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high << 8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

