#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel
#include <DS3232RTC.h>         // http://github.com/JChristensen/DS3232RTC
#include <Time.h>              // http://www.arduino.cc/playground/Code/Time
#include <Wire.h>              // http://arduino.cc/en/Reference/Wire (included with Arduino IDE)

#define LED_DIG_OUT 6
#define LED_COUNT 1  // TODO Eventually this will be 40

// Neo pixel interface for all the LEDS is though the single pin LED_DIG_OUT
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, LED_DIG_OUT, NEO_GRB + NEO_KHZ800);

// digit to LED index for the clock digits. Tens and Ones are sequenced differently.
static const unsigned char gTensDigit[] = {6, 9, 4, 3, 1, 8, 5, 2, 0, 7};
static const unsigned char gOnesDigit[] = {5, 0, 3, 6, 8, 1, 4, 7, 9, 2};

// To addess each digit use these bases + the above index.
#define HOUR_TENS_BASE 20
#define HOUR_ONES_BASE 30
#define MINUTE_TENS_BASE   0
#define MINUTE_ONES_BASE  10

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

void showTime(int hours, int minutes, uint32_t color)
{
  clearLEDs();

  if (minutes > 59) {
    minutes = 59;
  }

  if (hours > 24) {
    hours = 24;
  }

  // For now only support 12 hour display.
  while (hours > 12) {
    hours -= 12;
  }

  int hourTens = floor(hours/10);
  int hourOnes = hours - 10*hourTens;
  
  if (hourTens > 0) {
    leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[hourTens], color);
  }
  leds.setPixelColor(HOUR_ONES_BASE + gOnesDigit[hourOnes], color);

  int minuteTens = floor(minutes/10);
  int minuteOnes = minutes - minuteTens*10;

  leds.setPixelColor(MINUTE_TENS_BASE + gTensDigit[minuteTens], color);
  leds.setPixelColor(MINUTE_ONES_BASE + gOnesDigit[minuteOnes], color);
}

void loop()
{
    delay(500);

    time_t t = now();
    showTime(hour(t), minute(t), leds.Color(255, 10, 10));
}

// Sets all LEDs to off, but DOES NOT update the display;
// call leds.show() to actually turn them off after this.
void clearLEDs()
{
  for (int i=0; i<LED_COUNT; i++) {
    leds.setPixelColor(i, 0);
  }
}

