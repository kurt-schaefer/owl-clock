//
//  float64.c
//  OwlSim
//
//  Created by Kurt Schaefer on 2/3/16.
//  Copyright © 2016 Kurt Schaefer. All rights reserved.
//

#include "math.h"
#include "float64.h"

float64
split(float a)
{
    float64 result;
    result.x = a;
    result.y = 0.0;
    
    return result;
    
#if 0
    const float split = 4097; // (1 << 12) + 1;
    float t = a*split;
    float ahi = t - (t - a );
    float alo = a - ahi;
    
    float64 result;
    result.x = ahi;
    result.y = alo;
    
    return result;
#endif
}

float
merge(float64 a)
{
    return a.x;
}

float64
quickTwoSum (float a, float b)
{
    float s = a + b ;
    float e = b - (s - a) ;
    
    float64 result;
    result.x = s;
    result.y = e;
    return result;
}

float64
twoSum(float a,  float b)
{
    float s = a + b ;
    float v = s - a ;
    float e = ( a - ( s - v ) ) + ( b - v ) ;
    
    float64 result;
    result.x = s;
    result.y = e;
    return result;
}

float64
add64(float64 a,  float64 b)
{
    float64 s, t;
    s = twoSum( a.x , b.x);
    t = twoSum( a.y , b.y);
    s.y += t.x ;
    s = quickTwoSum(s.x, s.y);
    s.y += t.y;
    s = quickTwoSum(s  .x, s.y);
    return s;
}

//void
//doubleSplit(double a, float *aHi, float *aLo)
//{
//    double t = a*((1 << 29) + 1);
//    double tHi = t - (t - a);
//    double tLo = a - tHi;
//    *aHi = (float)tHi;
//    *aLo = (float)tLo;
//}

float64
twoProd(float a, float b)
{
    float p = a*b ;
    float64 aS = split(a);
    float64 bS = split(b);
    float err = ((aS.x*bS.x - p) + aS.x*bS.y + aS.y*bS.x) + aS.y*bS.y;
    
    float64 result;
    result.x = p;
    result.y = err;
    
    return result;
}

float64
mult64(float64 a, float64 b)
{
    float64 p;
    p = twoProd(a.x, b.x);
    p.y += a.x*b.y;
    p.y += a.y*b.x;
    p = quickTwoSum(p.x, p.y);
    return p ;
}

float64
div64(float64 b, float64 a)
{
    float xn = 1.0/a.x;
    float yn = b.x*xn;
    float diffTerm = (add64(b, mult64(a, split(-yn)))).x;
    float64 prodTerm = twoProd(xn, diffTerm);
    return add64(split(yn), prodTerm);
}

float64
sqrt64(float64 a)
{
    float xn = sqrt(a.x);
    float yn = a.x*xn ;
    float64 ynsqr = mult64(split(yn),split(yn));
    float diff = add64(a, mult64(split(-1.0), ynsqr)).x;
    float64 prodTerm = div64(twoProd(xn, diff), split(2.0));
    return add64(split(yn), prodTerm);
}

float64
cos64FAKE(float64 a)
{
    return split(cos(merge(a)));
    // I could not get the papers high precision cos/sin code to work, it just gets
    // trapped in the while loop forever.  If this becomes an issue I can try working on
    // it again.
    
#ifdef USE_HIGH_PRECISION_COS
    const  float thresh = 1.0E-20 * fabs(a.x);
    float64 t; /* Term being added. */
    float64 p; /* Current power of a. */
    float64 f; /* Denominator. */
    float64 s; /* Current partial sum. */
    float64 x; /* = −s q r ( a ) */
    float m;
    float64 sina , cosa;
    
    if (a.x == 0.0f) {
        sina.x = 0.0;
        sina.y = 0.0;
        cosa.x = 1.0;
        cosa.y = 0.0;
    } else {
        x = mult64(split(-1.0), mult64(a, a));
        s = a;
        p = a;
        m = 1.0;
        f.x = 1.0;
        f.y = 0.0; //=  float2 (ONE, ZERO);
        while (true) {
            p = mult64(p, x);
            m += 2.0;
            f = mult64(f, split(m*(m - 1.0)));
            t = div64(p, f);
            s = add64(s, t);
            if (fabs(t.x) < thresh) {
                break ;
            }
        }
        sina = s;
        cosa = sqrt64(add64(split(1.0), mult64(split(-1.0), mult64(s,s))));
    }
    return cosa;
#endif
}

// See above
float64
sin64FAKE(float64 a)
{
    return split(sin(merge(a)));
}
