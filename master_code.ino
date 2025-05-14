// CODE FOR MASTER

// code adapted from
// 

// allows making other pins - except the 0 and 1 on the serial monitor that are 
#include <SoftwareSerial.h>
#include <FastLED.h>

// Define software serial pins
SoftwareSerial BTSerial(10, 11); // (RX, TX)

#define LED_PIN 2     // led pin
#define button 3
#define NUM_LEDS 5    // we have 5 leds
int value = 0; //current value of the toggle state
int lastButtonState = HIGH;  // Start with unpressed - Basically old state
CRGB leds[NUM_LEDS];


void collarAnimation() {
  // Cycle through the LEDs to create a happiness animation
  for (int cycle = 0; cycle < 2; cycle++) {  // Repeat the animation twice
    for (int i = 0; i < NUM_LEDS; i++) {     // Turn on LEDs one by one
      leds[i] = CRGB(100, 100, 0);           // Purple color [GRB]
      FastLED.show();
      // leds[i] = CRGB::Green;  // Set color to green - happiness :)
      FastLED.show();
      delay(100);             // Slight delay to make the animation visible
      leds[i] = CRGB::Black;  // Turn off LED after animation completes
      FastLED.show();
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BTSerial.begin(9600);  // For Bluetooth communication
  pinMode(button, INPUT_PULLUP); //mainly to ensure debounce and not log 2 button presses
  delay(200);

  FastLED.addLeds<WS2812, LED_PIN, RGB>(leds, NUM_LEDS); // led strip initialising
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500); 
  FastLED.clear();
  FastLED.show();
}

void loop() {
  // Read current state of button
  int currentButtonState = digitalRead(button);
  // Serial.println(currentButtonState);

  // Detect falling edge: button just got pressed
  if (lastButtonState == HIGH && currentButtonState == LOW) { //check if it was just turned on
    // Toggle value
    value = !value; //just to make sure it doesnt send the same value again

    // Send data via Bluetooth
    BTSerial.write(value ? '1' : '0'); //sends 0 or 1 as per state
    Serial.print("Button pressed. Sending "); 
    Serial.println(value ? "1 to Slave" : "0 to Slave");
    Serial.println(currentButtonState);

    // Debugging logs
    Serial.print("Current value state: ");
    Serial.println(value);

    // if the value 1 is sent, then turn on the rgb led strip, 
    // otherwise start the collarAnimation 
    if (value==1){
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, 100, 100);  // Purple color [GRB]
      }
      FastLED.show();
    }
    else
    {
      collarAnimation();
    }

    delay(300); // debounce
  }

  // Update previous state for next loop iteration
  lastButtonState = currentButtonState;

  delay(10); // small delay for loop stability
}






// TESTNG
// Whether correct data is sent - print statements of - Sending 0/1 to Slave
// button state logic test - if(val == 0 && value == 0)
// button pressed for a long time - doesnt keep switching states
