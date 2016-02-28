
//
//  AstronomicalCalculations.c
//  OwlSim
//
//  Created by Kurt Schaefer on 2/15/16.
//  Copyright © 2016 Kurt Schaefer. All rights reserved.
//

#include "AstronomicalCalculations.h"
#include "Math.h"

float64
moonsMeanAnomalyMPrime(float64 T)
{
    // From p308
    float64 TT = mult64(T, T);
    float64 TTT = mult64(T, TT);
    float64 TTTT = mult64(TT, TT);
    
    return add64(
                 add64(
                       add64(
                             add64(add64(split(134.96), split(0.0034114)),
                                   mult64(add64(split(477198), split(0.8676313)), T)),
                             mult64(add64(split(0.0089), split(0.0000970)), TT)),
                       div64(TTT, split(-69699.0))),
                 div64(TTTT, split(-14712000.0)));
    // 134.9634114 + 477198.8676313*T + 0.0089970*T*T - (T*T*T)/69699.0 - (T*T*T*T)/14712000.0;
}

float64
sunsMeanAnomalyM(float64 T)
{
    // From p308
    float64 TT = mult64(T, T);
    float64 TTT = mult64(T, TT);
    
    return add64(add64(add64(add64(split(357.52), split(0.0091092)),
                             mult64(add64(split(35999.05), split(0.002909)), T)),
                       mult64(split(0.0001536), TT)),
                 div64(TTT, split(24490000.0)));
    // 357.5291092 + 35999.0502909*T - 0.0001536*T*T + (T*T*T)/24490000.0;
}

float64
moonsMeanElongationD(float64 T)
{
    // From p308
    float64 TT = mult64(T, T);
    float64 TTT = mult64(T, TT);
    float64 TTTT = mult64(TT, TT);
    
    return add64(add64(add64(add64(add64(split(297.85), split(0.0002042)),
                                   mult64(add64(split(445267.1), split(0.0115168)), T)),
                             mult64(split(-0.0013268), TT)),
                       div64(TTT, split(545868.0))),
                 div64(TTTT, split(113065000.0)));
    // 297.8502042 + 445267.1115168*T - 0.0013268*T*T + (T*T*T)/545868.0 - (T*T*T*T)/113065000.0;
}

float64
moonsPhaseAngleInDegI(float64 T)
{
    // From p316
    float64 D = moonsMeanElongationD(T);
    float64 MPrime = moonsMeanAnomalyMPrime(T);
    float64 M = sunsMeanAnomalyM(T);
    
    float64 degToRad = div64(split(M_PI), split(180.0));
    
    return add64(add64(add64(add64(add64(add64(add64(split(180.0), mult64(split(-1.0),D)),
                                               mult64(split(-6.289), sin64FAKE(mult64(degToRad, MPrime)))),
                                         mult64(split(2.100), sin64FAKE(mult64(degToRad, M)))),
                                   mult64(split(-1.274), sin64FAKE(mult64(degToRad,
                                                                          add64(mult64(split(2.0), D),
                                                                                mult64(split(-1.0), MPrime)))))),
                             mult64(split(-0.658), sin64FAKE(mult64(degToRad, mult64(split(2.0), D))))),
                       mult64(split(-0.214), sin64FAKE(mult64(degToRad, mult64(split(2.0), MPrime))))),
                 mult64(split(-9.110), sin64FAKE(mult64(degToRad, D))));
    
    //(180.0 - D - 6.289*sin(degToRad*MPrime)
    //        + 2.100*sin(degToRad*M)
    //        - 1.274*sin(degToRad*(2.0*D - MPrime))
    //        - 0.658*sin(degToRad*2.0*D)
    //        - 0.214*sin(degToRad*2.0*MPrime)
    //        - 9.110*sin(degToRad*D));
}

float64
illuminatedFractionK(float64 T)
{
    float64 degToRad = div64(split(M_PI), split(180.0));
    return div64(add64(split(1.0), cos64FAKE(mult64(degToRad, moonsPhaseAngleInDegI(T)))), split(2.0));
    //(1.0 + cos(degToRad*moonsPhaseAngleInDegI(T)))/2.0;
}


// only day can have a decimal portion
float64
julianEphemerisDay(float64 year, float64 month, float64 day)
{
    // From p61
    if (merge(month) <= 2.0) {
        year = add64(year, split(-1.0)); // year - 1;
        month = add64(month,  split(12));
    }
    
    float64 A = split(floorf(merge(div64(year, split(100.0)))));  // floorf(year/100.0);
    float64 B = add64(add64(split(2.0), mult64(split(-1.0), A)), split(floorf(merge(div64(A, split(4.0)))))); // 2 - A + floorf(A/4.0)
    
    return add64(add64(add64(add64(mult64(split(365.25), add64(year, split(4716.0))),
                                   split(floorf(merge(mult64(split(30.6001), add64(month, split(1.0))))))), day), B), split(-1524.5));
    // floorf(365.25*(year + 4716.0)) + floorf(30.6001 * (month + 1.0)) + day + B - 1524.5;
}

// Compute the time in centuries since J2000 known as the Julian Year (T)
// We compute it using time_t time since 1970
float64
julianYearFromTime(float64 year, float64 month, float64 day)
{
    float64 JED = julianEphemerisDay(year, month, day);
    return div64(add64(JED, split(-2451545.0)),split(36525.0)); //(JED - 2451545.0)/36525.0;
}

float
illuminatedFractionKForDate(int year, unsigned char month, float day)
{
    return merge(illuminatedFractionK(julianYearFromTime(split((float)year), split((float)month), split(day))));
}

unsigned char isWaxingForDate(int year, unsigned char month, float day)
{
    float illuminationNow = illuminatedFractionKForDate(year, month, day);
    float illuminationSoon = illuminatedFractionKForDate(year, month, day + 1.3);

    return (illuminationSoon > illuminationNow) ? 1 : 0;
}

float64
yearFactorY(int year)
{
    // from p166
    return div64(add64(split(year), split(-2000.0)), split(1000.0));
}

float64
springEquinox(int year)
{
    // from p166
    float64 Y = yearFactorY(year);
    float64 YY = mult64(Y,Y);
    float64 YYY = mult64(YY,Y);
    float64 YYYY = mult64(YY,YY);
    
    return add64(add64(add64(add64(add64(split(2451623.0), split(0.80984)),
                                   mult64(add64(split(365242.0), split(0.37404)), Y)),
                             mult64(split(0.05169), YY)),
                       mult64(split(-0.00411), YYY)),
                 mult64(split(-0.00057), YYYY));
    
    // return 2451623.80984 + 365242.37404*Y + 0.05169*Y*Y - 0.00411*Y*Y*Y - 0.00057*Y*Y*Y*Y;
}

float64
summerSolstice(int year)
{
    // from p166
    float64 Y = yearFactorY(year);
    float64 YY = mult64(Y,Y);
    float64 YYY = mult64(YY,Y);
    float64 YYYY = mult64(YY,YY);
    
    return add64(add64(add64(add64(add64(split(2451716.0), split(0.56767)),
                                   mult64(add64(split(365241.0), split(0.62603)), Y)),
                             mult64(split(0.00325), YY)),
                       mult64(split(0.00888), YYY)),
                 mult64(split(-0.00030), YYYY));
    
    //    return 2451716.56767 + 365241.62603*Y + 0.00325*Y*Y + 0.00888*(Y*Y*Y) - 0.00030*(Y*Y*Y*Y);
}

float64
fallEquinox(int year)
{
    // from p166
    float64 Y = yearFactorY(year);
    float64 YY = mult64(Y,Y);
    float64 YYY = mult64(YY,Y);
    float64 YYYY = mult64(YY,YY);
    
    return add64(add64(add64(add64(add64(split(2451810.0), split(0.21715)),
                                   mult64(add64(split(365242.0), split(0.01767)), Y)),
                             mult64(split(-0.11575), YY)),
                       mult64(split(0.00337), YYY)),
                 mult64(split(0.00078), YYYY));
    //    return 2451810.21715 + 365242.01767*Y - 0.11575*Y*Y + 0.00337*Y*Y*Y + 0.00078*Y*Y*Y*Y;
}

float64
winterSolstice(int year)
{
    // from p166
    float64 Y = yearFactorY(year);
    float64 YY = mult64(Y,Y);
    float64 YYY = mult64(YY,Y);
    float64 YYYY = mult64(YY,YY);
    
    return add64(add64(add64(add64(add64(split(2451900.0), split(0.05952)),
                                   mult64(add64(split(365242.0), split(0.74049)), Y)),
                             mult64(split(-0.06223), YY)),
                       mult64(split(-0.00823), YYY)),
                 mult64(split(0.00032), YYYY));
    //    return 2451900.05952 + 365242.74049*Y - 0.06223*Y*Y - 0.00823*Y*Y*Y + 0.00032*Y*Y*Y*Y;
}

float64
julianYearFromJDE(float64 JDE)
{
    return  div64(add64(JDE, split(-2451545.00)), split(36525.0));
    // return (JDE - 2451545.0)/36525.0;
}

float64
addTerm(float64 S, float64 degToRad, float64 T, float64 A, float64 B, float64 C)
{
    return add64(S, mult64(A, cos64FAKE(mult64(degToRad, add64(B, mult64(C,T))))));
}

void
computeDateFromSolarEventJDE(float64 JDE, int *yearOut, int *monthOut, float *dayOut)
{
    // from p165
    float64 T = julianYearFromJDE(JDE);
    float64 degToRad = div64(split(M_PI), split(180.0));
    
    float64 W = add64(mult64(split(35999.373), T), split(-2.47)); // 35999.373*T - 2.47;
    float64 DLam = add64(add64(split(1.0), mult64(split(0.0344), cos64FAKE(mult64(degToRad, W)))),
                         mult64(split(0.0007), cos64FAKE(mult64(mult64(degToRad, split(2.0)), W))));
    //1.0 + 0.0334*cos(degToRad*W) + 0.0007*cos(degToRad*2.0*W);
    
    // from p167
    float64 S = split(0.0);
    
    S = addTerm(S, degToRad, T, split(485.0), split(324.96), split(1934));
    S = addTerm(S, degToRad, T, split(203.0), split(337.23), split(32964.467));
    S = addTerm(S, degToRad, T, split(199.0), split(342.08), split(20.186));
    S = addTerm(S, degToRad, T, split(182.0), split(27.85), split(445267.112));
    S = addTerm(S, degToRad, T, split(156.0), split(73.14), split(45036.886));
    S = addTerm(S, degToRad, T, split(136.0), split(171.52), split(22518.443));
    S = addTerm(S, degToRad, T, split(77.0), split(222.54), split(65928.934));
    S = addTerm(S, degToRad, T, split(74.0), split(296.72), split(3034.906));
    S = addTerm(S, degToRad, T, split(70.0), split(243.58), split(9037.513));
    S = addTerm(S, degToRad, T, split(58.0), split(119.81), split(33718.147));
    S = addTerm(S, degToRad, T, split(52.0), split(297.17), split(150.678));
    S = addTerm(S, degToRad, T, split(50.0), split(21.02), split(2281.226));
    S = addTerm(S, degToRad, T, split(45.0), split(247.54), split(29929.562));
    S = addTerm(S, degToRad, T, split(44.0), split(325.15), split(31555.956));
    S = addTerm(S, degToRad, T, split(29.0), split(60.93), split(4443.417));
    S = addTerm(S, degToRad, T, split(18.0), split(155.12), split(67555.328));
    S = addTerm(S, degToRad, T, split(17.0), split(288.79), split(4562.452));
    S = addTerm(S, degToRad, T, split(16.0), split(198.04), split(62894.029));
    S = addTerm(S, degToRad, T, split(14.0), split(199.76), split(31436.921));
    S = addTerm(S, degToRad, T, split(12.0), split(95.39), split(14577.848));
    S = addTerm(S, degToRad, T, split(12.0), split(287.11), split(31931.756));
    S = addTerm(S, degToRad, T, split(12.0), split(320.81), split(34777.259));
    S = addTerm(S, degToRad, T, split(9.0), split(227.73), split(1222.114));
    S = addTerm(S, degToRad, T, split(8.0), split(15.45), split(16859.074));
    
    JDE = add64(JDE, div64(mult64(split(0.00001), S), DLam)); //JDE + (0.00001*S)/DLam;
    
    // from p63
    float64 Z = split(floorf(merge(JDE) + 0.5));
    float64 F = add64(add64(JDE, split(0.5)), mult64(split(-1.0), Z));
    
    float64 A = Z;
    if (merge(Z) >= 2299161.0) {
        float alpha = floorf(merge(div64(add64(Z, split(-1867216.25)), split(36524.25)))); //floorf((Z - 1867216.25)/36524.25);
        A = add64(add64(add64(Z, split(1.0)), split(alpha)), split(-floorf(alpha/4.0)));  // Z + 1.0 + alpha - floorf(alpha/4.0);
    }
    
    float B = merge(A) + 1524.0;
    float C = floorf((B - 122.1)/365.25);
    float D = floorf(365.25*C);
    float E = floorf((B - D)/30.6001);
    
    (*dayOut) = B - D - floorf(30.6001*E) + merge(F);
    (*monthOut) = (E < 14) ? E - 1.0 : E - 13;
    (*yearOut) = ((*monthOut) > 2) ? C - 4716 : C - 4715;
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

unsigned char
dayOfTheWeek(int year, unsigned char month, unsigned char day)
{
  // from p65
  float JD = merge(julianEphemerisDay(split(year), split(month), split(day)));
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
