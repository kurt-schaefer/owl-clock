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

// The various moon arcs, have digit equivalents, so we list those here.
// The percentages go from upper right to lower left.
#define ZERO_PERCENT_MOON_ARC 8
#define TWENTY_PERCENT_MOON_ARC 6
#define FORTY_PERCENT_MOON_ARC 4
#define SIXTY_PERCENT_MOON_ARC 3
#define EIGHTY_PERCENT_MOON_ARC 5
#define ONE_HUNDRED_PERCENT_MOON_ARC 7

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

static const float degToRad = M_PI/180.0;

float
moonsMeanAnomalyMPrime(float T)
{
    // From p308
    return 134.9634114 + 477198.8676313*T + 0.0089970*T*T - (T*T*T)/69699.0 - (T*T*T*T)/14712000.0;
}

float
sunsMeanAnomalyM(float T)
{
    // From p308
    return 357.5291092 + 35999.0502909*T - 0.0001536*T*T + (T*T*T)/24490000.0;
}

float
moonsMeanElongationD(float T)
{
    // From p308
    return 297.8502042 + 445267.1115168*T - 0.0013268*T*T + (T*T*T)/545868.0 - (T*T*T*T)/113065000.0;
}

float
moonsPhaseAngleInDegI(float T)
{
    // From p316
    float D = moonsMeanElongationD(T);
    float MPrime = moonsMeanAnomalyMPrime(T);
    float M = sunsMeanAnomalyM(T);
    
    return (180.0 - D - 6.289*sin(degToRad*MPrime)
            + 2.100*sin(degToRad*M)
            - 1.274*sin(degToRad*(2.0*D - MPrime))
            - 0.658*sin(degToRad*2.0*D)
            - 0.214*sin(degToRad*2.0*MPrime)
            - 9.110*sin(degToRad*D));
}

float
illuminatedFractionK(float T)
{
     return (1.0 + cos(degToRad*moonsPhaseAngleInDegI(T)))/2.0;
}

// only day can have a decimal fraction.
float
julianEphemerisDay(float year, float month, float day)
{
    // From p61
    if (month <= 2.0) {
        year = year - 1;
        month = month + 12;
    }
    
    float A = floorf(year/100.0);
    float B = 2 - A + floorf(A/4.0);
    
    return floorf(365.25*(year + 4716.0)) + floorf(30.6001 * (month + 1.0)) + day + B - 1524.5;
}

// Compute the time in centuries since J2000 known as the Julian Year (T)
// We compute it using time_t time since 1970
float
julianYearFromTime(float year, float month, float day)
{
    float JED = julianEphemerisDay(year, month, day);
    return (JED - 2451545.0)/36525.0;
}

boolean
isWaxingFromTime(float year, float month, float day)
{
  float illuminationNow = illuminatedFractionK(julianYearFromTime(year, month, day));
  float illuminationSoon = illuminatedFractionK(julianYearFromTime(year, month, day + 0.1));
    
  return illuminationSoon > illuminationNow;
}

void
setMoonPhaseLeds(int year, int month, int day, int hour)
{
    float fday = (float)day + (float)hour/24.0;
    float percentIlluminated =  illuminatedFractionK(julianYearFromTime(year, month, fday));
    boolean isWaxing = isWaxingFromTime(year, month, fday);
    
    // If the moon is going to over lap with a digit we show it very dimmly.
    while (hour > 12) {
      hour -= 12;
    }
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

