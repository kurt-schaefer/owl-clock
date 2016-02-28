//
//  AstronomicalCalculations.h
//  OwlSim
//
//  This contains calculations from Jean Meeus' Astronomical Algorithms
//  for use on an Arduino.  This file can only do 4 byte precision, so
//  the results are bad, but not as bad as the huge bloat introduced
//  with my float64 calculations.  This does force me to ditch the
//  solar event calculations altogether.
//
//  Created by Kurt Schaefer on 2/15/16.
//  Copyright © 2016 Kurt Schaefer. All rights reserved.
//

#ifndef AstronomicalCalculations_h
#define AstronomicalCalculations_h


// In theory the day can have a factional part that should be included, but
// in practice the calculations don't seem to be good enough for .5 of a day to
// have an impact.  Saddly this means you can't really use the output of this
// to compute the exact hour of the peak of the new moon, etc.  It is however good
// enough for reasonable sense of how full the moon will be on a given day.
float illuminatedFractionKForDate(int year, unsigned char month, float day);

// This computes the illumination twice. With the given time, and that plus a day or so.
// Most of the time this gives a valid result, but may be incorrect very close to full/new moons.
// Thankfully at those times the moon waxing orienation is hard to see.
unsigned char isWaxingForDate(int year, unsigned char month, float day);


#define SUNDAY 0
#define MONDAY 1
#define TUESDAY 2
#define WEDNESDAY 3
#define THURSDAY 4
#define FRIDAY 5
#define SATURDAY 6
#define UNKNOWN_DAY 255

// Compute the day of the week for a given date
unsigned char dayOfTheWeek(int year, unsigned char month, unsigned char day);

// Compute the month/day of easter for a given year.
void easterForYear(int year, unsigned char *monthOut, unsigned char *dayOut);

// So if for example you wanted the second Sunday in May you would use (<year>, MAY, SUNDAY, 2)
// If this fails to find the requested day it returns 255 (UNKNOWN_DAY). This isn't super smart about month lengths/leap years, etc.
// So don't use it for that, but it's great for mothers/fathers day, etc.

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

unsigned char computeHolidayBasedOnDayOfWeek(int year, unsigned char month, unsigned char dayOfWeek, unsigned dayOfWeekCount);

// Martin Luther King      Third Monday in January
// President's Day         Third Monday in February
// Labor Day               First Monday in September
// Columbus Day            Second Monday in October
// Mother's Day            Second Sunday in May
// Father's Day            Third Sunday in June
// Election Day            Tuesday after first Monday



#endif /* AstronomicalCalculations_h */
