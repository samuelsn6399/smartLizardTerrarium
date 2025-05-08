#include <LiquidCrystal.h>
#include "DHT.h"
#include <SPI.h>
#include <SD.h>

//////////////////////////// PUBLIC VARIABLES ////////////////////////////
// LCD Configuration
LiquidCrystal lcd(9, 8, 5, 4, 3, 2);
bool lcdToggle = true;
bool displayValveInfo = false; // Toggle for displaying valve info or environmental conditions

// DHT Sensor Configuration
#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Valve configuration
const int valvePin = A1;            // Valve pin
const unsigned long valveOpenDuration = 60000; // Valve open duration (1 minutes)
unsigned long valveCloseDuration = 60000; // Initial Valve close duration (1 minutes)
int valveState = 1; // initilize valve state, start with valve on
int valveOpen = 0; // valve is initially closed
unsigned long valveOpenStart = 0; // initial open time stamp
unsigned long valveCloseStart = 0; // initial close time stamp
unsigned long valveOpenTime = 0; // initial open time span
unsigned long valveCloseTime = 0; // initial close time span

// Potentiometer Setup
const int potPin = A0;              // Potentiometer pin

// SD Card configuration
File dataFile;
unsigned long lastLogTime = 0;
const unsigned long logInterval = 5000; // log every 5 seconds
int dataOn = 0; // initial data log off
unsigned int logNum = 0; // initial log number 0
String fileName = "Data";
String fileType = ".txt";
String file = fileName+String(logNum)+fileType;

// button and state configuration
const int switchPin = 7;                      // switch monitors pin 2
int buttonState;                              // remember the state of the button
int displayState = 0;                         //lcd monitor display state
                                                  // 0 = state0
                                                  // 1 = state1
                                                  // 2 = state2

unsigned long buttonPrevious = 0; // [ms]     // store the time when the button was initially pressed
const int buttonInterval = 3000; // [ms]      // interval for button being pressed to 'reset' the valve timer to trigger an immediate open
unsigned long buttonDiff = 0; // [ms]         // store the difference in time between button press and release

void setup() {
  pinMode(switchPin, INPUT);                  // set the switch pin as the input 
  pinMode(valvePin, OUTPUT);                  // set the valve pin as the output 
  pinMode(potPin, INPUT);                     // set the potentiometer as the input

  digitalWrite(valvePin, valveOpen);          // ensure valve is set to the initial condition (closed on start)
  
  // Serial.begin(9600);                         // set up the serial communication

  lcd.begin(16, 2);
  lcd.print("Initializing LCD");
  delay(2000);
  while(!SD.begin(10)){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Initializing SD");
    delay(2000);
  }
  
  dht.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing DHT");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready");
  delay(2000);

  while (analogRead(potPin)!=0){
    /*
    Serial.print("Set Potentiometer to Zero | ");
    Serial.println(analogRead(potPin));
    */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set Pot to Zero");
    lcd.setCursor(0, 1);
    lcd.print(analogRead(potPin));
    delay(1000);
  }

  buttonState = digitalRead(switchPin);       // read the initial button state
  /*
  Serial.println();
  Serial.println("System start up, State 0, Display Valve Settings");
  */
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Valve Settings:");
}

void loop() {
  int val = digitalRead(switchPin);           // read the input of the switch (1/0)
  delay(10);                                  // wait 10 ms to read switch state again
  int val2 = digitalRead(switchPin);          // read the input of the switch (1/0)

  if (val == val2) {                          // compare the 2 readings for bounce
    buttonDuration(val);                      // check the button press duration
    if (val != buttonState && buttonDiff < buttonInterval) { // check if the state has changed AND if the button has been pressed longer than the interval value
      if (val == LOW) {                       // check if the button is being pressed
        if (displayState == 0) {                // check if the display was previously in state 0
          displayState = 1;                     // change the display state to 1
          // Serial.println("Button just pressed, Display Environment Data");
          lcd.clear();
        }else if (displayState == 1){           // check if the display was previously in state 1 
          displayState = 2;                     // change the display state to 2
          // Serial.println("Button just pressed, Display Valve Timer");
          lcd.clear();
        }else{                                // check if the display was previously in state 2 
          displayState = 0;                     // change the display state to 0
          // Serial.println("Button just pressed, Display Valve Settings");
          lcd.clear();
        }
      }
    }else if (val != buttonState && buttonDiff > buttonInterval) {/* check if the user wants to reset the timer and trigger the valve to open immediately
                                                                                        by pressing button for 5 seconds.*/
      if (displayState == 2) {                   //  user must be on the "valve status" display.
        valveOpen = 0;
        valveState = 1;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Valve Reset");
        delay(1000);

      }else if (displayState == 1) {                    //  user must be on the "Eviro Data" display.
        if (dataOn == 0){
          dataOn = 1;
          file = fileName+String(logNum)+fileType;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Data Log On");
          // Serial.println(file);
          delay(1000);
        }else{
          dataOn = 0;
          logNum = logNum+1;
          // Serial.println(logNum);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Data Log Off");
          delay(1000);
        }
      }
    }
  }
  
  buttonState = val;                          // update the button state

  //////////////////////////// READ SENSORS             /////////////////////////
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(true);

  //////////////////////////// CONTROL VALVE             /////////////////////////
  if (valveState==1){
    if (valveOpen==0){
      valveOpen = 1;
      valveCloseTime = 0;
      valveOpenStart = millis();
      digitalWrite(valvePin, valveOpen); // open valve
    }else{
      valveOpenTime = millis() - valveOpenStart;
    }
    if (valveOpenTime > valveOpenDuration){
      valveState = 0;
    }
  }else{
    if (valveOpen==1){
      valveOpen = 0;
      valveOpenTime = 0;
      valveCloseStart = millis();
      digitalWrite(valvePin, valveOpen); // close valve
    }else{
      valveCloseTime = millis() - valveCloseStart;
    }
    if (valveCloseTime > valveCloseDuration){
      valveState = 1;
    }
  }

  //////////////////////////// LOG DATA                 /////////////////////////
  if (millis() - lastLogTime >= logInterval && dataOn) {
    if (!isnan(temperature) && !isnan(humidity)) {
      logDataToSD(temperature, humidity, valveOpen, file);
    }else{
      logDataToSD(0, 0, valveOpen, file);
    }
    lastLogTime = millis();
  }

  //////////////////////////// EXECUTE STATE FUNCTIONS  /////////////////////////
 if (displayState == 0){
  display0();
  lcd.setCursor(0, 0);
  lcd.print("Valve Settings:");
 }else if(displayState == 1){
  display1(temperature, humidity);
  lcd.setCursor(0, 0);
  lcd.print("Enviro Data:");
 }else{
  display2(valveOpenTime, valveCloseTime, valveOpenDuration, valveCloseDuration);
  lcd.setCursor(0, 0);
  lcd.print("Valve Status:");
 }
}

int display0(){                               // valve settings
  //Serial.println("State 0");
  //Serial.print("Valve Open: ");
  //Serial.print(valveOpenDuration/60000);
  //Serial.print(" min | ");
  //Serial.print("Valve Close: ");
  lcd.setCursor(0, 1);
  if (analogRead(potPin)>20 && analogRead(potPin)<100){
    valveCloseDuration = 60000*2;
    //Serial.print("2 min");
    lcd.print("2 min ");
  }else if (analogRead(potPin)>100 && analogRead(potPin)<200){
    valveCloseDuration = 60000*3;
    //Serial.print("3 min");
    lcd.print("3 min ");
  }else if (analogRead(potPin)>200 && analogRead(potPin)<300){
    valveCloseDuration = 60000*5;
    //Serial.print("5 min");
    lcd.print("5 min ");
  }else if (analogRead(potPin)>300 && analogRead(potPin)<400){
    valveCloseDuration = 60000*10;
    //Serial.print("10 min");
    lcd.print("10 min");
  }else if (analogRead(potPin)>400 && analogRead(potPin)<500){
    valveCloseDuration = 60000*20;
    //Serial.print("20 min");
    lcd.print("20 min");
  }else if (analogRead(potPin)>500 && analogRead(potPin)<600){
    valveCloseDuration = 60000*30;
    //Serial.print("30 min");
    lcd.print("30 min");
  }else if (analogRead(potPin)>600 && analogRead(potPin)<700){
    valveCloseDuration = 60000*60*1;
    //Serial.print("1 hr");
    lcd.print("1 hr  ");
  }else if (analogRead(potPin)>700 && analogRead(potPin)<800){
    valveCloseDuration = 60000*60*3;
    //Serial.print("3 hr");
    lcd.print("3 hr  ");
  }else if (analogRead(potPin)>800 && analogRead(potPin)<900){
    valveCloseDuration = 60000*60*6;
    //Serial.print("6 hr");
    lcd.print("6 hr  ");
  }else if (analogRead(potPin)>900 && analogRead(potPin)<1000){
    valveCloseDuration = 60000*60*12;
    //Serial.print("12 hr");
    lcd.print("12 hr ");
  }else if (analogRead(potPin)>1000){
    valveCloseDuration = 60000*60*24;
    //Serial.print("24 hr");
    lcd.print("24 hr ");
  }else{
    valveCloseDuration = 60000*1;
    //Serial.print("1 min");
    lcd.print("1 min ");
  }
  //Serial.println("");
  return(valveCloseDuration);
}

void display1(float data1, float data2){      // environmental conditions
  /*
  Serial.println("State 1");
  Serial.print("Temperature: ");
  Serial.print(data1);
  Serial.print(" | ");
  Serial.print("Humidity: ");
  Serial.print(data2);
  Serial.println("");
  */
  lcd.setCursor(0, 1);
  lcd.print("T: ");
  lcd.print(data1);
  lcd.print((char)223);
  lcd.print("F");
  lcd.print(" ");
  lcd.print("H: ");
  lcd.print(data2);
}

void display2(double openTime, double closeTime, double openDuration, double closeDuration){ // valve timer
  //Serial.println("State 2");
  if (valveOpen == 1){
    /*
    Serial.print("Valve Open: ");
    Serial.print(openTime/60000);
    Serial.print("/");
    Serial.print(openDuration/60000);
    Serial.println(" min");
    */
    lcd.setCursor(0, 1);
    lcd.print("Open: ");
    lcd.print(openTime/60000);
    lcd.print("min");
  }else{
    /*
    Serial.print("Valve Closed: ");
    Serial.print(closeTime/60000);
    Serial.print("/");
    Serial.print(closeDuration/60000);
    Serial.println(" min");  
    */
    lcd.setCursor(0, 1);
    lcd.print("Close: ");
    lcd.print(closeTime/60000);
    lcd.print("min");
  }
}

long buttonDuration(int val){                 // checking button press duration
  if(val != 1){                               // if the button has been pressed and has passed the debounce filter
    buttonPrevious = millis();                // if the button has NOT been pressed keep the previous button time updated
  }else{
    buttonDiff = millis() - buttonPrevious;   // if the button HAS been pressed log the duration of the press
  }
}

void logDataToSD(float temperature, float humidity, bool valveOpen, String name) {
  String timestamp = formatTime(millis());
  dataFile = SD.open(name, FILE_WRITE);
  if (dataFile) {
    dataFile.print(timestamp);
    dataFile.print(", ");
    dataFile.print(temperature, 2);
    dataFile.print(", ");
    dataFile.print(humidity, 2);
    dataFile.print(", ");
    dataFile.println(valveOpen ? "OPEN" : "CLOSED");
    dataFile.close();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SD Write Error");
    delay(2000); // brief user warning
  }
}

String formatTime(unsigned long ms) {
  unsigned long seconds = ms / 1000;
  unsigned int hours = seconds / 3600;
  unsigned int minutes = (seconds % 3600) / 60;
  unsigned int secs = seconds % 60;

  char buffer[9];
  sprintf(buffer, "%02u:%02u:%02u", hours, minutes, secs);
  return String(buffer);
}

