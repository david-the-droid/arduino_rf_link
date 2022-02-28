/* Description: Aircraft receive program
 * Author: David Anderson
 * Last modified: 12/12/21
 */

//** Library includes **//

#include <SPI.h>              // SPI library used by RF module
#include <RF24.h>             // Main radio library
#include <Servo.h>            // Servo library

//** Pre-processor defines **//

#define DEBUG_PRINT           // Controls the debug print statement enable (comment out to disable)

//** Digital I/O defines **//  

#define RUDDER_PIN 3
#define ELEVATOR_PIN 4
#define MOTOR_PIN 5
#define RADIO_CE_PIN 9 
#define RADIO_CSN_PIN 10
#define BAUDRATE 115200

//** Defines for external devices **//

Servo rudder; // create servo types
Servo elevator; 
Servo motor; 

RF24 aircraft_radio(RADIO_CE_PIN, RADIO_CSN_PIN); // CE, CSN


//** Data structures **//

struct data_package
{
  int rudder_out; 
  int elevator_out; 
  int motor_out; 
} data;


void setup() 
{
  radio_setup(aircraft_radio);
  servo_setup( rudder, elevator, motor, RUDDER_PIN, ELEVATOR_PIN, MOTOR_PIN ); 

  #ifdef DEBUG_PRINT
  Serial.begin(BAUDRATE);
  Serial.println("Initialisation complete");
  #endif
}

void loop()              //MAIN LOOP 
{
 if(aircraft_radio.available()){ 

    aircraft_radio.read(&data, sizeof(data_package));
    rudder.write(data.rudder_out);
    elevator.write(data.elevator_out); 
    motor.write(data.motor_out); 
    
    #ifdef DEBUG_PRINT
    Serial.print("Value for motor output is : ");
    Serial.println(data.motor_out);
    Serial.print("Value for rudder output is : ");
    Serial.println(data.rudder_out);
    Serial.print("Value for elevator output is : ");
    Serial.println(data.elevator_out);
    #endif
 
 }

}

void radio_setup( RF24 radio_dev )
{
  const uint64_t pipe = 1;
  radio_dev.begin();                  //Starting the Wireless communication
  radio_dev.openReadingPipe(1, pipe);
  radio_dev.setChannel(0x76);         //Sets the channel
  radio_dev.setPALevel(RF24_PA_MIN);  //Power level 
  radio_dev.enableDynamicPayloads();
  radio_dev.powerUp();                //Turn RF module on 
  radio_dev.startListening();         //This sets the module as receiver
}

void servo_setup( Servo R, Servo E, Servo M, int rudder_pin, int elevator_pin, int motor_pin )
{
  R.attach(rudder_pin); // Attaches pins to servo objects
  E.attach(elevator_pin); 
  M.attach(motor_pin);
  M.writeMicroseconds(1000);
}
