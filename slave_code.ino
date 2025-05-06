// CODE FOR SLAVE

//adding all the libraries 
#include <SoftwareSerial.h> 
#include <FastLED.h>

// // Define software serial pins
SoftwareSerial BTSerial(10, 11); // (RX, TX)

#define LED_PIN 2   // led pin 
#define speakerPin 6 //piezo pin
#define NUM_LEDS 5     // we have 5 leds

// Melody settings
int length = 15;
char notes[] = "ccggaagffeeddc "; 
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;

int ledPin = 2; // choose the pin for the LED
CRGB leds[NUM_LEDS];

bool isBarking = false;   // to see if we are in between barking 
bool stopBarking = false;  // to stop the barking manually when button press
unsigned long lastNoteTime = 0;    //when last note started
int currentNoteIndex = 0;   // which note is playing rn - its index

// Function to play a tone
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) { //
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
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

// Function to play a note
void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 }; //frequency defined for each note

  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      tone(speakerPin, tones[i], duration);  // non-blocking tone()
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BTSerial.begin(9600);  // For Bluetooth communication
  pinMode(ledPin, OUTPUT); // declare LED as output
  pinMode(speakerPin, OUTPUT);
  delay(200);
  digitalWrite(ledPin, LOW);        // Make sure LED is off at start
  digitalWrite(speakerPin, LOW); // Make sure piezo is silent at start

  FastLED.addLeds<WS2812, LED_PIN, RGB>(leds,NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,500);
  FastLED.clear();
  FastLED.show();
}

// ---- LOGIC -----//
// stop playing boolean will be false when 1 is recieved 
// if bluetooth module recieves 0, then stopPlaying is true 
// if stopplaying = true - return from the loop (to not beginning playing in the first place)
// if stopPlaying = false - go ahead with playing


void loop() {
  if (BTSerial.available()) { // Check if data is received
  Serial.println("Data Available!");  // Debugging
    char val = BTSerial.read(); // Read integer value
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
      
    
      // play happy birthday
      Serial.println("playing happy birthday");
      isBarking = true; //sound can play - if 1
      stopBarking = false;
      currentNoteIndex = 0; //always start with beginning
      lastNoteTime = millis(); // here using lastNoteTime with millis instead of the usual delay 
      //  because when i did with the delay, the entire program had a delay. With millis(), I was able to stop
      // keep track of the note with milliseconds but overall the program kept running checking for the val =0
      // when i had delay - then even next input did not come in on time. 
      // delay - blocks whole program - this is fine for button press for master because in master that's the only purpose fo the program now
      // millis - an additional delay for milliseconds that doesnt stop the entire program

      for (int i = 0; i < length; i++) { 
        if (isBarking==true){
          Serial.println("dont play the sound");
          return;
        }
        if (notes[i] == ' ') { //each note is played
          delay(beats[i] * tempo); //with a delay and tempo defined
        } else {
          playNote(notes[i], beats[i] * tempo);
        }
        delay(tempo / 2);
      }
    } 
    else if (val == '0') {  
      digitalWrite(ledPin, LOW);
      Serial.println("LED is OFF");
      FastLED.clear();
      FastLED.show();
      collarAnimation();

      stopBarking = true; //make this true - stop barking manually
      isBarking = false; //will change it
      noTone(speakerPin); // Stop the tone immediately
      Serial.println("LED OFF and Music stopped");
    }
  }

  //Play barking non-blocking
  if(isBarking ==true && stopBarking!=true){  //barking on and not asked to stop
    unsigned long currentMillis = millis();  // continue barking - counting millis

// if still barking needs to continue and if enough time passed, then continue barking
    if(currentNoteIndex < length && currentMillis - lastNoteTime >= beats[currentNoteIndex] * tempo){
      if (notes[currentNoteIndex] != ' ') { //null note checking - just realied this later so added it :)
        playNote(notes[currentNoteIndex], beats[currentNoteIndex] * tempo); // if next note is not null, then keep barking
      }
      lastNoteTime = currentMillis + tempo / 2;  // add short pause between notes
      currentNoteIndex++; 
      // Serial.print("Playing note: ");
      // Serial.println(notes[currentNoteIndex]);
    }

    // End of melody
    if (currentNoteIndex >= length) { //if all done 
      currentNoteIndex =0; //keep looping the bark because button wasnt pressed
    }
  }
}

void collarAnimation() {
  // Cycle through the LEDs to create a happiness animation
  for (int cycle = 0; cycle < 2; cycle++) {  // Repeat the animation twice
    for (int i = 0; i < NUM_LEDS; i++) {  // Turn on LEDs one by one
      leds[i] = CRGB(100, 100, 0);  // Purple color [GRB]
      FastLED.show();
      // leds[i] = CRGB::Green;  // Set color to green - happiness :)
      FastLED.show();
      delay(100);  // Slight delay to make the animation visible
      leds[i] = CRGB::Black;  // Turn off LED after animation completes
      FastLED.show();
    }
  }
}


// ARDUINO TECHNICALITY
// loop runs every second 
// millis () - thats why is a built in Arduino Timer  it checks how many milliseconds
//passed since the code started running so within that 1 sec of loop - tracking that 1 sec



