#include "Adafruit_NeoPixel.h"

#define DEBUG 1

// Pins
#define LED_PIN 6
#define PUSHBUTTON_PIN 2

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 120
#define MAX_HUE 65535

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
bool buttonPress = false;
bool latch = false;

// For Fade mode
int chaseLoc = 0;  // LED location of chase
bool chaseDir = true; // true = up, false = down
bool fadeDir = true; // true = up, false = down
int lastColor = 0;

// For rainbo mode
#define RAINBOW_STEPS 50
int rainbow_LastColor = 0;

// For stars mode
#define STARS_RATIO 20
#define STARS_BRIGHTSTEP 1
#define STARS_DELAY 20
int starsCount = 20;
int stars_location[20];
int stars_brightness[20];

// For Icicle Mode
int icicleLastPos = LED_COUNT;

// For Off
bool turnedOff = false;

// Modes for lights
enum modes {
    Rainbow,
    Fade,
    Stars,
    Christmas,
    Icicle,
    Off
  };

modes mode;
  
void setup() 
{
  #ifdef DEBUG
  Serial.begin(9600);
  Serial.print("serial test 0021");
  #endif
  
  // for indicator pin
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PUSHBUTTON_PIN, INPUT_PULLUP);

  // Set up strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Setup Star Location
  for(int i = 0; i < LED_COUNT/STARS_RATIO; i++)
  {
    stars_location[i] = -1;
  }

  mode = Icicle;
}

void loop() {

  processButton();
  
  switch (mode) {
    case Off:
      digitalWrite(13,HIGH);
      delay(2000);
      digitalWrite(13,LOW);
      delay(5000);
      if(!turnedOff)
      {
        uint32_t color = strip.Color(lastColor, 0, 0);  
        strip.fill(color, 0, LED_COUNT);
        strip.show();
      }
  
      processButton();
      if(buttonPress)
      {
        mode = Rainbow;
      }
      break;

    case Icicle:
      if(buttonPress)
      {
        mode = Rainbow;
      }
      icicle();
      delay(40);
      break;
      
    case Rainbow:
      if(buttonPress)
      {
        mode = Fade;
      }
      rainbow();
      delay(20);
      break;
      
    case Fade:
      if(buttonPress)
      {
        mode = Christmas;
      }
      fade(false);
      delay(50);
      break;

    case Christmas:
      if(buttonPress)
      {
        mode = Stars;
      }
      christmas();
      delay(10);
      break;

    case Stars:
      if(buttonPress)
      {
        mode = Icicle;
      }
      stars();
      delay(STARS_DELAY);
      break;
  }
}

void processButton()
{
    // Process pushbutton
  int pushbuttonVal = digitalRead(PUSHBUTTON_PIN);
  if(pushbuttonVal == HIGH)
  {
    digitalWrite(LED_BUILTIN, LOW); 
    buttonPress = false;
    latch = false;
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH);
    if(!buttonPress && !latch)
    {
      buttonPress = true;
      latch = true;
    }
    else
    {
      buttonPress = false;
    }
  }
}

// direction - True = Up, False = down
void fade(bool chase)
{
  if(fadeDir)
  {
    lastColor++;
  }
  else
  {
    lastColor--;
    
  }
  if(lastColor > 255)
  {
    lastColor = 254;
    fadeDir = false;
  }
  else if(lastColor < 1)
  {
    lastColor = 1;
    fadeDir = true;
  }

  uint32_t color = strip.Color(lastColor, lastColor, lastColor);
  strip.fill(color, 0, LED_COUNT);
  
  if(chase)
  {
    strip.setPixelColor(chaseLoc, 255-lastColor, 255-lastColor, 255-lastColor);
    
    if(chaseLoc > LED_COUNT)
    {
      chaseDir = false;
      chaseLoc = LED_COUNT - 1;  
    }
    
    if(chaseLoc < 0)
    {
      chaseDir = true;
      chaseLoc = 1;
    }

    if(chaseDir)
    {
        chaseLoc++;
    }
    else
    {
      chaseLoc--;
    }
  }
  strip.show();
}

// direction - True = Up, False = down
// makes rainbow lights chase up the string
void rainbow()
{
  uint32_t colors[LED_COUNT];
  uint32_t color;
  uint32_t colorStep = MAX_HUE/RAINBOW_STEPS;
  
  rainbow_LastColor = rainbow_LastColor + colorStep;
  if(rainbow_LastColor > MAX_HUE)
  {
    rainbow_LastColor = 0;
  }
  
  // Calculate colors
  for (int i = 0; i <= LED_COUNT; i++)
  {
    int hue = rainbow_LastColor + (colorStep * i);
    if(hue > MAX_HUE)
    { 
      hue = hue - MAX_HUE;
    }
    
    color = strip.ColorHSV(hue, 255, 255);
    strip.setPixelColor(i, color);
  }
  strip.show();
}


void icicle()
{
  uint32_t colors[LED_COUNT];
  uint32_t color;
 
  icicleLastPos--;
  if (icicleLastPos < 1)
  {
    icicleLastPos = LED_COUNT;
  }

  color = strip.ColorHSV(38228, 230, 125);
  strip.fill(color, 0, LED_COUNT);

  for (int i = 0; i < 6; i++)
  {
    color = strip.ColorHSV(38228, 1, 255);
    strip.setPixelColor(icicleLastPos - i, color);
  }
  /*
  // Calculate colors
  for (int i = 0; i <= LED_COUNT; i++)
  {
    int hue = rainbow_LastColor + (colorStep * i);
    if(hue > MAX_HUE)
    { 
      hue = hue - MAX_HUE;
    }
    
    color = strip.ColorHSV(hue, 255, 255);
    strip.setPixelColor(i, color);
  }
*/
  strip.show();
}


// direction - True = Up, False = down
// makes rainbow lights chase up the string

// For Stars
//int starsCount = LED_COUNT/STARS_RATIO;
//int stars_location[LED_COUNT/STARS_RATIO];
//int stars_brightness[LED_COUNT/STARS_RATIO];

void stars()
{
  Serial.println("Stars");
  int colorInt = 0;
  uint32_t color;
  int establishStar = (random(10) == 1);
  
  bool newStarFound = false;
  
  // Calculate colors
  for (int i = 0; i < starsCount; i++)
  {
    if(stars_location[i] == -1 && establishStar && !newStarFound)
    {
      stars_location[i] = random(LED_COUNT);
      stars_brightness[i] = 255;
      newStarFound = true;
    }
    else if(stars_location[i] > -1)
    {
      if(stars_brightness[i] <= STARS_BRIGHTSTEP)
      {
        stars_brightness[i] = 0;
        strip.setPixelColor(stars_location[i], 0, 0, 0);
        stars_location[i] = -1;
      }
      else
      {
        stars_brightness[i] = stars_brightness[i] - STARS_BRIGHTSTEP;
      }
    }
  }

  //Clear Strip
  color = strip.Color(0, 0, 0);
  strip.fill(color, 0, LED_COUNT);

  // Paint Colors
  for (int i = 0; i < starsCount; i++)
  {
    if(stars_location[i] > -1)
    {
      color = strip.ColorHSV(0, 0, stars_brightness[i]);
      strip.setPixelColor(stars_location[i], color);
    }
  }

  strip.show();
}

// direction - True = Up, False = down
enum christmasModeEnum {red, green};
christmasModeEnum christmasMode = red;

void christmas()
{
  if(chaseDir)
  {
    lastColor++;
  }
  else
  {
    lastColor--;
  }
  
  if(lastColor > 255)
  {
    lastColor = 254;
    chaseDir = false;
  }
  else if(lastColor < 1)
  {
    lastColor = 1;
    chaseDir = true;
    if(christmasMode == red)
    {
      christmasMode = green;
    }
    else
    {
      christmasMode = red;
    }
  }

  uint32_t color = 0;
  
  if(christmasMode == red)
  {
    color = strip.Color(lastColor, 0, 0);  
  }
  else
  {
    color = strip.Color(0, lastColor, 0);  
  }
  
  strip.fill(color, 0, LED_COUNT);
  strip.show();
}
