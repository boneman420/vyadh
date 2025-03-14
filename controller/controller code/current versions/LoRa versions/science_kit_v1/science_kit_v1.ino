#define steppin 13
#define dirpin 12

const int step = 1600;
const int step1 = 104;
const int step2 = 190;
int i = 0;
int speed = 5; // Placeholder speed value

// PUMP MOTOR PINS
#define enA A0    
#define in1 8
#define in2 9
#define enB A1
#define in3 10
#define in4 11
#define enC A2 
#define in5 7
#define in6 6 
#define enD A3 
#define in7 5
#define in8 4

void setup() {
  // STEP MOTOR SETUP **************************************************************************************
  Serial.begin(9600);
  pinMode(steppin, OUTPUT);
  pinMode(dirpin, OUTPUT);

  // PUMP MOTOR SETUP **************************************************************************************
  setupPump(enA, in1, in2);
  setupPump(enB, in3, in4);
  setupPump(enC, in5, in6);
  setupPump(enD, in7, in8);

  Serial.println("Ready for commands.");
}

void loop() {
  Serial.println("--");
  // Check for serial input *********************************************************************************
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "p") {
      preFill();
    } else if (command == "a") {
      runStepper();
    }
  }
}

// Function to set up pump pins
void setupPump(int enPin, int inPin1, int inPin2) {
  pinMode(enPin, OUTPUT);
  pinMode(inPin1, OUTPUT);
  pinMode(inPin2, OUTPUT);
  digitalWrite(inPin1, LOW);
  digitalWrite(inPin2, LOW);
}

// Function to control a pump
void controlPump(int enPin, int inPin1, int inPin2, int delayTime) {
  analogWrite(enPin, 255);
  digitalWrite(inPin1, HIGH);
  digitalWrite(inPin2, LOW);
  Serial.print("Pump ");
  Serial.print(enPin);
  Serial.println(" on");
  delay(delayTime);
  digitalWrite(inPin1, LOW);
  digitalWrite(inPin2, LOW);
  Serial.print("Pump ");
  Serial.print(enPin);
  Serial.println(" off");
}

void preFill() {
  Serial.println("Executing preFill routine...");
  controlPump(enA, in1, in2, 1000);
  delay(150);
  controlPump(enB, in3, in4, 1000);
  delay(150);
  controlPump(enC, in5, in6, 800);
  delay(150);
  controlPump(enD, in7, in8, 800);
  delay(150);
}

void runStepper() {
  int pumps[] = {enA, enB, enC, enD};
  int inPins1[] = {in1, in3, in5, in7};
  int inPins2[] = {in2, in4, in6, in8};

  for (int x = 1; x <= 3; x++) {
    if (x % 3 == 0) {
      controlPump(pumps[i], inPins1[i], inPins2[i], 550);
      stepMotor(step2);  // Perform larger step
    } else {
      controlPump(pumps[i], inPins1[i], inPins2[i], 550);  // Activate the respective pump
      stepMotor(step1);  // Perform smaller step
    }
  }
  i = (i + 1) % 4;  // Cycle through the pumps
  delay(150);
}

void stepMotor(int steps) {
  for (int j = 1; j <= steps; j++) {
    digitalWrite(steppin, HIGH);
    delay(speed);
    digitalWrite(steppin, LOW);
    delay(speed);
  }
}
//in the grand scheme of thingd it doesn't matter