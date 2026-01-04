#ifndef CONSTANTS_H
#define CONSTANTS_H

const double _PI = 3.14159265358979323846;
const double DEG_TO_RAD = _PI / 180.0;
const double RAD_TO_DEG = 180.0 / _PI;

inline double degreeToRadians(double degree) { return degree * DEG_TO_RAD; }
inline double radiansToDegrees(double radians) { return radians * RAD_TO_DEG; }

#endif