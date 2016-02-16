//
//  AstronomicalCalculations.h
//  OwlSim
//
//  This contains calculations from Jean Meeus' Astronomical Algorithms
//  for use on an Arduino.  This file MUST not use double because
//  the Arduino double is only 4 bytes, so we make use of the float64
//  math lib to improve the calculations precision some.  The results would
//  be better if we had real doubles.
//
//  Created by Kurt Schaefer on 2/15/16.
//  Copyright © 2016 Kurt Schaefer. All rights reserved.
//

#ifndef AstronomicalCalculations_h
#define AstronomicalCalculations_h

#include "float64.h"

// In theory the day can have a factional part that should be included, but
// in practice the calculations don't seem to be good enough for .5 of a day to
// have an impact.  Not sure if that's because we are using FAKE sin/cos or if
// it's something else.  Saddly this means you can't really use the output of this
// to compute the exact hour of the peak of the new moon, etc.  It is however good
// enough for reasonable sense of how full the moon will be on a given day.
float illuminatedFractionKForDate(int year, unsigned char month, float day);

// This computes the illumination twice. With the given time, and that plus a day.
// Most of the time this gives a valid result, but may be incorrect very close to full/new moons.
// Thankfully at those times the moon waxing orienation is hard to see.
unsigned char isWaxing(int year, unsigned char month, float day);

// Compute the Juliean Day Year (JDE) of the give event given a year.
float64 springEquinox(int year);
float64 summerSolstice(int year);
float64 fallEquinox(int year);
float64 winterSolstice(int year);

// This JDE should be a value returned from the above spring/summer/fall/winter Equinox/Solstice calculations.
// This is not as accurate as if actual doubles were used, but the values seem to only differ by only maybe
// 50 seconds from the values given in the book so I deam that good enough.  We only care about the days, so this
// will rarely be off except when the even in question is very close to midnight.
void computeDateFromSolarEventJDE(float64 JDE, int *yearOut, int *monthOut, float *dayOut);

#endif /* AstronomicalCalculations_h */
