#include "Arduino.h"
#include "clsDebounce.h"

clsDebounce::clsDebounce(int buttonPin) {
  debounceDelay=50;
  lastDebounceTime=0;
  lastButtonState=LOW;
  buttonState=LOW;
  intbuttonPin=buttonPin;
  
}

void clsDebounce::setPinMode(int intMode) {
  pinMode(intbuttonPin, intMode);
}

int clsDebounce::getButtonState() {
  int reading = digitalRead(intbuttonPin);
  
  if (reading != lastButtonState) {
    // reset the debouncing timer
    
    lastDebounceTime = millis();
  }
  
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    buttonState=reading;
  } 
  return buttonState;
}

//int clsDebounce::getButtonState() {
//  return readDigitalDebounce();
//}
