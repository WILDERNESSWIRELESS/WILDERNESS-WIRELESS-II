#include <SoftwareSerial.h>
#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management
#include <avr/wdt.h>      // Watchdog timer

#define VOK_PIN       0
#define VLO_PIN       1
#define WAK_PIN       2
#define SUP_PIN       3
#define SHD_PIN       4
#define ON_THRESH  4000
#define OFF_THRESH 3500

int f_wdt = 0;
long batteryVoltage = 0;
boolean serverUp = false;
boolean systemState = 0; // 0 = off, 1 = on
int chargeStatus = 0; // above ON_THRESH = 2, btw ON_ and OFF_THRESH = 1, below OFF_THRESH = 0
int upCounter = 0;

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

//////////////// MAIN LOOP

void loop() {
  
  // RESET WATCHDOG TIMER
  wdt_reset();

  // CHECK BATTERY VOLTAGE
  batteryVoltage = readVcc();

  // DETERMINE CHARGE STATE
  chargeStatus = getChargeStatus(batteryVoltage);

  // WRITE TO STATUS LEDS
  indicateStatus(chargeStatus);
  
  // CHECK TO SEE IF RPI IS UP
  serverUp = digitalRead(SUP_PIN);

  // DECISIONS ABOUT STATE
  
  // IF VOLTAGE IS LOW, START SHUTDOWN PROCEDURE
  if (chargeStatus == 0 && upCounter > 1) {
    
    //IF RPI is UP, SHUTDOWN
    //if(serverUp == true){
        digitalWrite(SHD_PIN, LOW);
        delay(7000);
    //}
    
    // IF RPI is DOWN, REMOVE POWER
    //if(serverUp == false){
        digitalWrite(WAK_PIN, LOW);
    //}
    
  }
  
  // IF BATTERY VOLTAGE IS GOOD, START STARTUP PROCEDURE
  if(chargeStatus == 2 && upCounter > 1){

    // IS THE RPI DOWN?

    //if(serverUp = false){
    
      // BRING THE SHUTDOWN LINE HIGH
      digitalWrite(SHD_PIN, HIGH);

      // APPLY POWER TO RPI
      digitalWrite(WAK_PIN, HIGH);
    //}
  }

  // CATCH STRANGE BEHAVIOR

  if(chargeStatus == 0 && serverUp == true){
    // to do
  }

  // GO TO SLEEP FOR A BIT TO SAVE POWER
  
  delay(100);
  lightsOff();
  enterSleep();
  
}

//////////////// END MAIN LOOP

// ENTER SLEEP

void enterSleep(void) {
  noInterrupts ();       // timed sequence coming up
  // pat the dog
  wdt_reset();
  // clear various "reset" flags
  MCUSR = 0;
  // allow changes, disable reset, clear existing interrupt
  WDTCR = bit (WDCE) | bit (WDE) | bit (WDIF);
  // set interrupt mode and an interval (WDE must be changed from 1 to 0 here)
  //WDTCR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  WDTCR = bit (WDIE) | bit (WDP3);    // set WDIE, and 4 seconds delay
  //WDTCR = bit (WDIE) | bit (WDP2) | bit (WDP1) | bit(WDP0);    // set WDIE, and 2 seconds delay
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable ();       // ready to sleep
  interrupts ();         // interrupts are required now
  sleep_mode ();          // sleep
  sleep_disable ();      // precaution
}

// WATCHDOG INTERRUPT

ISR(WDT_vect) {
  upCounter++;
}

// GET CHARGE STATUS

int getChargeStatus(long v){
  
  int state = 0;
  
  if (v <= OFF_THRESH ) {
    state = 0;
  }

  if(v > OFF_THRESH && v < ON_THRESH){
    state = 1;
  }

  if(v >= ON_THRESH){
    state = 2;
  }
  return state;
}

// INDICATE STATUS ON STATUS LEDS

void indicateStatus(int s) {
  
  // Message Voltage >= THRESH
  
  if(s == 2){
      digitalWrite(VOK_PIN, HIGH);
      digitalWrite(VLO_PIN, LOW);
  }

  // Message Voltage between ON_ and OFF_ THRESH 

  if(s == 1){
    digitalWrite(VOK_PIN, HIGH);
    digitalWrite(VLO_PIN, HIGH);
  }
  
  // Message Voltage <= THRESH
  
  if(s == 0){
      digitalWrite(VLO_PIN, HIGH);
      digitalWrite(VOK_PIN, LOW);
  }

}

// TURN LEDS OFF

void lightsOff(){
  digitalWrite(VLO_PIN, LOW);
  digitalWrite(VOK_PIN, LOW);
}

// MEASURE VOLTAGE

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

