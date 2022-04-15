/* Description: Client, sends pings to receiver and counts how many packets are received back
 * Author: David Anderson
 * Last modified: 15/04/22
 */


// NOTES
/*
 */

//** Library includes **//

#include <SPI.h>          // SPI library used by RF module
#include <RF24.h>         // Main radio library

//** Pre-processor defines **//

#define DEBUG_PRINT // comment out to disable debug print

//** Digital defines **//

#define RUDDER_CONTROL_PIN 0
#define ELEVATOR_CONTROL_PIN 1
#define MOTOR_CONTROL_PIN 6
#define RADIO_CE_PIN 9 
#define RADIO_CSN_PIN 10
#define BAUDRATE 115200

//** Defines for external devices **//

#define PRINT_DELAY 100 // mS 

#define THROTTLE_UPPER_LIM 1700 // Upper throttle limit for the flitest Radial 1806 2280kV, must not send a value greater than 1700
#define THROTTLE_LOWER_LIM 900  // Lower throttle limit for the flitest Radial 1806 2280kV, minimum value needed for ESC to arm itself

RF24 controller_radio(RADIO_CE_PIN, RADIO_CSN_PIN); // CE, CSN   

//** Data structures **//

struct data_plane_control_package{
  int rudder_out; 
  int elevator_out;  
  int motor_out; 
} data;

//** Test variables **//

unsigned long print_previous_millis = 0; // time will count in millis
unsigned long current_millis = 0; // Read the current value


void setup() 
{
  radio_setup( controller_radio ); // Configure the controller_radio
  #ifdef DEBUG_PRINT
  Serial.begin(BAUDRATE);
  Serial.println("Initialisation complete");
  #endif
  
}

void loop() 
{
  current_millis = millis();

  if (current_millis - print_previous_millis >= PRINT_DELAY) // send approx 10 times a second
    {
      // here a packet needs to be sent

      // step 1. Switch the radio into transmitter mode
      // step 2. Read the analog values from control input
      // step 3. Send radio payload
      // Step 4. Switch the radio back to listening mode
     
      Serial.println("Sending packet");
      print_previous_millis = current_millis;
      
    }

  // step 1. Is radio packet available
  // step 2. (if above true) unpack data and count/display received packet
  // step 3. Switch the radio back to transmissions mode
}


void radio_setup( RF24 radio_dev )
{
  const uint64_t pipe = 1;            // Address through which two modules communicate
  radio_dev.begin();                  // Starting the Wireless communication
  radio_dev.openWritingPipe(pipe);    // 
  radio_dev.setChannel(0x76);         // Sets the channel as 0x76 = 118, add this 118 to 2400MHz and its running on channel 2518MHz (channels have 1MHz gaps)
  radio_dev.setPALevel(RF24_PA_MIN);  // You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  radio_dev.enableDynamicPayloads();  
  radio_dev.powerUp();
  radio_dev.stopListening();          // This sets the module as transmitter
}
