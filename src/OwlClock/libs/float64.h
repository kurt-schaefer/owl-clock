//
//  float64.h
//  OwlSim
//
//  On the Arduino doubles are still only 4 bytes, but we need better precision in order to
//  do astronomical calculations like when the solstices are, etc.  In order to support that I
//  adapted this lib which gives you a float64 which is made up of two 4 byte floats, that can
//  be operated on at a higher precision.  (A bit more like a real double)
//  Read the included paper for more details.
//
//  Created by Kurt Schaefer on 2/3/16.
//  Copyright © 2016 Kurt Schaefer. All rights reserved.
//

#ifndef float64_h
#define float64_h

typedef struct {
    float x, y;
} float64;

// Take a float and split it into a float64 struct with two float pieces.
float64 split(float a);

// Merge the two float pieces back into a single float.
float merge(float64 a);

// Do these math operations at the higher precision.
float64 add64(float64 a,  float64 b);
float64 mult64(float64 a, float64 b);
float64 div64(float64 a, float64 b);
float64 sqrt64(float64 a);

// This really doesn't do a higher precision cos, it just does internal splits
// and merges to use the built in cos. If it turns out we need higher precision
// cos I'll try and write a real one of these and I can replace all the
// calls to cos64FAKE
float64 cos64FAKE(float64 a);
float64 sin64FAKE(float64 a);

#endif /* float64_h */
