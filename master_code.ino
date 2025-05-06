// CODE FOR MASTER

// code adapted from
// 

// allows making other pins - except the 0 and 1 on the serial monitor that are 
#include <SoftwareSerial.h>

// Define software serial pins
SoftwareSerial BTSerial(10, 11); // (RX, TX)

#define button 3
int value = 0; //current value of the toggle state
int lastButtonState = HIGH;  // Start with unpressed - Basically old state

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BTSerial.begin(9600);  // For Bluetooth communication
  pinMode(button, INPUT_PULLUP); //mainly to ensure debounce and not log 2 button presses
  delay(200);
}

void loop() {
  // Read current state of button
  int currentButtonState = digitalRead(button);
  // Serial.println(currentButtonState);

  // Detect falling edge: button just got pressed
  if (lastButtonState == HIGH && currentButtonState == LOW) { //check if it was just turned on
    // Toggle value
    value = !value;

    // Send data via Bluetooth
    BTSerial.write(value ? '1' : '0'); //sends 0 or 1 as per state
    Serial.print("Button pressed. Sending "); 
    Serial.println(value ? "1 to Slave" : "0 to Slave");
    Serial.println(currentButtonState);

    // Debugging logs
    Serial.print("Current value state: ");
    Serial.println(value);

    delay(300); // basic debounce
  }

  // Update previous state for next loop iteration
  lastButtonState = currentButtonState;

  delay(10); // small delay for loop stability
}



// TESTNG
// Whether correct data is sent - print statements of - Sending 0/1 to Slave
// button state logic test - if(val == 0 && value == 0)
// button pressed for a long time - doesnt keep switching states
