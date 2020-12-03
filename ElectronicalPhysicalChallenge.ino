/**
 * @author: Idan Kringel
 * 8-Bit Theremin
 * 
 * The next project is a Theremin that plays 8-Bit like music in various scales and tempos using Adafruit Circuit Playground Express microprocessor.
 * The instrument detects IR retransmission intensity (the threshold intensity is set by the room but usually works better with lights off),
 *  and plays a note according to that intensity - meaning that the closer your hand the higher the note played.
 * The instrument plays notes in a rapid pace, set by tempo variable, and the tempo can be changed pressing the left button - long press yields slower tempo and
 *   short press yields faster tempo
 * Also, the right button can change the mode of the notes (Modus) - the instrument initially plays C Major scale, and each press switches to C in the next mode -  
 *   Dorian - Phrygian - Lydian - Mixolydian - Aeolian (Minor) - Locrian - and back to Ionian\Major
 *   
 * The Circuit:
 * Adafruit Circuit Playground Express only :)
 * 
 * Video Link - https://www.youtube.com/watch?v=ExWx5Vve3Bc
 */

//Imports
#include <Adafruit_CircuitPlayground.h>
#include <stdlib.h>

//Macros
#define ARRAY_SIZE(x) ((sizeof x) / (sizeof *x))
#define RANDOM(low, high) low + rand() / (RAND_MAX / (high - low + 1) + 1)

//function declarations
void setIR();
int readIR();
void readRoom();
void setRandomNeoPixels();
void getNewTempo();
void getNewMode();

//global variables
int ROOM_DEFAULT = 0;
int IR_res;
int neoPixelsParity = 0;
int tempo = 20; // sets the delay between loops
int mode = 0; //mode for music mode (modus)
int NOTES [7][8] = {{261, 293, 329, 349, 391, 440, 493, 523},
                    {261, 293, 311, 349, 391, 440, 466, 523},
                    {261, 277, 311, 349, 391, 415, 466, 523},
                    {261, 293, 329, 369, 391, 440, 493, 523},
                    {261, 293, 329, 349, 391, 440, 466, 523},
                    {261, 293, 311, 349, 391, 415, 466, 523},
                    {261, 277, 311, 349, 369, 415, 466, 523}}; // All basic modes for C, first one is C-D-E-F-G-A-B-C



// the next two functions are modifications of proximity library adapted to the projects needs
void setIR(){
  /**
   * Initilizes IR LED Pin
   */
  //Setup IR LED pin
  pinMode(25, OUTPUT);
  digitalWrite(25, LOW);
  delay(50);
  readRoom();
}

int readIR(){
  /**
   * Reads IR and map it to a value fitting the NOTES array size
   */
  digitalWrite(25, HIGH);
  delay(10);
  //Get reading
  int IR = analogRead(A8);
  //Turn off IR LED
  digitalWrite(25, LOW);
  //Make reading a more reasonable number
  Serial.println(IR);
  IR = map(IR, ROOM_DEFAULT, 980, 0, ARRAY_SIZE(NOTES[0]) - 1);
  if(IR >= ARRAY_SIZE(NOTES[0])){
    IR = ARRAY_SIZE(NOTES[0]) - 1;
  }
  else if(IR < 0){
    IR = 0;
  }
  return IR;
}

void readRoom(){
  /**
   * Reads room initial IR state (assuming user isn't messing around with room lighting while playing)
   */
  digitalWrite(25, HIGH);
  delay(10);
  //Get reading
  int IR = analogRead(A8);
  //Turn off IR LED
  digitalWrite(25, LOW);
  delay(50);
  ROOM_DEFAULT = IR;
}

void setRandomNeoPixels(int parity){
  /**
   * Sets Neo Pixels to random values, 5 at a time. 
   * Parity - if set to 0, will make even pixels to light, if set to 1, will make odd pixels to light.
   */
  int i;
  for(i = parity; i < 10; i+=2){
    // generating LED values with less red doesn't look as good but prevents bugs for IR detection
    CircuitPlayground.setPixelColor(i, RANDOM(0x000000, 0x11FFFF)); 
  }
}

void getNewTempo(){
  /**
   * The next function is used to make the left button to be used as tap delay. 
   * It's a hack since most standard time counting libraries in C have collisions with Adafruit Circuit Playground header for some reason
   */
  int count = 0;
  while(CircuitPlayground.leftButton()){
    delay(1);
    ++count;
  }
  // Experimentation led me to the conclusion that this formula yields a "tempo" reasonable to the length of the press
  tempo = (count - 350 > 10)? count - 350: 10;  
}

void getNewMode(){
  /**
   * When right button is up (after being down) switches mode (Modus \ scale) to the next mode.
   */
  while(CircuitPlayground.rightButton()){
    delay(1);
  }
  mode = (mode + 1) % ARRAY_SIZE(NOTES);
}


void setup() {
  CircuitPlayground.begin();
  while (!Serial);
  Serial.begin(19200);
  setIR();
  CircuitPlayground.clearPixels();
}

void loop() {
  IR_res = readIR();
  //every iteration of the loop neoPixelsParity will be flipped between 0 and 1
  neoPixelsParity = neoPixelsParity ^ 1;
  // play random lights and play tone according to reading
  setRandomNeoPixels(neoPixelsParity); 
  CircuitPlayground.playTone(NOTES[mode][IR_res], 100);
  CircuitPlayground.clearPixels();
  // The next form of loop makes button functionality more responsive in a trade off for less accurate "tempo"
  for(int i = 0; i < tempo; i++){
    delay(1);
    if(CircuitPlayground.leftButton()){
      getNewTempo();
      break;
    }
    if(CircuitPlayground.rightButton()){
      getNewMode();
      break;
    }
  }
}
