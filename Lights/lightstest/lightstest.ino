
#include <Adafruit_NeoPixel.h>

#define DEBUG 1

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 60 
#define MAX_HUE 65535

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// For Fade mode
int chaseLoc = 0;  // LED location of chase
bool chaseDir = true; // true = up, false = down
int lastColor = 0;

// For rainbo mode
#define RAINBOW_STEPS 200
int rainbow_LastColor = 0;

// For stars mode
#define STARS_RATIO 20
#define STARS_BRIGHTSTEP 20
int starsCount = LED_COUNT/STARS_RATIO;
int stars_location[LED_COUNT/STARS_RATIO];
int stars_brightness[LED_COUNT/STARS_RATIO];

// Modes for lights
enum modes {
    Rainbow,
    Fade,
    Stars
  };

modes mode;
  
void setup() {

  #ifdef DEBUG
  Serial.begin(9600);
  Serial.print("serial test 0021");
  #endif
  
  // for indicator pin
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Set up strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  for(int i = 0; i < LED_COUNT/STARS_RATIO; i++)
  {
    stars_location[i] = 0;
  }
  
  mode = Stars;
}

void loop() {
  
  switch (mode) {
    case Rainbow:
      rainbow();
      delay(25);
      break;
      
    case Fade:
      fade(true);
      delay(50);
      break;

    case Stars:
      stars();
      delay(100);
      break;

  }
}

// direction - True = Up, False = down
void fade(bool chase)
{
  lastColor++;
  if(lastColor > 255)
  {
    lastColor = 0;
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
    
    color = strip.ColorHSV(hue, 255, 128);
    strip.setPixelColor(i, color);
  }
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
  
  bool newStarFound = false;
  
  // Calculate colors
  for (int i = 0; i <= starsCount; i++)
  {
    if(stars_location[i] == 0 && !newStarFound)
    {
      stars_location[i] = random(LED_COUNT);
      stars_brightness[i] = 255;
    }
  }
  
  // Paint Colors
  for (int i = 0; i <= starsCount; i++)
  {
    if(stars_location[i] > 0)
    {
      color = strip.ColorHSV(0, 0, stars_brightness[i]);
      strip.setPixelColor(stars_location[i], color);
      stars_brightness[i] = stars_brightness[i] - STARS_BRIGHTSTEP;
      
      if(stars_brightness[i] < 0)
      {
        stars_brightness[i] = 0;
        stars_location[i] = 0;
      }
    }
  }
  strip.show();
}
