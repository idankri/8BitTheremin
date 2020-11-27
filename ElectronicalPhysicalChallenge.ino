//Imports
#include <Adafruit_CircuitPlayground.h>
#include <stdlib.h>

//Macros
#define ARRAY_SIZE(x) ((sizeof x) / (sizeof *x))
#define RANDOM(min, max) min + rand() / (RAND_MAX / (max - min + 1) + 1)

//Global Variables
void IR();
int readIR();
int makeSound();
int ROOM_DEFAULT = 0;
int IR_res;
int neoPixelsParity = 0;
int NOTES [8] = {261, 293, 329, 349, 391, 440, 493, 523};

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

void readRoom(){
  /**
   * Reads room initial IR state (assuming user isn't messing aroudn with room lighting while playing)
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
  IR = map(IR, ROOM_DEFAULT, 980, 0, ARRAY_SIZE(NOTES) - 1);
  if(IR >= ARRAY_SIZE(NOTES)){
    IR = ARRAY_SIZE(NOTES) - 1;
  }
  else if(IR < 0){
    IR = 0;
  }
  return IR;
}

void setRandomNeoPixels(int parity){
  /**
   * Sets Neo Pixels to random values, 5 at a time. 
   * Parity - if set to 0, will make even pixels to light, if set to 1, will make odd pixels to light.
   */
  int i;
  for(i = parity; i < 10; i+=2){
    CircuitPlayground.setPixelColor(i, RANDOM(0x000000, 0xFFFFFF));
  }
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
  // prints IR reading for debugging purposes
  Serial.println(IR_res);
  //every iteration of the loop neoPixelsParity will be 0 or 1
  neoPixelsParity = (neoPixelsParity + 1) % 2;
  // play random lights and play tone according to reading
  setRandomNeoPixels(neoPixelsParity); 
  CircuitPlayground.playTone(NOTES[IR_res], 100);
  //clear pixels and have some delay for robustness purposes
  CircuitPlayground.clearPixels();
  delay(20);
  

}
