#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel
#include <DS3232RTC.h>         // http://github.com/JChristensen/DS3232RTC
#include <Time.h>              // http://www.arduino.cc/playground/Code/Time
#include <Wire.h>              // http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include <EEPROM.h>


#define LIGHT_SENSOR_TOP_ANALOG_IN     A0
#define LIGHT_SENSOR_INSIDE_ANALOG_IN  A1
#define LED_COUNT 40

#define LED_DIG_OUT        6

// TODO convert these to 0,1,2 I hope using 0 & 1 isn't a big problem.  :b
#define SWITCH_UP_DIG_IN   4
#define SWITCH_DOWN_DIG_IN 5
#define SWITCH_SET_DIG_IN  7

// Neo pixel interface for all the LEDS is though the single pin LED_DIG_OUT
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, LED_DIG_OUT, NEO_GRB + NEO_KHZ800);

// digit to LED index for the clock digits. Tens and Ones are sequenced differently.
static const unsigned char gTensDigit[] = {6, 9, 4, 3, 1, 8, 5, 2, 0, 7};
static const unsigned char gOnesDigit[] = {5, 0, 3, 6, 8, 1, 4, 7, 9, 2};

static int gGMTOffsetInSeconds = -8*60*60;

// To addess each digit use these bases + the above index.
#define HOUR_TENS_BASE    20
#define HOUR_ONES_BASE    30
#define MINUTE_TENS_BASE   0
#define MINUTE_ONES_BASE  10

void setup()
{
  // TODO: Only switch these to 0, 1, 2 when we're programming the actual boards, 
  // otherwise Serial uses 0 and 1.
//  pinMode(SWITCH_UP_DIG_IN, INPUT_PULLUP);
//  pinMode(SWITCH_DOWN_DIG_IN, INPUT_PULLUP);
//  pinMode(SWITCH_SET_DIG_IN, INPUT_PULLUP);
  
//  Serial.begin(19200);

  //setSyncProvider() causes the Time library to synchronize with the
  //external RTC by calling RTC.get() every five minutes by default.
 setSyncProvider(RTC.get);
  
 // if (timeStatus() != timeSet) {
 //   Serial.print(F("RTC Sync setup failed."));
 // }
 
  leds.begin();
  clearLeds();
  leds.show();
}

void setLedsWithTime(int hours, int minutes, uint32_t color)
{
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

void setLedsWithDigits(int hours, int minutes, uint32_t color)
{
  int hourTens = floor(hours/10);
  int hourOnes = hours - 10*hourTens;
  
  leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[hourTens], color);
  leds.setPixelColor(HOUR_ONES_BASE + gOnesDigit[hourOnes], color);

  int minuteTens = floor(minutes/10);
  int minuteOnes = minutes - minuteTens*10;

  leds.setPixelColor(MINUTE_TENS_BASE + gTensDigit[minuteTens], color);
  leds.setPixelColor(MINUTE_ONES_BASE + gOnesDigit[minuteOnes], color);
}

void displayCountdown(int minutes, int seconds)
{
  while (minutes > 0 && seconds > 0) {
    seconds--;
    if (seconds < 0) {
      seconds = 59;
      minutes--;
    }

    clearLeds();
    setLedsWithTime(minutes, seconds, leds.Color(255, 128, 128));
    leds.show();
    delay(1000);
  }
}


void colorTest()
{
  for (int i=0; i<10; ++i) {
    clearLeds();
    setLedsWithDigits(0, i, leds.Color(255, 0, 0));
    leds.show();
    delay(500);
    
    clearLeds();
    setLedsWithDigits(0, i, leds.Color(0, 255, 0));
    leds.show();
    delay(500);
    
    clearLeds();
    setLedsWithDigits(0, i, leds.Color(0, 0, 255));
    leds.show();
    delay(500);
  }
   for (int i=0; i<10; ++i) {
    clearLeds();
    setLedsWithDigits(0, i*10, leds.Color(255, 0, 0));
    leds.show();
    delay(500);
    
    clearLeds();
    setLedsWithDigits(0, i*10, leds.Color(0, 255, 0));
    leds.show();
    delay(500);
    
    clearLeds();
    setLedsWithDigits(0, i*10, leds.Color(0, 0, 255));
    leds.show();
    delay(500);
  }
   for (int i=0; i<10; ++i) {
    clearLeds();
    setLedsWithDigits(i, 0, leds.Color(255, 0, 0));
    leds.show();
    delay(500);
    
    clearLeds();
    setLedsWithDigits(i, 0, leds.Color(0, 255, 0));
    leds.show();
    delay(500);
    
    clearLeds();
    setLedsWithDigits(i, 0, leds.Color(0, 0, 255));
    leds.show();
    delay(500);
  }
   for (int i=0; i<10; ++i) {
    clearLeds();
    setLedsWithDigits(i*10, 0, leds.Color(255, 0, 0));
    leds.show();
    delay(500);
    
    clearLeds();
    setLedsWithDigits(i*10, 0, leds.Color(0, 255, 0));
    leds.show();
    delay(500);
    
    clearLeds();
    setLedsWithDigits(i*10, 0, leds.Color(0, 0, 255));
    leds.show();
    delay(500);
  }
}
void test()
{ 
  for (int j=0; j<100; j += 1) {
  for (int i=0; i<100; i += 1) {
    clearLeds();
    setLedsWithDigits(j, i, leds.Color(255, 10,10));
    leds.show();
    delay(100);
  }
  }
}

void loop()
{
//   colorTest();
// while (1) {
//  test();
// }

    time_t t = now() + gGMTOffsetInSeconds;
    clearLeds();
    setLedsWithTime(hour(t), minute(t), leds.Color(255, 10, 10));
    leds.show();
}

void clearLeds()
{
  for (int i=0; i<LED_COUNT; i++) {
    leds.setPixelColor(i, 0);
  }
}

