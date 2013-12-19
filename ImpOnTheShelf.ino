
//Copyright 2013 Grand St (Authored by Nick Steele - nick@grandst.com)
//Happy Holidays bbs!

#include <Adafruit_NeoPixel.h>

#define PIN 52

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(300, PIN, NEO_GRB + NEO_KHZ800);

enum state {
  Steady,
  Wiping,
  Blinking,
  ColorBlinking,
  Checker,
  Rainbows,
  Splosions,
  Pulsing
};

//Hold our Enum States 
byte currentState;
byte newState;

//Hold our Colors
uint32_t curColor;
uint32_t secondaryColor;

//Get the UART input
String inputString = "";

//Tells us whether we have our Command 
boolean stringComplete = false;
boolean setSecondary = false;
boolean noNewInput = true;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
  //Serial1.begin(2400);
  Serial2.begin(2400);
  
  randomSeed(1227);
  
  curColor = strip.Color(200,0,0);
  secondaryColor = strip.Color(0,250,0);
  
  currentState = Steady;
  newState = Steady;
  
}

void loop() {
   if (stringComplete) {
    Serial.println(inputString); 
    //Okay let's figure out what to do with the tree
    //If we're changing the color, we have to do some heavy lifting :)
    if(inputString.startsWith("Color:")){
      if(setSecondary == true){
        secondaryColor = parseColor(inputString);
        setSecondary = false;
      }else {
        curColor = parseColor(inputString);
      }
      Serial2.write(0x10);      
    } else if(inputString.startsWith("Command:")){
      if(runCommand(inputString)){
        Serial2.write(0x10);  
      }
    } else {
      Serial2.write(0x01);      
    }
    
    inputString = "";
    stringComplete = false;
    noNewInput = true;
  }
  if(newState != currentState) {
   currentState = newState; 
  }
  switch(currentState){
  case Steady :
      setColor(curColor);
      break;
  case Wiping :
      colorWipe(returnColorWipe(curColor,50),15);
      break;
  case Blinking :
      setBlink(curColor,100);
      break;    
  case ColorBlinking :
      setColorBlink(curColor,secondaryColor,100);
      break;
  case Rainbows :
      rainbowCycle(0);
      break;
  case Checker :
      animateChecker(curColor,secondaryColor,100);
      break;
  case Pulsing :
      colorPulse(200,150,20);  //max brightness, min brightness, delay
      break;
  case Splosions :
      setSploding();
      break;
  default :
      newState = Steady;
      break;    
  }
  
  delay(20);
}

void serialEvent1() {
  //Serial.write("event");      
}

void serialEvent2() {
  //Serial.println("this is happening \n");
  while (Serial2.available()) {
    // get the new byte:
    char inChar = (char)Serial2.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

void setSploding() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(random(255),random(255),random(255)));
  }
  strip.show();
}

void setBlink(uint32_t c,uint8_t wait){ 
  _setColor(c,wait);
  _setColor(strip.Color(0,0,0),wait);
  delay(wait);
}

void setColorBlink(uint32_t c,uint32_t c2, uint8_t wait){
   _setColor(c,wait);
   _setColor(c2,wait);  
}

void _setEveryOtherColor(uint8_t k,uint32_t c,uint8_t wait) {//functions prefixed with underscore denote delays
  for(uint16_t i=0; i<strip.numPixels(); i+=k) {
      strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

void animateChecker(uint32_t c, uint32_t c2,uint8_t wait){
  setChecker(c,c2,wait);
  setChecker(c2,c,wait);
  delay(wait);
}

void setChecker(uint32_t col1, uint32_t col2,uint8_t wait){
   _setEveryOtherColor(1,col1,wait);
   _setEveryOtherColor(2,col2,wait);
} 

void _setColor(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

void setEveryOtherColor(uint32_t c,uint8_t k) {
  for(uint16_t i=0; i<strip.numPixels(); i+=k) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

void setColor(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

void colorPulseUp(uint16_t bMax, uint16_t bMin, uint8_t wait) { 
   for(uint16_t j = bMin; j < bMax ; j++){
      strip.setBrightness(j); 
      strip.show();
      delay(wait); 
   }
}

void colorPulseDown(uint16_t bMax, uint16_t bMin, uint8_t wait) { 
   for(uint16_t j = bMax; j > bMin ; j--){
      strip.setBrightness(j); 
      strip.show();
      delay(wait); 
   }
}

void _colorPulse(uint16_t bMax, uint16_t bMin, uint8_t wait) {
  colorPulseDown(bMax,bMin,wait);
  delay(wait);
  colorPulseUp(bMax,bMin,wait);  
}

void colorPulse(uint16_t bMax, uint16_t bMin, uint8_t wait) {
  colorPulseDown(bMax,bMin,wait);
  colorPulseUp(bMax,bMin,wait);  
}

void colorWipe(uint32_t c, uint8_t wait) {
  if(c != secondaryColor){
    c = secondaryColor;
  } else {
    c = curColor;
  }
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

uint32_t returnColorWipe(uint32_t c, uint8_t wait) {
  if(c != secondaryColor){
    c = secondaryColor;
  }
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
  return c;
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void everyOtherWipe(uint8_t k, uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i+=k) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

//==== Handle Commands ====//
boolean runCommand(String inputString) {
  String cmd = inputString.substring(8);
  Serial.print("Command is " + cmd);
  if(cmd.startsWith("Blink")){
    //do blink
    newState = Blinking;
  } else if (cmd.startsWith("ColorBlink")){
    //do steady
    newState = ColorBlinking;
  } else if (cmd.startsWith("Steady")){
    //do steady
    newState = Steady;
  } else if (cmd.startsWith("Checker")){
    //do steady
    newState = Checker;
  } else if (cmd.startsWith("Rainbow")){
    //do rainbows
    newState = Rainbows;
  } else if (cmd.startsWith("Wipe")){
    //do wiping
    newState = Wiping;
  } else if (cmd.startsWith("Pulse")){
    //do Pulsing
    newState = Pulsing;
  } else if (cmd.startsWith("Splode")){
    //do splodes
    newState = Splosions;
  } else if (cmd.startsWith("ChangeSecondary")){
     //change second color
    setSecondary = true; 
  } else {
    Serial2.write(0x01);
    return false;
  }  
}


//==== Other Important Things ====//


String getValue(String data, char separator, int index) //Thanks alvarolb from Stack Overflow!
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


uint32_t parseColor(String inputString){
  String ps = inputString.substring(6);
  ps.trim();
  //Serial.print("Numbers are " + ps);        //Make sure you're getting the proper length of the string or else you can't cast to int!
  int red = getValue(ps,',',0).toInt();
  int green = getValue(ps,',',1).toInt();
  int blue = getValue(ps,',',2).toInt();
  if(setSecondary){
    Serial.print("New Secondary Color is: [" + getValue(ps,',',0) + "," + getValue(ps,',',1) + "," + getValue(ps,',',2) + "]");
  } else {
    Serial.print("New Color is: [" + getValue(ps,',',0) + "," + getValue(ps,',',1) + "," + getValue(ps,',',2) + "]");
  }
  return strip.Color(red,green,blue);    
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
