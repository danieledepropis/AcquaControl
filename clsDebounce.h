class clsDebounce {
  unsigned long debounceDelay;
  unsigned long lastDebounceTime;
  int lastButtonState;
  int buttonState;
  int intbuttonPin;
public:

clsDebounce(int buttonPin);
void setPinMode(int intMode);
//int readDigitalDebounce();
int getButtonState();
};
