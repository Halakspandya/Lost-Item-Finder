// CODE FOR SLAVE

//adding all the libraries
#include <SoftwareSerial.h>
#include <FastLED.h>

// // Define software serial pins
SoftwareSerial BTSerial(10, 11);  // (RX, TX)

#define LED_PIN 2     // led pin
#define speakerPin 6  //piezo pin
#define NUM_LEDS 5    // we have 5 leds

// Melody settings - used for earlier happy bday sounds.
// This is still used as a reference to be able to refer to for understanding the tone funciton of the piezo
int length = 15;
char notes[] = "ccggaagffeeddc ";
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 }; 
int tempo = 300;

int ledPin = 2;  // choose the pin for the LED
CRGB leds[NUM_LEDS];

bool isBarking = false;    // to see if we are in between barking
bool stopBarking = false;  // to stop the barking manually when button press
int barkStep = 0;
int barkState = 0;  // 0 = woof1, 1 = woof2
unsigned long lastBarkTime = 0;

int volume = 1;  // volume increases after each loop of bark

// Function to play a tone
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {  //
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
  noTone(speakerPin);
}

// LOGIC FOR STOPPING MID MELODY
// track each note in miliseconds
// track input of the master - 0 or 1 - keep reading it - happening in loop
// As soon as 0 is read, you stop mid it from playing the next note - this can be handled
// by tone () instead of using the playNote - which means you kreep playing the entire melody.
// Calling playNote () means I keep asking it to play the melody again and again but when
// I ask the tone (), I am asking it to play each note
// Ignore the number of print statements, and the long comments, i needed them to not forget my code and to
// debug and check everything working


void resetVolume() {
  volume = 1;
}


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
  BTSerial.begin(9600);     // For Bluetooth communication
  pinMode(ledPin, OUTPUT);  // declare LED as output
  pinMode(speakerPin, OUTPUT);
  delay(200);
  digitalWrite(ledPin, LOW);      // Make sure LED is off at start
  digitalWrite(speakerPin, LOW);  // Make sure piezo is silent at start

  FastLED.addLeds<WS2812, LED_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.clear();
  FastLED.show();
}

// ---- LOGIC -----//
// stop playing boolean will be false when 1 is recieved
// if bluetooth module recieves 0, then stopPlaying is true
// if stopplaying = true - return from the loop (to not beginning playing in the first place)
// if stopPlaying = false - go ahead with playing


void loop() {
  if (BTSerial.available()) {           // Check if data is received
    Serial.println("Data Available!");  // Debugging
    char val = BTSerial.read();         // Read integer value
    Serial.print("Received: ");
    Serial.println(val);  // Debugging

    if (val == '1') {
      //to check bluetooth module recieves data - connection established well and no errors with data type
      Serial.println("Condition matched for 1");

      // turn on the led
      digitalWrite(ledPin, HIGH);
      Serial.println("LED is ON");
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, 100, 100);  // Purple color [GRB]
      }
      FastLED.show();



      isBarking = true;  //sound can play - if 1
      stopBarking = false;
      barkStep = 0;
      barkState = 0;
      lastBarkTime = millis();


      // because when i did with the delay, the entire program had a delay. With millis(), I was able to stop
      // keep track of the note with milliseconds but overall the program kept running checking for the val =0
      // when i had delay - then even next input did not come in on time.
      // delay - blocks whole program - this is fine for button press for master because in master that's the only purpose fo the program now
      // millis - an additional delay for milliseconds that doesnt stop the entire program


      for (int i = 0; i < length; i++) {
        if (stopBarking == true) {
          Serial.println("dont play the sound");
          return;
        }
      }
    }


    else if (val == '0') {
      digitalWrite(ledPin, LOW);
      Serial.println("LED is OFF");
      FastLED.clear();
      FastLED.show();
      collarAnimation();

      stopBarking = true;  //make this true - stop barking manually
      isBarking = false;   //will change it
      noTone(speakerPin);  // Stop the tone immediately
      Serial.println("LED OFF and Music stopped");
    }
  }

  //Play barking non-blocking
  if (isBarking == true && stopBarking != true) {  //barking on and not asked to stop
    unsigned long currentMillis = millis();        // continue barking - counting millis
    if (barkState == 0) {                          // First woof
      if (barkStep < 20) {
        if (currentMillis - lastBarkTime >= 15) {
          int freq = random(180 + volume * 10, 280 + volume * 10);
          tone(speakerPin, freq);
          lastBarkTime = currentMillis;
          barkStep++;
        }
      } else {
        noTone(speakerPin);
        barkState = 1;
        barkStep = 0;
        lastBarkTime = currentMillis;
      }
    }

    else if (barkState == 1) {  // Pause between woofs
      noTone(speakerPin);
      if (currentMillis - lastBarkTime >= 500) {
        barkState = 2;
        barkStep = 0;
        lastBarkTime = currentMillis;
      }
    }

    else if (barkState == 2) {  // Second woof
      if (barkStep < 15) {
        if (currentMillis - lastBarkTime >= 10) {
          int freq = random(250 + volume * 10, 350 + volume * 10);
          tone(speakerPin, freq);
          lastBarkTime = currentMillis;
          barkStep++;
        }
      } else {
        noTone(speakerPin);
        barkState = 3;
        lastBarkTime = currentMillis;
      }
    }

    else if (barkState == 3) {  // End pause
      if (currentMillis - lastBarkTime >= 150) {
        barkState = 0;  // Repeat barking cycle (loop)
        barkStep = 0;
        lastBarkTime = currentMillis;
      }
    }
  }
}

// ARDUINO TECHNICALITY
// loop runs every second
// millis () - thats why is a built in Arduino Timer  it checks how many milliseconds
//passed since the code started running so within that 1 sec of loop - tracking that 1 sec
