// Trivial LED actions for built in LED
// dmc - Aug 2025
#ifndef LED_Control_h
#define LED_Control_h

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

class LED_Control{
 private:
  int IOpin;
  boolean LEDstate=false;// true=on
 public:
  void Init(int PinLED){
    IOpin=PinLED;
    pinMode(IOpin, OUTPUT);
  }
  void Init(){ // Use built in LED as default
    Init(LED_BUILTIN);
  }
  boolean OnOff(boolean s){
    digitalWrite(IOpin, LEDstate=s);
    return(LEDstate);
  }
  boolean OnOff(){
    return(LEDstate);
  }
  boolean Toggle(){
    digitalWrite(IOpin, LEDstate=!LEDstate); 
    return(LEDstate);
  }
  LED_Control(){}
  ~LED_Control(){}
};
#endif
