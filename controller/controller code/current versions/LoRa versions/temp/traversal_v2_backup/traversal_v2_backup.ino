#include <SPI.h>
#include <LoRa.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

uint8_t broadcastAddress[] = {0xEC, 0x64, 0xC9, 0x5E, 0x11, 0x3C}; //reciever's MAC address, (this is sender's code)

uint8_t joystick1_x = 0;
uint8_t joystick1_y = 0;
uint8_t Button1 = 0;

uint8_t joystick2_x = 0;
uint8_t joystick2_y = 0;
uint8_t Button2 = 0;

uint8_t joystick3_x = 0;
uint8_t joystick3_y = 0;
uint8_t Button3 = 0;

uint8_t mode_select = 0;
uint8_t slide2 = 0;


uint8_t payload[11];

esp_now_peer_info_t slave;

int speed = 255;
int delay_rate = 2; //delay between each gradual increase step

int gradual_increase_flag = 0;

// Pin definitions for LoRa SX1278
#define ss 22         // Slave Select (NSS)
#define rst 21       // Reset
#define dio0 5       // Digital I/O 0

#define P2 32
#define D2 33

#define P1 25
#define D1 26

void gradual_speed(int final_speed, int delay_rate, bool ifOn = 0) {
  if (ifOn && gradual_increase_flag){
    for (int speed = 0; speed < final_speed; speed++) {
      analogWrite(P1, speed);
      analogWrite(P2, speed);
      //Serial.println(speed);
      delay(delay_rate);
    }
  }
  analogWrite(P1, final_speed);
  analogWrite(P2, final_speed);

  gradual_increase_flag = 0;
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Transmission Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Receiver Setup");

  // Configure LoRa module
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) { // Set frequency to 433 MHz
    Serial.println("Starting LoRa failed! Check connections and power supply.");
    while (1);
  }

  Serial.println("LoRa initialization successful!");


  pinMode(P1, OUTPUT);
  pinMode(P2, OUTPUT);

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  assignFromPayload(payload);
  resetPayload(payload);

  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  
  //Register peer
  memcpy(slave.peer_addr, broadcastAddress, 6);
  slave.channel = 0;
  slave.encrypt = false;

  if (esp_now_add_peer(&slave) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}


void loop() {
  // Try to parse a packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.println("###Received packet###");

    // Read packet contents 
    int i = LoRa.available();
    while (i) {
      //Serial.println(i);
      payload[i-1] = LoRa.read();
      //Serial.println(payload[i-1]);
      i--;
    }

    // Print RSSI (signal strength)
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
    Serial.println(LoRa.packetFrequencyError());

    //sending the data through esp now to robotic arm
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &payload, sizeof(payload));

    if (result == ESP_OK) {
    Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
  }

  assignFromPayload(payload);

  //traversal code
  //printing the joystick values: - comment this if the rx tx port is needed
  Serial.println("payload:");
  Serial.print(joystick1_x); Serial.print(", "); Serial.print(joystick1_y);  Serial.print(", "); Serial.println(Button1);
  Serial.print(joystick2_x); Serial.print(", "); Serial.print(joystick2_y);  Serial.print(", "); Serial.println(Button2);
  Serial.print(joystick3_x); Serial.print(", "); Serial.print(joystick3_y);  Serial.print(", "); Serial.println(Button3);
  Serial.print(mode_select); Serial.print(", "); Serial.println(slide2);

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");

  if (joystick1_x <= 5) {
    Serial.println("left");
    left(speed);
  }

  else if (joystick1_x >= 250) {
    Serial.println("right");
    right(speed);
  }
  
  else if (joystick1_y >= 250) {
    Serial.println("backward");
    backward(speed);
  }

  else if (joystick1_y <= 5) {
    Serial.println("forward");
    forward(speed);
  }

  else {
    Serial.println("stopped");
    stop();
  }
}

void assignFromPayload(uint8_t payload[11]) {
  slide2 = payload[0];
  mode_select = payload[1];

  Button3 = payload[2];
  joystick3_y = payload[3];
  joystick3_x = payload[4];

  Button2 = payload[5];
  joystick2_y = payload[6];
  joystick2_x = payload[7];

  Button1 = payload[8];
  joystick1_y = payload[9];
  joystick1_x = payload[10];
}

void resetPayload(uint8_t payload[11]) {
  payload[0] = 0;
  payload[1] = 0;
  payload[2] = 0;
  payload[3] = 128;
  payload[4] = 128;
  payload[5] = 0;
  payload[6] = 128;
  payload[7] = 128;
  payload[8] = 0;
  payload[9] = 128;
  payload[10] = 128;
}


void forward(uint8_t speed) { //add a speed argument when speed change is decided
  //speed = map(speed, 200, 255, 0, 255);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, LOW);

  gradual_speed(speed, delay_rate, 1);
}

void backward(uint8_t speed) { //add a speed argument when speed change is decided
  //speed = map(speed, 50, 0, 0, 255);
  digitalWrite(D1, LOW);
  digitalWrite(D2, HIGH);

  gradual_speed(speed, delay_rate, 1);
}


void right(uint8_t speed) { //add a speed argument when speed change is decided
  //speed = map(speed, 200, 255, 0, 255);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);

  gradual_speed(speed, delay_rate, 1);
}


void left(uint8_t speed) { //add a speed argument when speed change is decided
  //speed = map(speed, 50, 0, 0, 255);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);

  gradual_speed(speed, delay_rate, 1);
}


void stop() { //add a speed argument when speed change is decided
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);

  gradual_speed(0, delay_rate, 1);
}