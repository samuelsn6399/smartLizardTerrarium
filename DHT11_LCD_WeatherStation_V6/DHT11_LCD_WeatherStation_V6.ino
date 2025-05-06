//////////////////////////// PUBLIC VARIABLES ////////////////////////////
const int switchPin = 7;                      // switch monitors pin 2
int buttonState;                              // remember the state of the button
int displayState = 0;                           // turns the led on and off during blinking
                                                  // 0 = off
                                                  // 1 = state1
                                                  // 2 = state2

unsigned long buttonPrevious = 0; // [ms]     // store the time when the button was initially pressed
const int buttonInterval = 1000; // [ms]      // interval for button being pressed to 'cancel' the change in state
unsigned long buttonDiff = 0; // [ms]         // store the difference in time between button press and release

void setup() {
  pinMode(switchPin, INPUT);                  // set the switch pin as the input 
  Serial.begin(9600);                         // set up the serial communication
  while(!Serial){}
  buttonState = digitalRead(switchPin);       // read the initial button state
  Serial.println();
  Serial.println("System start up, State 0, Display Valve Settings");
}

void loop() {
  int val = digitalRead(switchPin);           // read the input of the switch (1/0)
  delay(10);                                  // wait 10 ms to read switch state again
  int val2 = digitalRead(switchPin);          // read the input of the switch (1/0)

  if (val == val2) {                          // compare the 2 readings for bounce
    buttonDuration(val);                      // check the button press duration
    if (val != buttonState && buttonDiff < buttonInterval) { // check if the state has changed AND if the button has been pressed longer than the interval value
      if (val == LOW) {                       // check if the button is being pressed
        if (displayState == 0) {                // check if the light was previously in state 0
          displayState = 1;                     // change the light state to 1
          Serial.println("Button just pressed, Display Environment Data");
        }else if (displayState == 1){           // check if the light was previously in state 1 
          displayState = 2;                     // change the light state to 2
          Serial.println("Button just pressed, Display Valve Timer");
        }else{                                // check if the light was previously in state 2 
          displayState = 0;                     // change the light state to 0
          Serial.println("Button just pressed, Display Valve Settings");
        }
      }
    }
  }
  buttonState = val;                          // update the button state
  //////////////////////////// EXECUTE STATE FUNCTIONS  /////////////////////////
 if (displayState == 0){
  display0();
 }else if(displayState == 1){
  display1();
 }else{
  display2();
 }
}

void display0(){ // valve settings
  //Serial.println("State 0");
}

void display1(){ // environmental conditions
  //Serial.println("State 1");
}

void display2(){ // valve timer
  //Serial.println("State 2");
}

long buttonDuration(int val){                 // checking button press duration
  if(val != 1){                               // if the button has been pressed and has passed the debounce filter
    buttonPrevious = millis();                // if the button has NOT been pressed keep the previous button time updated
  }else{
    buttonDiff = millis() - buttonPrevious;   // if the button HAS been pressed log the duration of the press
  }
}