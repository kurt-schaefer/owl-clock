
//
//  AstronomicalCalculations.c
//  OwlSim
//
//  Created by Kurt Schaefer on 2/15/16.
//  Copyright © 2016 Kurt Schaefer. All rights reserved.
//

#include "AstronomicalCalculations.h"
#include "Math.h"

// This code doesn't work that well with 4 byte doubles.  The float64 code I wrote to compute this
// stuff is huge though, so I'm converting back to save space, and hopefully the values won't
// be that horribly far off.

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
    // Replace this with day(time), year(time), etc.
    //   NSDate *date = [NSDate dateWithTimeIntervalSince1970:time];
    // From p131
    float JED = julianEphemerisDay(year, month, day);
    return (JED - 2451545.0)/36525.0;
}

void
easterForYear(int year, unsigned char *monthOut, unsigned char *dayOut)
{
    // From page 67
    
    int a = year % 19;
    int b = year/100;
    int c = year%100;
    int d = b/4;
    int e = b%4;
    
    int f = (b +8)/25;
    int g = (b - f + 1)/3;
    int h = (19*a + b - d - g + 15)%30;
    int i = c/4;
    int k = c%4;
    int l = (32 + 2*e + 2*i - h - k)%7;
    int m = (a + 11*h + 22*l)/451;
    
    (*monthOut) = (unsigned char) ((h + l - 7*m + 114)/31);
    (*dayOut)   = (unsigned char)(((h + l - 7*m + 114)%31) + 1);
}

float
illuminatedFractionKForDate(int year, unsigned char month, float day)
{
    return illuminatedFractionK(julianYearFromTime(year, month, day));
}

unsigned char isWaxingForDate(int year, unsigned char month, float day)
{
    float illuminationNow = illuminatedFractionKForDate(year, month, day);
    float illuminationSoon = illuminationNow;

    // At this bad precision, even a full day may not come up with a differnt answer.  :b
    // So we scan forward looking for a change.
    while (illuminationSoon == illuminationNow) {
        day += 1.0;
        illuminationSoon = illuminatedFractionKForDate(year, month, day);
    }

    return (illuminationSoon > illuminationNow) ? 1 : 0;
}

unsigned char
dayOfTheWeek(int year, unsigned char month, unsigned char day)
{
  // from p65
    float JD = julianEphemerisDay(year, month, day);
  JD = JD + 1.5;
  return (unsigned char)floorf(JD)%7;
}

unsigned char computeHolidayBasedOnDayOfWeek(int year, unsigned char month, unsigned char dayOfWeek, unsigned dayOfWeekCount)
{
  unsigned char curDay = 1;
  unsigned char curDayOfWeek = dayOfTheWeek(year, month, curDay);
   
  while (curDay < 32 && dayOfWeekCount > 0) {
    if (curDayOfWeek == dayOfWeek) {
      dayOfWeekCount--;
      if (dayOfWeekCount == 0) {
	break;
      }
    }
    curDay++;
    curDayOfWeek = (curDayOfWeek + 1)%7;
  }
    
  if (curDay < 32) {
    return curDay;
  }

  // If you ask for the 54th Sunday of of June you will fail to find it.
  return UNKNOWN_DAY;
}
