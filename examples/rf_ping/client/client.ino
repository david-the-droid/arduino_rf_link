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
#define DEBUG_PACKET_INFO

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

struct data_transmit_packet{
  int rudder_out; 
  int elevator_out;  
  int motor_out; 
} data_tx;

struct data_receive_packet{
  int rudder_out; 
  int elevator_out;
  int motor_out;
} data_rx;

//** Test variables **//

unsigned long print_previous_millis = 0; // time will count in millis
unsigned long current_millis = 0; // Read the current value


void setup() 
{
  radio_setup(controller_radio); // Configure the controller_radio
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
      controller_radio.stopListening();          
      
      // step 2. Read the analog values from control input
      data_tx.rudder_out = analogRead(RUDDER_CONTROL_PIN); // read the current analogue pin value
      data_tx.elevator_out = analogRead(ELEVATOR_CONTROL_PIN); 
      data_tx.motor_out = analogRead(MOTOR_CONTROL_PIN); 
      
      data_tx.rudder_out = map(data_tx.rudder_out, 0, 1023, 0, 180); // remap value to required value
      data_tx.elevator_out = map(data_tx.elevator_out, 0, 1023, 10, 140);
      data_tx.motor_out = map(data_tx.motor_out, 0, 300, 880, 1700); 
    
      if (data_tx.motor_out > THROTTLE_UPPER_LIM)
      {
        data_tx.motor_out = THROTTLE_UPPER_LIM;
      }
    
      if (data_tx.motor_out < THROTTLE_LOWER_LIM)
      {
        data_tx.motor_out = THROTTLE_LOWER_LIM; 
      }
      
      #if defined(DEBUG_PRINT) && defined(DEBUG_PACKET_INFO)
      Serial.println("Sending packet");
      Serial.print("Rudder value: ");
      Serial.println(data_tx.rudder_out);
      Serial.print("Elevator value: ");
      Serial.println(data_tx.elevator_out);
      Serial.print("Motor value: ");
      Serial.println(data_tx.motor_out);
      #endif
          
      // step 3. Send radio payload
      controller_radio.write(&data_tx, sizeof(data_transmit_packet));
      
      // Step 4. Switch the radio back to listening mode
      controller_radio.startListening();

      // Step 5. Log the current milli second and await another 100mS  
      print_previous_millis = current_millis;
          
    }

  // step 1. Is radio packet available
  if(controller_radio.available())
   { 
      // step 2. (if above true) unpack data
      controller_radio.read(&data_rx, sizeof(data_receive_packet));

      if (data_rx.motor_out > THROTTLE_UPPER_LIM)
      {
        data_rx.motor_out = THROTTLE_UPPER_LIM;
      }
      if (data_rx.motor_out < THROTTLE_LOWER_LIM)
      {
        data_rx.motor_out = THROTTLE_LOWER_LIM;
      } 
      
      #ifdef DEBUG_PRINT
      Serial.print("Value for motor output is : ");
      Serial.println(data.motor_out);
      Serial.print("Value for rudder output is : ");
      Serial.println(data.rudder_out);
      Serial.print("Value for elevator output is : ");
      Serial.println(data.elevator_out);
      #endif

      // Step 3. Switch radio back to listening mode
      controller_radio.startListening();  
  }
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
  radio_dev.startListening();
}
