//#################################
//#################################

//transmitter code

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(4, 5);  // CE, CSN

byte address[6] = "822081"; //maybe an address that's not 000001 will help

#define joy1_x 26
#define joy1_y 27
//#define but1 13

#define joy2_x 14
#define joy2_y 12
//#define but2 25

#define joy3_x 33
#define joy3_y 32
//#define but3 35

#define slde1 39
#define slde2 34




void setup() {

  //initializing pins
  // Joystick 1
  pinMode(joy1_x, INPUT);
  pinMode(joy1_y, INPUT);
  //pinMode(but1, INPUT);

  // Joystick 2
  pinMode(joy2_x, INPUT);
  pinMode(joy2_y, INPUT);
  //pinMode(but2, INPUT);

  //joystick 3
  pinMode(joy3_x, INPUT);
  pinMode(joy3_y, INPUT);
  //pinMode(but3, INPUT);

  //slide switches
  pinMode(slde1, INPUT);
  pinMode(slde2, INPUT);

  // begining the module/library
  if (!radio.begin()) {
  Serial.println(F("radio hardware not responding!"));
  while (1) {} // hold program in infinite loop to prevent subsequent errors
  }

  Serial.begin(115200);
  Serial.println("start");

  //the RF module's power level:
  //Options: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setPALevel(RF24_PA_MAX);

  //setting this RF module to transmit mode:
  radio.stopListening();

  // Open the writing pipe with the defined address
  // This is the address the data will be sent to
  // use a different address if you wanna send the data to a different module.
  radio.openWritingPipe(address);

}

struct joystickData{
  int joystick1_x;
  int joystick1_y;
//  int Button1;

  int joystick2_x;
  int joystick2_y;
//  int Button2;

  int joystick3_x;
  int joystick3_y;
//  int Button3;
};

struct slideData{
  int slide1;
  int slide2;
};

joystickData joystickPayload;
slideData slidePayload;

void loop() {

  // Joystick 1
  joystickPayload.joystick1_x = analogRead(joy1_x);
  joystickPayload.joystick1_y = analogRead(joy1_y);
  //joystickPayload.Button1 = digitalRead(but1);

  // Joystick 2
  joystickPayload.joystick2_x = analogRead(joy2_x);
  joystickPayload.joystick2_y = analogRead(joy2_y);
  //joystickPayload.Button2 = digitalRead(but2);

  //joystick 3
  joystickPayload.joystick3_x = analogRead(joy3_x);
  joystickPayload.joystick3_y = analogRead(joy3_y);
  //joystickPayload.Button3 = digitalRead(but3);

  slidePayload.slide1 = digitalRead(slde1);
  slidePayload.slide2 = digitalRead(slde2);


  Serial.print(joystickPayload.joystick1_x);
  Serial.print(",");
  Serial.print(joystickPayload.joystick1_y);
  //Serial.print(",");
  //Serial.print(joystickPayload.Button1);
  Serial.println("  ");

  Serial.print(joystickPayload.joystick2_x);
  Serial.print(",");
  Serial.print(joystickPayload.joystick2_y);
  //Serial.print(",");
  //Serial.print(joystickPayload.Button2);
  Serial.println("  ");

  Serial.print(joystickPayload.joystick3_x);
  Serial.print(",");
  Serial.print(joystickPayload.joystick3_y);
  //Serial.print(",");
  //Serial.print(joystickPayload.Button3);
  Serial.println("  ");

  Serial.print(slidePayload.slide1);
  Serial.print(",");
  Serial.print(slidePayload.slide2);
  Serial.println("  ");

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");

  radio.write(&joystickPayload, sizeof(joystickPayload));
  delay(20);
  radio.write(&slidePayload, sizeof(slidePayload));

  Serial.print("joystick payload: "); Serial.println(sizeof(joystickPayload));
  Serial.print("slide switch payload: "); Serial.println(sizeof(slidePayload));

  delay(20);
}