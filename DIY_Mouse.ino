#include <Mouse.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Wire.h>
#include <Keyboard.h>
#include <SoftwareSerial.h>

MPU6050 mpu;
int16_t accx, accy;
int prevX, prevY;
const float filterValue = 0.95;
const int mouseSpeed = 2;

int RightClickButton = 8;
int LeftClickButton = 9;
int ESCButton = 5;
int F5Button = 6;
int ForwardButton = 4;
int BackwardButton = 10;
int ModeButton = 7;

int Bluetooth_RX = 16;
int Bluetooth_TX = 14;

int Led = 1;

boolean buttonState = HIGH;
boolean lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

boolean buttonState_1 = HIGH;
boolean lastButtonState_1 = HIGH;
unsigned long lastDebounceTime_1 = 0;
unsigned long debounceDelay_1 = 50;

SoftwareSerial bluetooth(Bluetooth_RX, Bluetooth_TX);

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  Wire.begin();
  mpu.initialize();
  Mouse.begin();

  pinMode(Led, OUTPUT);
  pinMode(RightClickButton, INPUT_PULLUP);
  pinMode(LeftClickButton, INPUT_PULLUP);
  pinMode(ESCButton, INPUT_PULLUP);
  pinMode(F5Button, INPUT_PULLUP);
  pinMode(ForwardButton, INPUT_PULLUP);
  pinMode(BackwardButton, INPUT_PULLUP);
  pinMode(ModeButton, INPUT_PULLUP);
}

void loop() {
  if (bluetooth.available() > 0) {
    char command = bluetooth.read();
    handleBluetoothCommand(command);
  }

  mpu.getMotion6(&accx, &accy, NULL, NULL, NULL, NULL);
  int deltaX = map(accx, -20000, 20000, -20, 20);
  int deltaY = map(accy, -20000, 20000, -20, 20);
  int newX = prevX + int(filterValue * (deltaX - prevX));
  int newY = prevY + int(filterValue * (deltaY - prevY));

  Mouse.move(mouseSpeed * newX, mouseSpeed * newY, 0);
  prevX = newX;
  prevY = newY;

  delay(10);
  HandleButtons();
}

void handleBluetoothCommand(char command) {
  switch (command) {
    case 'E':
      // Handle ESC command
      Keyboard.press(KEY_ESC);
      delay(50);
      Keyboard.release(KEY_ESC);
      break;
    case 'F':
      // Handle F5 command
      Keyboard.press(KEY_F5);
      delay(50);
      Keyboard.release(KEY_F5);
      break;
    // Add more cases for other commands as needed
  }
}

void HandleButtons() {
  HandleButton(ESCButton, KEY_ESC);
  HandleButton(F5Button, KEY_F5);
  HandleButton(ForwardButton, KEY_RIGHT_ARROW);
  HandleButton(BackwardButton, KEY_LEFT_ARROW);
  // Add more buttons as needed
}

void HandleButton(int buttonPin, char key) {
  boolean reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        Serial.println("Button Pressed!");
        Keyboard.press(key);
        delay(50);
        Keyboard.release(key);
        bluetooth.print(key);  // Send key over Bluetooth
      }
    }
  }
  lastButtonState = reading;
}
