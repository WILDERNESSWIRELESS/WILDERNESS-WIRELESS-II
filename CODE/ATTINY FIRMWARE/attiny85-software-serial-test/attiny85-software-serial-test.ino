

// HAVE TO USE 1200 BAUD IN TERMINAL

#include <SoftwareSerial.h>


#define VOK_PIN       0
#define VLO_PIN       1
#define WAK_PIN       2
#define SUP_PIN       3
#define SHD_PIN       4
#define ON_THRESH  4000
#define OFF_THRESH 3500


SoftwareSerial mySerial(VOK_PIN, VLO_PIN);

void setup() {

  mySerial.begin(9600);

}

//////////////// MAIN LOOP

void loop() {
  

  //DEBUG MSG
  mySerial.println("I'm awake...");
delay(100);
  
}

//////////////// END MAIN LOOP




