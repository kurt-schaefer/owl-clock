#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel
#include <DS3232RTC.h>         // http://github.com/JChristensen/DS3232RTC
#include <Time.h>              // http://www.arduino.cc/playground/Code/Time
#include <Wire.h>              // http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include "WS2812_Definitions.h"

#define LED_DIG_OUT 6
#define LED_COUNT 1  // TODO Eventually this will be 40

#define PIN 6 // Remove these eventually

// Neo pixel interface for all the LEDS is though the single pin LED_DIG_OUT
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, LED_DIG_OUT, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(19200);

  //setSyncProvider() causes the Time library to synchronize with the
  //external RTC by calling RTC.get() every five minutes by default.
  setSyncProvider(RTC.get);
  
  if (timeStatus() != timeSet) {
    Serial.print(F("RTC Sync setup failed."));
  }
 
  leds.begin();
  clearLEDs();
  leds.show();
}

void loop()
{
    delay(1000);
}

// Sets all LEDs to off, but DOES NOT update the display;
// call leds.show() to actually turn them off after this.
void clearLEDs()
{
  for (int i=0; i<LED_COUNT; i++) {
    leds.setPixelColor(i, 0);
  }
}

