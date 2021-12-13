#include "Adafruit_NeoPixel.h"
#include "declarations.h"
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

// For rainbow mode
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

// For Christmas Chase
#define CC_STEP 3   // Step for each pixel

// Modes for lights
enum modes {
  Rainbow,
  Fade,
  Stars,
  Christmas,
  ChristmasChase,
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
  for (int i = 0; i < LED_COUNT / STARS_RATIO; i++)
  {
    stars_location[i] = -1;
  }

  mode = Rainbow;
}

void loop() {

  processButton();

  // Determine mode and execute
  switch (mode) {

    case Rainbow:
      if (buttonPress)
      {
        mode = Fade;
      }
      rainbow();
      delay(20);
      break;

    case Fade:
      if (buttonPress)
      {
        mode = Christmas;
      }
      fade();
      delay(50);
      break;

    case Christmas:
      if (buttonPress)
      {
        mode = ChristmasChase;
      }
      christmas();
      delay(10);
      break;

    case ChristmasChase:
      if (buttonPress)
      {
        mode = Stars;
      }
      christmasChase();
      delay(20);
      break;

    case Stars:
      if (buttonPress)
      {
        mode = Icicle;
      }
      stars();
      delay(STARS_DELAY);
      break;

    case Icicle:
      if (buttonPress)
      {
        mode = Rainbow;
      }
      icicle();
      delay(40);
      break;
  }
}

// Determines button state and sets buttonPress
void processButton()
{
  // Process pushbutton
  int pushbuttonVal = digitalRead(PUSHBUTTON_PIN);
  if (pushbuttonVal == HIGH)
  {
    digitalWrite(LED_BUILTIN, LOW);
    buttonPress = false;
    latch = false;
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH);
    if (!buttonPress && !latch)
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

// Fade Mode
// direction - True = Up, False = down
void fade()
{
  if (fadeDir)
  {
    lastColor++;
  }
  else
  {
    lastColor--;

  }
  if (lastColor > 255)
  {
    lastColor = 254;
    fadeDir = false;
  }
  else if (lastColor < 1)
  {
    lastColor = 1;
    fadeDir = true;
  }

  uint32_t color = strip.Color(lastColor, lastColor, lastColor);
  strip.fill(color, 0, LED_COUNT);

  strip.show();
}

// direction - True = Up, False = down
// makes rainbow lights chase up the string
void rainbow()
{
  uint32_t color;
  uint32_t colorStep = MAX_HUE / RAINBOW_STEPS;

  rainbow_LastColor = rainbow_LastColor + colorStep;
  if (rainbow_LastColor > MAX_HUE)
  {
    rainbow_LastColor = 0;
  }

  // Calculate colors
  for (int i = 0; i <= LED_COUNT; i++)
  {
    int hue = rainbow_LastColor + (colorStep * i);
    if (hue > MAX_HUE)
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

  strip.show();
}


// Stars Mode
void stars()
{
  Serial.println("Stars");
  uint32_t color;

  bool establishStar = (random(10) == 1);  // Determine if we are going to estabilsh a star
  bool newStarFound = false; // True if a star has been established this time

  for (int i = 0; i < starsCount; i++)
  {
    if (stars_location[i] == -1 && establishStar && !newStarFound)
    {
      stars_location[i] = random(LED_COUNT);
      stars_brightness[i] = 255;
      newStarFound = true;
    }
    else if (stars_location[i] > -1)
    {
      if (stars_brightness[i] <= STARS_BRIGHTSTEP)
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
    if (stars_location[i] > -1)
    {
      color = strip.ColorHSV(0, 0, stars_brightness[i]);
      strip.setPixelColor(stars_location[i], color);
    }
  }

  strip.show();
}


// Christmas Mode
christmasModeEnum christmasMode = Red;   // Default
bool christmasFadeDirection = true;

void christmas()
{
  if (christmasFadeDirection)
  {
    lastColor++;
  }
  else
  {
    lastColor--;
  }

  if (lastColor > 255)
  {
    lastColor = 254;
    christmasFadeDirection = false;
  }
  else if (lastColor < 1)
  {
    lastColor = 1;
    christmasFadeDirection = true;
    if (christmasMode == Red)
    {
      christmasMode = Green;
    }
    else
    {
      christmasMode = Red;
    }
  }

  uint32_t color = 0;

  if (christmasMode == Red)
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

// Christmas Mode
// based on the first

ccLightStatus ccFirst = { 0, Red, true };

void christmasChase()
{
  uint32_t color;
  ccProcessColor(ccFirst);
  ccLightStatus ccOther = { ccFirst.color, ccFirst.mode, ccFirst.fadeDirection };

  for (int i = 0; i < LED_COUNT; i++)
  {
    ccProcessColor(ccOther);

    switch (ccOther.mode)
    {
      case Red:
        color = strip.Color(ccOther.color, 0, 0);
        break;
      case Green:
        color = strip.Color(0, ccOther.color, 0);
        break;
    }

    strip.setPixelColor(i, color);
  }
  strip.show();
}


void ccProcessColor(ccLightStatus& lightstatus)
{
  if (lightstatus.fadeDirection)
  {
    lightstatus.color = lightstatus.color + CC_STEP;
  }
  else
  {
    lightstatus.color = lightstatus.color - CC_STEP;
  }

  if (lightstatus.color > 100)
  {
    lightstatus.color = 99;
    lightstatus.fadeDirection = false;
  }
  else if (lightstatus.color < 1)
  {
    lightstatus.color = 1;
    lightstatus.fadeDirection = true;

    if (lightstatus.mode == Red)
    {
      lightstatus.mode = Green;
    }
    else
    {
      lightstatus.mode = Red;
    }
  }
 
}
