

#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel
#include <DS3232RTC.h>         // http://github.com/JChristensen/DS3232RTC
#include <Time.h>              // http://www.arduino.cc/playground/Code/Time
#include <Wire.h>              // http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include <EEPROM.h>

// My two helper libs that hold the extran precision math, and the astronomical calculations.
extern "C" {
#include "float64.h"
#include "AstronomicalCalculations.h"
}

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

// The various moon arcs, have digit equivalents, so we list those here.
// The percentages go from upper right to lower left.
#define ZERO_PERCENT_MOON_ARC 8
#define TWENTY_PERCENT_MOON_ARC 6
#define FORTY_PERCENT_MOON_ARC 4
#define SIXTY_PERCENT_MOON_ARC 3
#define EIGHTY_PERCENT_MOON_ARC 5
#define ONE_HUNDRED_PERCENT_MOON_ARC 7

#define SUNDAY 0
#define MONDAY 1
#define TUESDAY 2
#define WEDNESDAY 3
#define THURSDAY 4
#define FRIDAY 5
#define SATURDAY 6
#define UNKNOWN_DAY_OF_WEEK 255

#define JANUARY 1
#define FEBRUARY 2
#define MARCH 3
#define APRIL 4
#define MAY 5
#define JUNE 6
#define JULY 7
#define AUGUST 8
#define SEPTEMBER 9
#define OCTOBER 10
#define NOVEMBER 11
#define DECEMBER 12

typedef struct {
  unsigned short year;
  unsigned char month;
  unsigned char day;

  int gmtOffsetInSeconds;

  float longitude;
  float latitude;

  unsigned char springEquinoxDay;  // in March
  unsigned char summerSolsticeDay; // in June
  unsigned char fallEquinoxDay;    // in September
  unsigned char winterSolsticeDay; // in December
  
  unsigned char easterDay;
  
} eepromInfo;

void setup(void)
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



int 
twelveHourValueFrom24HourTime(int hour)
{
  while (hour > 12) {
    hour -= 12;
  }
  if (hour == 0) {
    hour = 12;
  }
  return hour;
}

void 
setLedsWithTime(int hours, int minutes, uint32_t color)
{
  if (minutes > 59) {
    minutes = 59;
  }

  hours = twelveHourValueFrom24HourTime(hours);

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

void 
setLedsWithDigits(int hours, int minutes, uint32_t color)
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

void 
displayCountdown(int minutes, int seconds)
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


void
setMoonPhaseLeds(int year, int month, int day, int hour)
{
    float fday = (float)day + (float)hour/24.0;
    float percentIlluminated =  illuminatedFractionKForDate(year, month, fday);
    boolean isWaxing = isWaxingForDate(year, month, fday);
    
    // If the moon is going to over lap with a digit we show it very dimmly.
    hour = twelveHourValueFrom24HourTime(hour);
    uint32_t moonColor = (hour <= 9) ? leds.Color(255, 248, 209) : leds.Color(15,18,13);

    // We use a .05 on either side to make new/full a bit more discriminating.  We might want to
    // make it even smaller.
    if (isWaxing) {
        if (percentIlluminated < 0.05) {
            // New moon, we're done.
        } else {
            // Turn on the upper right rim.
            leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[ZERO_PERCENT_MOON_ARC], moonColor);
            
            if (percentIlluminated < 0.3) {
                leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[TWENTY_PERCENT_MOON_ARC], moonColor);
            } else if (percentIlluminated < 0.5) {
                leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[FORTY_PERCENT_MOON_ARC], moonColor);
            } else if (percentIlluminated < 0.7) {
                leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[SIXTY_PERCENT_MOON_ARC], moonColor);
            } else if (percentIlluminated < 0.95) {
                leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[EIGHTY_PERCENT_MOON_ARC], moonColor);
            } else {
                leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[ONE_HUNDRED_PERCENT_MOON_ARC], moonColor);
            }
        }
    } else {
       // Use the lower left rim.
        if (percentIlluminated < 0.05) {
            // New moon, we're done.
        } else {
            // Turn on the lower left rim.
            leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[ONE_HUNDRED_PERCENT_MOON_ARC], moonColor);
            
            if (percentIlluminated < 0.3) {
                leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[EIGHTY_PERCENT_MOON_ARC], moonColor);
            } else if (percentIlluminated < 0.5) {
                leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[SIXTY_PERCENT_MOON_ARC], moonColor);
            } else if (percentIlluminated < 0.7) {
                leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[FORTY_PERCENT_MOON_ARC], moonColor);
            } else if (percentIlluminated < 0.95) {
                leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[TWENTY_PERCENT_MOON_ARC], moonColor);
            } else {
                leds.setPixelColor(HOUR_TENS_BASE + gTensDigit[ZERO_PERCENT_MOON_ARC], moonColor);
            }
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
  setMoonPhaseLeds(year(t), month(t), day(t), hour(t));
  leds.show();
}

void clearLeds()
{
  for (int i=0; i<LED_COUNT; i++) {
    leds.setPixelColor(i, 0);
  }
}

