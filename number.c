#include "number.h"
#include <math.h>

Number num_from_mag_angle(float magnitude, float angle)
{
  Number n;
  n.magnitude = magnitude;
  n.angle = angle;

  n.real = magnitude * cos(angle);
  n.imaginary = magnitude * sin(angle);
  return n;
}

Number num_from_real_imaginary(float real, float imaginary)
{
  Number n;
  n.real = real;
  n.imaginary = imaginary;

  n.magnitude = sqrt(real * real + imaginary * imaginary);
  n.angle = atan2(imaginary, real);
  return n;
}

Number add(Number a, Number b)
{
  float real = a.real + b.real;
  float imaginary = a.imaginary + b.imaginary;
  return num_from_real_imaginary(real, imaginary);
}

Number subtract(Number a, Number b)
{
  Number negative_one = num_from_real_imaginary(-1, 0);
  return add(a, multiply(b, negative_one));
}

Number multiply(Number a, Number b)
{
  float magnitude = a.magnitude * b.magnitude;
  float angle = a.angle + b.angle;
  return num_from_mag_angle(magnitude, angle);
}

Number divide(Number numerator, Number denominator)
{
  if (denominator.magnitude == 0)
    return numerator;
  float magnitude = numerator.magnitude / denominator.magnitude;
  float angle = numerator.angle - denominator.angle;
  return num_from_mag_angle(magnitude, angle);
}