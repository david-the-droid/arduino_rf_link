/* Description: Controller transmitter for RF link
 * Author: David Anderson
 * Last modified: 12/12/21
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

#define TIME_DELAY 100

#define THROTTLE_UPPER_LIM 1700 // Upper throttle limit for the flitest Radial 1806 2280kV, must not send a value greater than 1700
#define THROTTLE_LOWER_LIM 900  // Lower throttle limit for the flitest Radial 1806 2280kV, minimum value needed for ESC to arm itself

RF24 controller_radio(RADIO_CE_PIN, RADIO_CSN_PIN); // CE, CSN         

//** Data structures **//

struct data_plane_control_package{
  int rudder_out; 
  int elevator_out;  
  int motor_out; 
} data;

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
  data.rudder_out = analogRead(RUDDER_CONTROL_PIN); // read the current analogue pin value
  data.elevator_out = analogRead(ELEVATOR_CONTROL_PIN); 
  data.motor_out = analogRead(MOTOR_CONTROL_PIN); 
  
  data.rudder_out = map(data.rudder_out, 0, 1023, 0, 180); // remap value to required value
  data.elevator_out = map(data.elevator_out, 0, 1023, 10, 140);
  data.motor_out = map(data.motor_out, 0, 300, 880, 1700); 

  if (data.motor_out > THROTTLE_UPPER_LIM)
  {
    data.motor_out = THROTTLE_UPPER_LIM;
  }

  if (data.motor_out < THROTTLE_LOWER_LIM)
  {
    data.motor_out = THROTTLE_LOWER_LIM; 
  }
  
  #ifdef DEBUG_PRINT
  Serial.print("Rudder value: ");
  Serial.println(data.rudder_out);
  Serial.print("Elevator value: ");
  Serial.println(data.elevator_out);
  Serial.print("Motor value: ");
  Serial.println(data.motor_out);
  #endif
  
  controller_radio.write(&data, sizeof(data_plane_control_package));//Sending payload to receiver
  delay(TIME_DELAY); // Time duration until next command is sent

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
